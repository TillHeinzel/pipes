#include <gtest/gtest.h>

#include <pipes2/detail/core/connect.hpp>
#include <pipes2/detail/core/traits.impl.hpp>

#include "../Dummy.hpp"
#include "is_node.hpp"

using namespace tillh::pipes2;

using Open = OpenConnectionPlaceHolder;
using Closed = DummyOutput;
auto makeOutput() { return Closed{DummyOp(), {}}; }

template<std::size_t index, class Node1, class Node2>
auto replace(Node1 node1, Node2 node2)
{
  return makeNode(node1.op, tuple_replace<index>(node1.connections, node2), node1.primaryConnection);
}

TEST(isNode, node)
{
  using NodeT = Node<DummyOp, std::tuple<>, Open>;
  static_assert(is_node_v<NodeT>);
}

TEST(isNode, int)
{
  static_assert(!is_node_v<int>);
}

TEST(canSecondaryConnect, singleOpen)
{
  using NodeT = Node<DummyOp, std::tuple<Open>, NoPrimary>;
  static_assert(canSecondaryConnect<NodeT>);
}

TEST(canSecondaryConnect, singleClosed)
{
  using NodeT = Node<DummyOp, std::tuple<Closed>, NoPrimary>;
  static_assert(!canSecondaryConnect<NodeT>);
}

TEST(canSecondaryConnect, secondOpen)
{
  using NodeT = Node<DummyOp, std::tuple<Closed, Open>, NoPrimary>;
  static_assert(canSecondaryConnect<NodeT>);
}

TEST(canSecondaryConnect, twoClosed)
{
  using NodeT = Node<DummyOp, std::tuple<Closed, Closed>, NoPrimary>;
  static_assert(!canSecondaryConnect<NodeT>);
}

TEST(canSecondaryConnect, manyClosed)
{
  using NodeT = Node<DummyOp, std::tuple<Closed, Closed, Closed, Closed, Closed, Closed, Closed>, NoPrimary>;
  static_assert(!canSecondaryConnect<NodeT>);
}

TEST(canSecondaryConnect, manyOneOpen)
{
  using NodeT = Node<DummyOp, std::tuple<Closed, Closed, Closed, Open, Closed, Closed, Closed>, NoPrimary>;
  static_assert(canSecondaryConnect<NodeT>);
}

TEST(canSecondaryConnect, manySeveralOpen)
{
  using NodeT = Node<DummyOp, std::tuple<Closed, Closed, Closed, Open, Closed, Closed, Open, Closed>, NoPrimary>;
  static_assert(canSecondaryConnect<NodeT>);
}

TEST(connectSecondaryImpl, singleNodeToOutput)
{
  auto node1 = makeNode<false, 1>(DummyOp());
  using NodeT = decltype(node1);
  static_assert(canSecondaryConnect<NodeT>);
  static_assert(!canPrimaryConnect<NodeT>);

  auto node2 = makeOutput();
  static_assert(is_output_v<decltype(node2)>);

  auto result = connectImpl(secondary_constant(), node1, node2);
  static_assert(is_output_v<decltype(result)>);
}

TEST(connectSecondaryImpl, depthFirst)
{
  auto node1 = makeNode<false, 2>(DummyOp());
  static_assert(canSecondaryConnect<decltype(node1)>);

  auto node2 = makeNode<false, 1>(DummyOp());
  static_assert(canSecondaryConnect<decltype(node2)>);

  auto node3 = connectSecondaryImpl(node1, node2);
  static_assert(canSecondaryConnect<decltype(node3)>);

  auto output = makeOutput();
  auto result = connectImpl(secondary_constant(), node3, output);
  static_assert(canSecondaryConnect<decltype(result)>);
}

TEST(connectSecondaryImpl, openCount)
{
  auto node1 = makeNode<false, 2>(DummyOp());
  static_assert(openCount<decltype(node1)> == 2);

  auto node2 = makeNode<false, 1>(DummyOp());
  static_assert(openCount<decltype(node2)> == 1);

  auto node3 = replace<0>(node1, node2);
  static_assert(openCount<decltype(node3)> == 2);

  auto output = makeOutput();
  auto result = replace<0>(node1, replace<0>(node2, output));
  static_assert(openCount<decltype(result)> == 1);
}

