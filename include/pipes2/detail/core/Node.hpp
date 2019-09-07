#pragma once

#include <type_traits>
#include <tuple>

#include "pipes2/detail/util/metaprogramming.hpp"
#include "pipes2/detail/util/ignoreWarning.hpp"
#include "pipes2/detail/util/FWD.hpp"

#include "pipes2/detail/core/traits.hpp"

namespace tillh::pipes2
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
}

namespace tillh::pipes2
{
  template<class Op, class Connections>
  auto makeNode(Op op, Connections connections)
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
    return makeNode(op, std::tuple_cat(makeOpenConnections(std::make_index_sequence<secondaryConnections>()), makePrimary()));
  }
}
