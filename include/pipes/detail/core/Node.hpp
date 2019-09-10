#pragma once

#include <type_traits>
#include <tuple>

#include "pipes/detail/util/metaprogramming.hpp"
#include "pipes/detail/util/ignoreWarning.hpp"
#include "pipes/detail/util/FWD.hpp"

#include "pipes/detail/core/traits.hpp"

namespace tillh
{
  namespace pipes
  {
    template<class OpT, class ConnectionsT>
    struct Node
    {
      using Op = OpT;
      using Connections = ConnectionsT;

      template<class Connections_, class Op_>
      Node(Connections_&& _connections, Op_&& _op):
        connections(std::forward<Connections_>(_connections)), op(std::forward<Op_>(_op))
      {}

      template<class... Rhss>
      auto operator()(Rhss&& ... rhss) const
      {
        return connectSecondary(*this, FWD(rhss)...);
      }

      Connections connections;
      Op op;
    };

    struct OpenConnectionPlaceHolder {};
    struct PrimaryOpenConnectionPlaceHolder {};

    template<class Op, class Connections>
    auto makeNodeDirect(Op op, Connections connections)
    {
      return Node<Op, Connections>(std::move(connections), std::move(op));
    }


    // kinda hacky implementation. Im sure there is a better way to create a tuple of N identical objects
    template<std::size_t... Is>
    constexpr auto makeOpenConnections(std::index_sequence<Is... >)
    {
      // gcc complains about unused value Is infront of comma operator, which is needed for expansion
      DISABLE_WARNING_PUSH;
      DISABLE_WARNING_UNUSED_VALUE;
      DISABLE_WARNING_MISUSED_COMMA;
      return std::make_tuple((Is, OpenConnectionPlaceHolder())...);
      DISABLE_WARNING_POP;
    }


    template<bool hasPrimaryConnection, std::size_t secondaryConnections, class Op>
    auto makeNode(Op op)
    {
      constexpr auto makePrimary = []
      {
        if constexpr(hasPrimaryConnection)
        {
          return std::make_tuple(PrimaryOpenConnectionPlaceHolder());
        }
        else
        {
          return std::tuple<>();
        }
      };
      return makeNodeDirect(op, std::tuple_cat(makeOpenConnections(std::make_index_sequence<secondaryConnections>()), makePrimary()));
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

    template<std::size_t index, class Op, class Connections, class F>
    auto apply(Node<Op, Connections>&& node, F f)
    {
      return replace<index>(std::move(node), f(std::get<index>(std::move(node).connections)));
    }
    template<class Op, class Connections, class F>
    auto apply_last(Node<Op, Connections>&& node, F f)
    {
      return apply<util::tuple_back<Connections>>(std::move(node), f);
    }
  }
}