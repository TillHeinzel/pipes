#pragma once

#include <type_traits>
#include <tuple>

#include "pipes2/detail/util/metaprogramming.hpp"

#include "pipes2/detail/core/Node.hpp"
#include "pipes2/detail/core/Output.hpp"
#include "pipes2/detail/core/Input.hpp"
#include "pipes2/detail/core/evaluate.hpp"
#include "pipes2/detail/core/traits.hpp"

namespace tillh::pipes2
{
  template<class... Ts>
  constexpr static bool canSecondaryConnectAny(Type<std::tuple<Ts...>>)
  {
    return (canSecondaryConnect(Type<Ts>()) || ...);
  }

  template<class T>
  constexpr static bool canSecondaryConnect(Type<T>)
  {
    static_assert(fail_assert<T>::value);
    return false;
  }

  constexpr static bool canSecondaryConnect(Type<OpenConnectionPlaceHolder>)
  {
    return true;
  }

  template<class Op, class Connections, class PrimaryConnection>
  constexpr static bool canSecondaryConnect(Type<Node<Op, Connections, PrimaryConnection>>)
  {
    return canSecondaryConnectAny(Type<Connections>());
  }


  template<class Op, class Connections>
  constexpr static bool canSecondaryConnect(Type<Output<Op, Connections>>)
  {
    return false;
  }
}

namespace tillh::pipes2
{
  template<std::size_t index, class Node>
  constexpr std::size_t firstOpen_impl(const Type<Node> node)
  {
    if constexpr(index >= std::tuple_size_v<typename Node::Connections>)
    {
      return index;
    }
    else
    {
      if constexpr(canSecondaryConnect(Type<std::tuple_element_t<index, typename Node::Connections>>()))
      {
        return index;
      }
      else
      {
        return firstOpen_impl<index + 1>(node);
      }
    }
  }

  template<std::size_t index, class Node>
  constexpr std::size_t firstPlaceholder_impl(const Type<Node>)
  {
    if constexpr(index >= std::tuple_size_v<typename Node::Connections>)
    {
      return index;
    }
    else
    {
      using ConnectionAtIndex = std::tuple_element_t<index, typename Node::Connections>;
      if constexpr(std::is_same_v<ConnectionAtIndex, OpenConnectionPlaceHolder>)
      {
        return index;
      }
      else
      {
        return firstOpen_impl<index + 1>(Type<Node>());
      }
    }
  }

  template<class Node>
  constexpr std::size_t firstOpen(const Type<Node> node)
  {
    static_assert(!is_output_v<Node>);
    return firstOpen_impl<0>(node);
  }

  template<class Node>
  constexpr std::size_t firstPlaceholder(const Type<Node>)
  {
    return firstPlaceholder_impl<0>(Type<Node>());
  }

  template<std::size_t index, class Node1, class Node2>
  auto replace(Node1 node1, Node2 node2)
  {
    return makeNode(node1.op, tuple_replace<index>(node1.connections, node2), node1.connectPrimaryion);
  }

  template<class Node>
  auto evaluateIfFinished(Node node)
  {
    static_assert(!is_output_v<Node>);
    if constexpr(!canPrimaryConnect_v<Node> && !canSecondaryConnect(Type<Node>()))
    {
      return evaluate(std::move(node));
    }
    else
    {
      return node;
    }
  }

  template<class Node>
  auto connectSecondary(OpenConnectionPlaceHolder, Node rhs)
  {
    return rhs;
  }

  template<class Node1, class Node2>
  auto connectSecondary(Node1 lhs, Node2 rhs)
  {
    static_assert(!is_output_v<Node1>);
    static_assert(!std::is_same_v<Node1, OpenConnectionPlaceHolder>);
    constexpr auto index = firstOpen(Type<Node1>());
    static_assert(index < std::tuple_size_v<typename Node1::Connections>);
    const auto& connection = std::get<index>(lhs.connections);
    return evaluateIfFinished(replace<index>(lhs, connectSecondary(connection, rhs)));
  }

  template<class Source, class InputNode, class Node>
  auto connectSecondary(Input<Source, InputNode>&& lhs, Node rhs)
  {
    return makeInput(std::move(lhs.source), connectSecondary(lhs.node, rhs));
  }

  template<class Node>
  auto connectSecondaryNonRecursive(OpenConnectionPlaceHolder, Node rhs)
  {
    return rhs;
  }

  template<class Node1, class Node2>
  auto connectSecondaryNonRecursive(Node1 lhs, Node2 rhs)
  {
    static_assert(!is_output_v<Node1>);
    static_assert(!std::is_same_v<Node1, OpenConnectionPlaceHolder>);
    constexpr auto index = firstPlaceholder(Type<Node1>());
    static_assert(index < std::tuple_size_v<decltype(lhs.connections)>);
    return evaluateIfFinished(replace<index>(lhs, rhs));
  }

  namespace detail
  {
    template<class Node, class Output, class... Outputs>
    auto connectAllNonRecursive(Node&& node, Output&& output, Outputs&& ... outputs)
    {
      if constexpr(sizeof...(Outputs) == 0)
      {
        return connectSecondaryNonRecursive(node, output);
      }
      else
      {
        return connectAllNonRecursive(connectSecondaryNonRecursive(node, output), std::forward<Outputs>(outputs)...);
      }
    }
  }

  template<class Node1, class Node2>
  auto connectPrimary_impl(Node1 lhs, Node2 rhs)
  {
    static_assert(canPrimaryConnect_v<Node1>);
    if constexpr(std::is_same_v<typename Node1::PrimaryConnection, OpenConnectionPlaceHolder>)
    {
      return makeNode(std::move(lhs.op), std::move(lhs.connections), std::move(rhs));
    }
    else
    {
      return makeNode(std::move(lhs.op), std::move(lhs.connections), connectPrimary_impl(lhs.connectPrimaryion, rhs));
    }
  }

  template<class Node2>
  auto connectPrimary(OpenConnectionPlaceHolder, Node2 rhs)
  {
    return rhs;
  }

  template<class Node1, class Node2>
  auto connectPrimary(Node1 lhs, Node2 rhs)
  {
    return evaluateIfFinished(connectPrimary_impl(lhs, rhs));
  }

  template<class Source, class Node1, class Node2>
  auto connectPrimary(Input<Source, Node1>&& lhs, Node2 rhs)
  {
    return makeInput(std::move(lhs.source), connectPrimary(lhs.node, rhs));
  }
}
