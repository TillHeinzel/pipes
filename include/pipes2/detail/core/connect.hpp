#pragma once

#include <type_traits>
#include <tuple>

#include "pipes2/detail/util/metaprogramming.hpp"
#include "pipes2/detail/util/FWD.hpp"

#include "pipes2/detail/core/Node.hpp"
#include "pipes2/detail/core/Output.hpp"
#include "pipes2/detail/core/Input.hpp"
#include "pipes2/detail/core/evaluate.hpp"
#include "pipes2/detail/core/traits.hpp"

namespace tillh::pipes2
{
  template<class T>
  struct openCountT : int_constant<0> {};

  template<>
  struct openCountT<OpenConnectionPlaceHolder> : int_constant<1> {};

  template<class Op, class... Connections, class Primary>
  struct openCountT<Node<Op, std::tuple<Connections...>, Primary>> : sum<openCountT<Connections>...> {};

  template<class T>
  constexpr static std::size_t openCount = openCountT<T>::value;
}

namespace tillh::pipes2
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
namespace tillh::pipes2
{
  enum class ConnectMode{Primary, Secondary};

  template<ConnectMode mode>
  using mode_constant = std::integral_constant<ConnectMode, mode>;

  using primary_constant = mode_constant<ConnectMode::Primary>;
  using secondary_constant = mode_constant<ConnectMode::Secondary>;

  template<class Connection, std::size_t... Indices, class NewConnects>
  auto connectIndices(Connection&& connection, std::index_sequence<Indices...>, NewConnects newConnects)
  {
    if constexpr(sizeof...(Indices) == 0)
    {
      return connection;
    }
    else
    {
      return connect(secondary_constant(), std::forward<Connection>(connection), std::get<Indices>(newConnects)...);
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
namespace tillh::pipes2
{
  template<class Node, class... Children>
  auto connectSecondary(Node&& node, Children&& ... children)
  {
    static_assert(sizeof...(children) > 0);
    static_assert(sizeof...(children) <= openCount<remove_cv_ref_t<Node>>);
    auto indices = getIndices<sizeof...(children), remove_cv_ref_t<Node>>();
    return makeNode(std::forward<Node>(node).op, connectIndexWise(std::forward<Node>(node).connections, indices, std::forward_as_tuple(children...)), std::forward<Node>(node).primaryConnection);
  }

  template<class Node, class Child>
  auto connectPrimary2(Node&& node, Child&& child)
  {
    return makeNode(FWD(node).op, FWD(node).connections, connect(primary_constant(), FWD(node).primaryConnection, FWD(child)));
  }
  
  template<ConnectMode mode, class Lhs, class... Rhss>
  auto connect(mode_constant<mode>, Lhs&& lhs, Rhss&& ... rhss)
  {
    static_assert(sizeof...(Rhss) > 0, "at least 1 rhs needed");

    if constexpr(std::is_same_v<remove_cv_ref_t<Lhs>, OpenConnectionPlaceHolder>)
    {
      static_assert(sizeof...(Rhss) == 1, "there should never be a call to connect one placeholder with multiple nodes");

      return getFirst(FWD(rhss)...);
    }
    else
    {
      if constexpr(mode == ConnectMode::Secondary)
      {
        return evaluateIfFinished(connectSecondary(FWD(lhs), FWD(rhss)...));
      }
      else if(mode == ConnectMode::Primary)
      {
        static_assert(sizeof...(Rhss) == 1, "can only primaryconnect 1 at a time");
        return evaluateIfFinished(connectPrimary2(FWD(lhs), FWD(rhss)...));
      }
    }
  }

  template<ConnectMode mode, class Source, class Node, class... Rhss>
  auto connect(mode_constant<mode> m, Input<Source, Node> lhs, Rhss&& ... rhss)
  {
    return makeInput(std::move(lhs.source), connect(m, std::move(lhs.node), FWD(rhss)...));
  }
}
