#pragma once

#include "pipes/detail/core/Node.hpp"
#include "pipes/detail/core/Output.hpp"

#include "pipes/detail/core/traits.hpp"

namespace tillh
{
  namespace pipes
  {
    template<class Op, class Connections>
    auto evaluate(Node<Op, Connections>&& node)
    {
      using NodeT = Node<Op, Connections>;
      static_assert(!canSecondaryConnect<NodeT>);
      static_assert(!canPrimaryConnect<NodeT>);

      return makeOutput(std::move(node.op), std::move(node.connections));
    }

    template<class Op, class Connections>
    auto evaluateIfFinished(Node<Op, Connections> node)
    {
      if constexpr(!canPrimaryConnect<Node<Op, Connections>> && !canSecondaryConnect<Node<Op, Connections>>)
      {
        return evaluate(std::move(node));
      }
      else
      {
        return node;
      }
    }
       
    template<class Op, class Connections>
    auto makeNode(Op&& op, Connections&& connections)
    {
      return evaluateIfFinished(makeNodeDirect(FWD(op), FWD(connections)));
    }
  }
}
