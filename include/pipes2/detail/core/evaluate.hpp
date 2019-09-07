#pragma once

#include "pipes2/detail/core/Node.hpp"
#include "pipes2/detail/core/Output.hpp"

#include "pipes2/detail/core/traits.hpp"

namespace tillh::pipes2
{
  template<class Op, class Connections>
  auto evaluate(Node<Op, Connections>&& node)
  {
    using NodeT = Node<Op, Connections>;
    static_assert(!canSecondaryConnect<NodeT>);
    static_assert(!canPrimaryConnect<NodeT>);

    return makeOutput(std::move(node.op), std::move(node.connections));
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
