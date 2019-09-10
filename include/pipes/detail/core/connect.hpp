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

#include "pipes/detail/core/openCount.hpp"
#include "pipes/detail/core/getIndices.hpp"

namespace tillh
{
  namespace pipes
  {
    enum class ConnectMode { Primary, Secondary };

    template<ConnectMode mode>
    using mode_constant = std::integral_constant<ConnectMode, mode>;

    using primary_constant = mode_constant<ConnectMode::Primary>;
    using secondary_constant = mode_constant<ConnectMode::Secondary>;

    template<class T>
    decltype(auto) clearPrimary(T&& t)
    {
      return FWD(t);
    }

    template<std::size_t index, class Op, class Connections, class Child >
    auto replace(Node<Op, Connections>&& node, Child&& child)
    {
      return makeNode(std::move(node.op), util::tuple_replace<index>(std::move(node.connections), FWD(child)));
    }

    template<class Op, class Connections, class Child >
    auto replace_last(Node<Op, Connections>&& node, Child&& child)
    {
      constexpr auto lastIndex = util::tuple_back<Connections>;
      return replace<lastIndex>(std::move(node), FWD(child));
    }

    inline auto clearPrimary(PrimaryOpenConnectionPlaceHolder)
    {
      return OpenConnectionPlaceHolder();
    }

    template<class Op, class Connections>
    auto clearPrimary(Node<Op, Connections>&& node)
    {
      return makeNode(std::move(node.op), util::tuple_replace<util::tuple_back<Connections>>(std::move(node.connections), clearPrimary(util::getLast(node.connections))));
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
      constexpr auto connectionCount = std::tuple_size_v<util::remove_cv_ref_t<Connections>>;
      static_assert(std::tuple_size_v<Indices> == connectionCount);
      return connectIndexWise_impl(std::forward<Connections>(connections), indices, newConnects, std::make_index_sequence<connectionCount>());
    }

    template<class Node, class... Children>
    auto connectSecondaryImpl(Node&& node, Children&& ... children)
    {
      static_assert(sizeof...(children) > 0);
      static_assert(sizeof...(children) <= openCount<util::remove_cv_ref_t<Node>>);
      auto indices = getIndices<sizeof...(children), util::remove_cv_ref_t<Node>>();
      return makeNode(std::forward<Node>(node).op, connectIndexWise(std::forward<Node>(node).connections, indices, std::forward_as_tuple(children...)));
    }
  }
}

namespace tillh
{
  namespace pipes
  {
    template<class Child>
    decltype(auto) connectPrimaryImpl(PrimaryOpenConnectionPlaceHolder, Child&& child)
    {
      return FWD(child);
    }
    template<class Child>
    auto connectPrimaryImpl(OpenConnectionPlaceHolder, Child&& child)
    {
      static_assert(util::fail_assert<Child>, "bug: cannot primary connect to secondary placeholder");
    }
    
    template<class Op, class Connections, class Child>
    auto connectPrimaryImpl(Node<Op, Connections>&& node, Child&& child)
    {
      auto newNode = replace_last(std::move(node), connectPrimaryImpl(util::getLast(std::move(node.connections)), FWD(child)));
      return evaluateIfFinished(std::move(newNode));
    }
  }
}

namespace tillh
{
  namespace pipes
  {
    template<ConnectMode mode, class Lhs, class... Rhss>
    auto connectImpl(mode_constant<mode>, Lhs&& lhs, Rhss&& ... rhss)
    {
      static_assert(sizeof...(Rhss) > 0, "at least 1 rhs needed");

      if constexpr(mode == ConnectMode::Secondary)
      {
        if constexpr(std::is_same_v<util::remove_cv_ref_t<Lhs>, OpenConnectionPlaceHolder>)
        {
          static_assert(sizeof...(Rhss) == 1, "there should never be a call to connect one placeholder with multiple nodes");
          return util::getFirst(FWD(rhss)...);
        }
        else
        {
          return evaluateIfFinished(connectSecondaryImpl(FWD(lhs), FWD(rhss)...));
        }
      }
      else if(mode == ConnectMode::Primary)
      {
        static_assert(sizeof...(Rhss) == 1, "can only primaryconnect 1 at a time");
        return connectPrimaryImpl(FWD(lhs), FWD(rhss)...);
      }
    }

    template<ConnectMode mode, class Source, class Node, class... Rhss>
    auto connectImpl(mode_constant<mode> m, Input<Source, Node> lhs, Rhss&& ... rhss)
    {
      auto result = makeInput(std::move(lhs.source), connectImpl(m, std::move(lhs.node), FWD(rhss)...));

      if constexpr(isCompleted<decltype(result)>)
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
}

namespace tillh
{
  namespace pipes
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
}
