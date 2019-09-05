#include <gtest/gtest.h>

#include <pipes2/detail/core/connect.hpp>

#include "../Dummy.hpp"
#include "is_node.hpp"

using namespace tillh::pipes2;

using Open = OpenConnectionPlaceHolder;
using Closed = DummyOutput;
auto makeOutput() { return Closed{DummyOp(), {}}; }

TEST(isNode, node)
{
  using NodeT = Node<DummyOp, std::tuple<>, Open>;
  static_assert(is_node_v<NodeT>);
}

TEST(isNode, int)
{
  static_assert(!is_node_v<int>);
}

TEST(firstOpen, open)
{
  using NodeT = Node<DummyOp, std::tuple<>, NoPrimary>;
  static_assert(firstOpen(Type<NodeT>()) == 0);
}

TEST(canSecondaryConnect, singleOpen)
{
  using NodeT = Node<DummyOp, std::tuple<Open>, NoPrimary>;
  static_assert(canSecondaryConnect(Type<NodeT>()));
}

TEST(canSecondaryConnect, singleClosed)
{
  using NodeT = Node<DummyOp, std::tuple<Closed>, NoPrimary>;
  static_assert(!canSecondaryConnect(Type<NodeT>()));
}

TEST(canSecondaryConnect, secondOpen)
{
  using NodeT = Node<DummyOp, std::tuple<Closed, Open>, NoPrimary>;
  static_assert(canSecondaryConnect(Type<NodeT>()));
}

TEST(canSecondaryConnect, twoClosed)
{
  using NodeT = Node<DummyOp, std::tuple<Closed, Closed>, NoPrimary>;
  static_assert(!canSecondaryConnect(Type<NodeT>()));
}

TEST(canSecondaryConnect, manyClosed)
{
  using NodeT = Node<DummyOp, std::tuple<Closed, Closed, Closed, Closed, Closed, Closed, Closed>, NoPrimary>;
  static_assert(!canSecondaryConnect(Type<NodeT>()));
}

TEST(canSecondaryConnect, manyOneOpen)
{
  using NodeT = Node<DummyOp, std::tuple<Closed, Closed, Closed, Open, Closed, Closed, Closed>, NoPrimary>;
  static_assert(canSecondaryConnect(Type<NodeT>()));
}

TEST(canSecondaryConnect, manySeveralOpen)
{
  using NodeT = Node<DummyOp, std::tuple<Closed, Closed, Closed, Open, Closed, Closed, Open, Closed>, NoPrimary>;
  static_assert(canSecondaryConnect(Type<NodeT>()));
}

TEST(connectSecondary, singleNodeToOutput)
{
  auto node1 = makeNode<false, 1>(DummyOp());
  static_assert(canSecondaryConnect(Type<decltype(node1)>()));
  static_assert(!canPrimaryConnect(Type<decltype(node1)>()));

  auto node2 = makeOutput();
  static_assert(is_output_v<decltype(node2)>);

  auto result = connectSecondary(node1, node2);
  static_assert(is_output_v<decltype(result)>);
}

TEST(connectSecondary, depthFirst)
{
  auto node1 = makeNode<false, 2>(DummyOp());
  static_assert(canSecondaryConnect(Type<decltype(node1)>()));
  static_assert(firstOpen(Type<decltype(node1)>()) == 0);

  auto node2 = makeNode<false, 1>(DummyOp());
  static_assert(canSecondaryConnect(Type<decltype(node2)>()));
  static_assert(firstOpen(Type<decltype(node2)>()) == 0);

  auto node3 = connectSecondary(node1, node2);
  static_assert(canSecondaryConnect(Type<decltype(node3)>()));
  static_assert(firstOpen(Type<decltype(node3)>()) == 0);

  auto output = makeOutput();
  auto result = connectSecondary(node3, output);
  static_assert(canSecondaryConnect(Type<decltype(result)>()));
  static_assert(firstOpen(Type<decltype(result)>()) == 1);
}

TEST(connectSecondaryNonRecursive, singleNodeToOutput)
{
  auto node1 = makeNode<false, 1>(DummyOp());
  static_assert(canSecondaryConnect(Type<decltype(node1)>()));

  auto node2 = makeOutput();
  static_assert(is_output_v<decltype(node2)>);

  auto result = connectSecondaryNonRecursive(node1, node2);
  static_assert(is_output_v<decltype(result)>);
}

TEST(connectSecondary, breadthFirst)
{
  auto node1 = makeNode<false, 2>(DummyOp());
  static_assert(canSecondaryConnect(Type<decltype(node1)>()));
  static_assert(firstOpen(Type<decltype(node1)>()) == 0);

  auto node2 = makeNode<false, 1>(DummyOp());
  static_assert(canSecondaryConnect(Type<decltype(node2)>()));
  static_assert(firstOpen(Type<decltype(node2)>()) == 0);

  auto node3 = connectSecondaryNonRecursive(node1, node2);
  static_assert(canSecondaryConnect(Type<decltype(node3)>()));
  static_assert(firstOpen(Type<decltype(node3)>()) == 0);

  auto output = makeOutput();
  auto result = connectSecondaryNonRecursive(node3, output);
  static_assert(canSecondaryConnect(Type<decltype(result)>()));
  static_assert(firstOpen(Type<decltype(result)>()) == 0);
}