TEST(connectSecondaryImpl, getIndices_x)
{
  static_assert(std::is_same_v<decltype(getIndices_x<0, 4, 1>()), std::index_sequence<0>>);
  static_assert(std::is_same_v<decltype(getIndices_x<1, 4, 3>()), std::index_sequence<1, 2, 3>>);
  static_assert(std::is_same_v<decltype(getIndices_x<0, 3, 1>()), std::index_sequence<0>>);
  static_assert(std::is_same_v<decltype(getIndices_x<1, 3, 1>()), std::index_sequence<1>>);
  static_assert(std::is_same_v<decltype(getIndices_x<2, 3, 1>()), std::index_sequence<2>>);
  static_assert(std::is_same_v<decltype(getIndices_x<0, 1, 1>()), std::index_sequence<0>>);

  static_assert(std::is_same_v<decltype(getIndices_x<0, 3, 1>()), std::index_sequence<0>>);
  static_assert(std::is_same_v<decltype(getIndices_x<1, 3, 3>()), std::index_sequence<1, 2>>);
  static_assert(std::is_same_v<decltype(getIndices_x<0, 2, 1>()), std::index_sequence<0>>);
  static_assert(std::is_same_v<decltype(getIndices_x<1, 2, 1>()), std::index_sequence<1>>);
  static_assert(std::is_same_v<decltype(getIndices_x<2, 2, 1>()), std::index_sequence<>>);
  static_assert(std::is_same_v<decltype(getIndices_x<3, 2, 1>()), std::index_sequence<>>);
}


TEST(connectSecondaryImpl, getIndices)
{
  auto node1 = makeNode<false, 2>(DummyOp());
  auto node2 = makeNode<false, 3>(DummyOp());
  auto node3 = makeNode<false, 1>(DummyOp());

  auto combined = replace<1>(node1, replace<1>(node2, node3));

  static_assert(std::is_same_v<decltype(getIndices<4, decltype(combined)>()), std::tuple<std::index_sequence<0>, std::index_sequence<1, 2, 3>>>);
  static_assert(std::is_same_v<decltype(getIndices<3, decltype(combined)>()), std::tuple<std::index_sequence<0>, std::index_sequence<1, 2>>>);
  static_assert(std::is_same_v<decltype(getIndices<2, decltype(combined)>()), std::tuple<std::index_sequence<0>, std::index_sequence<1>>>);
  static_assert(std::is_same_v<decltype(getIndices<1, decltype(combined)>()), std::tuple<std::index_sequence<0>, std::index_sequence<>>>);
  
  auto combined2 = replace<1>(node2, node3);

  static_assert(std::is_same_v<decltype(getIndices<3, decltype(combined2)>()), std::tuple<std::index_sequence<0>, std::index_sequence<1>, std::index_sequence<2>>>);
  static_assert(std::is_same_v<decltype(getIndices<2, decltype(combined2)>()), std::tuple<std::index_sequence<0>, std::index_sequence<1>, std::index_sequence<>>>);
  static_assert(std::is_same_v<decltype(getIndices<1, decltype(combined2)>()), std::tuple<std::index_sequence<0>, std::index_sequence<>, std::index_sequence<>>>);
}

TEST(connectNew, connectSecondaryImpl)
{
  const auto node1 = makeNode<false, 1>(DummyOp());
  const auto node2 = makeNode<false, 2>(DummyOp());
  const auto node3 = makeNode<false, 3>(DummyOp());
  {
    auto result = connectSecondaryImpl(node1, node1);
    auto expected = replace<0>(node1, node1);
    static_assert(std::is_same_v<decltype(result), decltype(expected)>);
  }
  {
    auto result = connectSecondaryImpl(node2, node1, node2);
    auto expected = replace<1>(replace<0>(node2, node1), node2);
    static_assert(std::is_same_v<decltype(result), decltype(expected)>);
  }
  {
    auto result = connectSecondaryImpl(node3, node1, node2);
    auto expected = replace<1>(replace<0>(node3, node1), node2);
    static_assert(std::is_same_v<decltype(result), decltype(expected)>);
  }
}
TEST(connectNew, connect_secondary)
{
  const auto node1 = makeNode<false, 1>(DummyOp());
  const auto node2 = makeNode<false, 2>(DummyOp());
  const auto node3 = makeNode<false, 3>(DummyOp());
  {
    auto result = connectImpl(secondary_constant(), node1, node1);
    auto expected = replace<0>(node1, node1);
    static_assert(std::is_same_v<decltype(result), decltype(expected)>);
  }
  {
    auto result = connectImpl(secondary_constant(), node2, node1, node2);
    auto expected = replace<1>(replace<0>(node2, node1), node2);
    static_assert(std::is_same_v<decltype(result), decltype(expected)>);
  }
  {
    auto result = connectImpl(secondary_constant(), node3, node1, node2);
    auto expected = replace<1>(replace<0>(node3, node1), node2);
    static_assert(std::is_same_v<decltype(result), decltype(expected)>);
  }
}
