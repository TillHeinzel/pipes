#pragma once

#include <type_traits>
#include <tuple>

#include "pipes/detail/util/metaprogramming.hpp"
#include "pipes/detail/util/FWD.hpp"

#include "pipes/detail/core/Node.hpp"
#include "pipes/detail/core/Output.hpp"
#include "pipes/detail/core/Input.hpp"
#include "pipes/detail/core/evaluate.hpp"
#include "pipes/detail/core/traits.hpp"

namespace tillh::pipes
{
  template<class T>
  struct openCountT : int_constant<0> {};

  template<>
  struct openCountT<OpenConnectionPlaceHolder> : int_constant<1> {};

  template<class Op, class... Connections>
  struct openCountT<Node<Op, std::tuple<Connections...>>> : sum<openCountT<Connections>...> {};

  template<class T>
  constexpr static std::size_t openCount = openCountT<T>::value;
}

namespace tillh::pipes
{
  template<std::size_t add, std::size_t... Is>
  constexpr auto addTo(std::index_sequence<Is...>)
  {
    return std::index_sequence<(Is + add)...>();
  }

  template<std::size_t accu, std::size_t max, std::size_t currentCount>
  constexpr auto getIndices_x()
  {
    if constexpr(accu >= max)
    {
      return std::make_index_sequence<0>();
    }
    else
    {
      return addTo<accu>(std::make_index_sequence<std::min(currentCount, max - accu)>());
    }
  }

  template<std::size_t max, std::size_t accu>
  constexpr auto getIndices_impl()
  {
    return std::tuple<>();
  }

  template<std::size_t max, std::size_t accu, class T, class... Ts>
  constexpr auto getIndices_impl(Type<T>, Type<Ts>...)
  {
    return std::tuple_cat(std::make_tuple(getIndices_x<accu, max, openCount<T>>()), getIndices_impl<max, accu + openCount<T>>(Type<Ts>()...));
  }

  template<std::size_t max, class... Ts>
  constexpr auto getIndices(TypeList<Ts...>)
  {
    return getIndices_impl<max, 0>(Type<Ts>()...);
  }

  template<std::size_t max, class Node>
  constexpr auto getIndices()
  {
    static_assert(max <= openCount<Node>);

    return getIndices<max>(
      typeList(
        Type<typename Node::Connections>()
      ));
  }
}

namespace tillh::pipes
{
  enum class ConnectMode { Primary, Secondary, Any };

  template<ConnectMode mode>
  using mode_constant = std::integral_constant<ConnectMode, mode>;

  using primary_constant = mode_constant<ConnectMode::Primary>;
  using secondary_constant = mode_constant<ConnectMode::Secondary>;
  using any_constant = mode_constant<ConnectMode::Secondary>;

  template<class T>
  decltype(auto) clearPrimary(T&& t)
  {
    return FWD(t);
  }

  inline auto clearPrimary(PrimaryOpenConnectionPlaceHolder)
  {
    return OpenConnectionPlaceHolder();
  }

  template<class Op, class Connections>
  auto clearPrimary(Node<Op, Connections>&& node)
  {
    return makeNode(std::move(node.op), tuple_replace<tuple_back<Connections>>(std::move(node.connections), clearPrimary(getLast(node.connections))));
  }

  template<class Connection, std::size_t... Indices, class NewConnects>
  auto connectIndices(Connection&& connection, std::index_sequence<Indices...>, NewConnects newConnects)
  {
    if constexpr(sizeof...(Indices) == 0)
    {
      return connection;
    }
    else
    {
      return clearPrimary(connectImpl(secondary_constant(), std::forward<Connection>(connection), std::get<Indices>(newConnects)...));
    }
  }

  template<class Connections, class... Indices, class NewConnects, std::size_t... Is>
  auto connectIndexWise_impl(Connections&& connections, std::tuple<Indices...> indices, NewConnects newConnects, std::index_sequence<Is...>)
  {
    return std::make_tuple(connectIndices(std::get<Is>(std::forward<Connections>(connections)), std::get<Is>(indices), newConnects)...);
  }

