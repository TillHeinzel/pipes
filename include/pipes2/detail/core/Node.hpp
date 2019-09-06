#pragma once

#include <type_traits>
#include <tuple>

#include "pipes2/detail/util/metaprogramming.hpp"
#include "pipes2/detail/util/ignoreWarning.hpp"

#include "pipes2/detail/core/traits.hpp"

namespace tillh::pipes2
{
  template<class OpT, class ConnectionsT, class PrimaryConnectionT>
  struct Node
  {
    using Op = OpT;
    using Connections = ConnectionsT;
    using PrimaryConnection = PrimaryConnectionT;

    template<class Connections_, class PrimaryConnection_, class Op_>
    Node(Connections_&& _connections, PrimaryConnection_&& _connectPrimaryion,
         Op_&& _op): primaryConnection(std::forward<PrimaryConnection_>(_connectPrimaryion)), connections(
           std::forward<Connections_>(_connections)),
      op(std::forward<Op_>(_op))
    {}

    PrimaryConnection primaryConnection;
    Connections connections;
    Op op;
  };

  struct NoPrimary {};
  struct OpenConnectionPlaceHolder {};
}

namespace tillh::pipes2
{
  template<class Op, class Connections, class PrimaryConnection>
  auto makeNode(Op op, Connections connections, PrimaryConnection primaryConnection)
  {
    return Node<Op, Connections, PrimaryConnection>(std::move(connections), std::move(primaryConnection), std::move(op));
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
    constexpr auto makePrimary = [] {if constexpr(hasPrimaryConnection) { return OpenConnectionPlaceHolder(); } else { return NoPrimary(); }};
    return makeNode(op, makeOpenConnections(std::make_index_sequence<secondaryConnections>()), makePrimary());
  }
}
