#pragma once

#include "pipes2/detail/core/Node.hpp"
#include "pipes2/detail/core/Output.hpp"

namespace tillh::pipes2
{
  template<class Op, class Connections, class PrimaryConnection>
  auto evaluate(Node<Op, Connections, PrimaryConnection>&& node)
  {
    using NodeT = Node<Op, Connections, PrimaryConnection>;
    static_assert(!canSecondaryConnect(Type<NodeT>()));
    static_assert(!canPrimaryConnect(Type<NodeT>()));

    if constexpr(hasPrimary(Type<NodeT>()))
    {
      return makeOutput(std::move(node.op), std::tuple_cat(std::move(node.connections), std::make_tuple(std::move(node.connectPrimaryion))));
    }
    else
    {
      return makeOutput(std::move(node.op), std::move(node.connections));
    }
  }
}