  template<class Connections, class Indices, class NewConnects>
  auto connectIndexWise(Connections&& connections, Indices indices, NewConnects newConnects)
  {
    constexpr auto connectionCount = std::tuple_size_v<remove_cv_ref_t<Connections>>;
    static_assert(std::tuple_size_v<Indices> == connectionCount);
    return connectIndexWise_impl(std::forward<Connections>(connections), indices, newConnects, std::make_index_sequence<connectionCount>());
  }
}

namespace tillh::pipes
{
  template<class Node, class... Children>
  auto connectSecondaryImpl(Node&& node, Children&& ... children)
  {
    static_assert(sizeof...(children) > 0);
    static_assert(sizeof...(children) <= openCount<remove_cv_ref_t<Node>>);
    auto indices = getIndices<sizeof...(children), remove_cv_ref_t<Node>>();
    return makeNode(std::forward<Node>(node).op, connectIndexWise(std::forward<Node>(node).connections, indices, std::forward_as_tuple(children...)));
  }

  template<class Node, class Child>
  auto connectPrimaryImpl(Node&& node, Child&& child)
  {
    static_assert(canPrimaryConnect<remove_cv_ref_t<Node>>);
    constexpr std::size_t lastIndex = tuple_back<typename remove_cv_ref_t<Node>::Connections>;
    return makeNode(FWD(node).op, tuple_replace<lastIndex>(FWD(node).connections, connectImpl(primary_constant(), getLast(FWD(node).connections), FWD(child))));
  }

  template<ConnectMode mode, class Lhs, class... Rhss>
  auto connectImpl(mode_constant<mode>, Lhs&& lhs, Rhss&& ... rhss)
  {
    static_assert(sizeof...(Rhss) > 0, "at least 1 rhs needed");

    if constexpr(mode == ConnectMode::Secondary)
    {
      if constexpr(std::is_same_v<remove_cv_ref_t<Lhs>, OpenConnectionPlaceHolder>)
      {
        static_assert(sizeof...(Rhss) == 1, "there should never be a call to connect one placeholder with multiple nodes");
        return getFirst(FWD(rhss)...);
      }
      else
      {
        return evaluateIfFinished(connectSecondaryImpl(FWD(lhs), FWD(rhss)...));
      }
    }
    else if(mode == ConnectMode::Primary)
    {
      if constexpr(std::is_same_v<remove_cv_ref_t<Lhs>, PrimaryOpenConnectionPlaceHolder>)
      {
        static_assert(sizeof...(Rhss) == 1, "there should never be a call to connect one placeholder with multiple nodes");
        return getFirst(FWD(rhss)...);
      }
      else
      {
        static_assert(sizeof...(Rhss) == 1, "can only primaryconnect 1 at a time");
        return evaluateIfFinished(connectPrimaryImpl(FWD(lhs), FWD(rhss)...));
      }
    }

  }

  template<ConnectMode mode, class Source, class Node, class... Rhss>
  auto connectImpl(mode_constant<mode> m, Input<Source, Node> lhs, Rhss&& ... rhss)
  {
    auto result = makeInput(std::move(lhs.source), connectImpl(m, std::move(lhs.node), FWD(rhss)...));

    if constexpr(detail::isCompleted<decltype(result)>)
    {
      result.source.run(result.node);
      return;
    }
    else
    {
      return result;
    }
  }
}

namespace tillh::pipes
{
  template<class T> struct ensureValidOutputT;

  template<class T>
  ensureValidOutputT<T&&> ensureValidOutputF(T&& t);

  template<class T> struct ensureValidInputT;

  template<class T>
  ensureValidInputT<T&&> ensureValidInputF(T&& t);

  template<class Lhs, class... Rhs>
  auto connectPrimary(Lhs&& lhs, Rhs&& ... rhs)
  {
    return connectImpl(primary_constant(), ensureValidInputF(FWD(lhs))(), ensureValidOutputF(FWD(rhs))()...);
  }

  template<class Lhs, class... Rhs>
  auto connectSecondary(Lhs&& lhs, Rhs&& ... rhs)
  {
    return connectImpl(secondary_constant(), ensureValidInputF(FWD(lhs))(), ensureValidOutputF(FWD(rhs))()...);
  }
}
