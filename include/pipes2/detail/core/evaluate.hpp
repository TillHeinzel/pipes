#pragma once

#include "pipes2/detail/core/Node.hpp"
#include "pipes2/detail/core/Output.hpp"

#include "pipes2/detail/core/traits.hpp"

namespace tillh::pipes2
{
  template<class Op, class Connections, class PrimaryConnection>
  auto evaluate(Node<Op, Connections, PrimaryConnection>&& node)
  {
    using NodeT = Node<Op, Connections, PrimaryConnection>;
    static_assert(!canSecondaryConnect<NodeT>);
    static_assert(!canPrimaryConnect<NodeT>);

    if constexpr(hasPrimary_v<NodeT>)
    {
      return makeOutput(std::move(node.op), std::tuple_cat(std::move(node.connections), std::make_tuple(std::move(node.primaryConnection))));
    }
    else
    {
      return makeOutput(std::move(node.op), std::move(node.connections));
    }
  }

  template<class Node>
  auto evaluateIfFinished(Node node)
  {
    static_assert(!is_output_v<Node>);
    if constexpr(!canPrimaryConnect<Node> && !canSecondaryConnect<Node>)
    {
      return evaluate(std::move(node));
    }
    else
    {
      return node;
    }
  }
}
