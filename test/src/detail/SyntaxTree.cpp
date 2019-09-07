#include <gtest/gtest.h>

#include <pipes2/pipes.hpp>

#include "../testUtils.hpp"
#include "is_node.hpp"

namespace pipes = tillh::pipes2;

using Open = pipes::OpenConnectionPlaceHolder;
using Closed = DummyOutput;
auto makeOutput() { return Closed{DummyOp(), {}}; }

template<std::size_t index, class Node1, class Node2>
auto replace(Node1 node1, Node2 node2)
{
  return pipes::makeNode(node1.op, pipes::tuple_replace<index>(node1.connections, node2));
}

template<bool hasPrimary, std::size_t secondaries>
constexpr auto node()
{
  return pipes::makeNode<hasPrimary, secondaries>(DummyOp());
}

TEST(isNode, node)
{
  using NodeT = pipes::Node<DummyOp, std::tuple<>>;
  static_assert(is_node_v<NodeT>);
}

TEST(isNode, int)
{
  static_assert(!is_node_v<int>);
}

TEST(canSecondaryConnect, singleOpen)
{
  using NodeT = pipes::Node<DummyOp, std::tuple<Open>>;
  static_assert(pipes::canSecondaryConnect<NodeT>);
}

TEST(canSecondaryConnect, singleClosed)
{
  using NodeT = pipes::Node<DummyOp, std::tuple<Closed>>;
  static_assert(!pipes::canSecondaryConnect<NodeT>);
}

TEST(canSecondaryConnect, secondOpen)
{
  using NodeT = pipes::Node<DummyOp, std::tuple<Closed, Open>>;
  static_assert(pipes::canSecondaryConnect<NodeT>);
}

TEST(canSecondaryConnect, twoClosed)
{
  using NodeT = pipes::Node<DummyOp, std::tuple<Closed, Closed>>;
  static_assert(!pipes::canSecondaryConnect<NodeT>);
}

TEST(canSecondaryConnect, manyClosed)
{
  using NodeT = pipes::Node<DummyOp, std::tuple<Closed, Closed, Closed, Closed, Closed, Closed, Closed>>;
  static_assert(!pipes::canSecondaryConnect<NodeT>);
}

TEST(canSecondaryConnect, manyOneOpen)
{
  using NodeT = pipes::Node<DummyOp, std::tuple<Closed, Closed, Closed, Open, Closed, Closed, Closed>>;
  static_assert(pipes::canSecondaryConnect<NodeT>);
}

TEST(canSecondaryConnect, manySeveralOpen)
{
  using NodeT = pipes::Node<DummyOp, std::tuple<Closed, Closed, Closed, Open, Closed, Closed, Open, Closed>>;
  static_assert(pipes::canSecondaryConnect<NodeT>);
}

TEST(connectSecondaryImpl, singleNodeToOutput)
{
  auto node1 = node<false, 1>();
  using NodeT = decltype(node1);
  static_assert(pipes::canSecondaryConnect<NodeT>);
  static_assert(!pipes::canPrimaryConnect<NodeT>);

  auto node2 = makeOutput();
  static_assert(pipes::is_output_v<decltype(node2)>);

  auto result = pipes::connectImpl(pipes::secondary_constant(), node1, node2);
  static_assert(pipes::is_output_v<decltype(result)>);
}

TEST(connectSecondaryImpl, depthFirst)
{
  auto node1 = node<false, 2>();
  static_assert(pipes::canSecondaryConnect<decltype(node1)>);

  auto node2 = node<false, 1>();
  static_assert(pipes::canSecondaryConnect<decltype(node2)>);

  auto node3 = pipes::connectSecondaryImpl(node1, node2);
  static_assert(pipes::canSecondaryConnect<decltype(node3)>);

  auto output = makeOutput();
  auto result = pipes::connectImpl(pipes::secondary_constant(), node3, output);
  static_assert(pipes::canSecondaryConnect<decltype(result)>);
}

TEST(connectSecondaryImpl, openCount)
{
  auto node1 = node<false, 2>();
  static_assert(pipes::openCount<decltype(node1)> == 2);

  auto node2 = node<false, 1>();
  static_assert(pipes::openCount<decltype(node2)> == 1);

  auto node3 = replace<0>(node1, node2);
  static_assert(pipes::openCount<decltype(node3)> == 2);

  auto output = makeOutput();
  auto result = replace<0>(node1, replace<0>(node2, output));
  static_assert(pipes::openCount<decltype(result)> == 1);
}

TEST(connectSecondaryImpl, getIndices_x)
{
  static_assert(std::is_same_v<decltype(pipes::getIndices_x<0, 4, 1>()), std::index_sequence<0>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices_x<1, 4, 3>()), std::index_sequence<1, 2, 3>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices_x<0, 3, 1>()), std::index_sequence<0>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices_x<1, 3, 1>()), std::index_sequence<1>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices_x<2, 3, 1>()), std::index_sequence<2>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices_x<0, 1, 1>()), std::index_sequence<0>>);

  static_assert(std::is_same_v<decltype(pipes::getIndices_x<0, 3, 1>()), std::index_sequence<0>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices_x<1, 3, 3>()), std::index_sequence<1, 2>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices_x<0, 2, 1>()), std::index_sequence<0>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices_x<1, 2, 1>()), std::index_sequence<1>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices_x<2, 2, 1>()), std::index_sequence<>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices_x<3, 2, 1>()), std::index_sequence<>>);
}


TEST(connectSecondaryImpl, getIndices)
{
  auto node1 = node<false, 2>();
  auto node2 = node<false, 3>();
  auto node3 = node<false, 1>();

  auto combined = replace<1>(node1, replace<1>(node2, node3));

  static_assert(std::is_same_v<decltype(pipes::getIndices<4, decltype(combined)>()), std::tuple<std::index_sequence<0>, std::index_sequence<1, 2, 3>>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices<3, decltype(combined)>()), std::tuple<std::index_sequence<0>, std::index_sequence<1, 2>>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices<2, decltype(combined)>()), std::tuple<std::index_sequence<0>, std::index_sequence<1>>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices<1, decltype(combined)>()), std::tuple<std::index_sequence<0>, std::index_sequence<>>>);
  
  auto combined2 = replace<1>(node2, node3);

  static_assert(std::is_same_v<decltype(pipes::getIndices<3, decltype(combined2)>()), std::tuple<std::index_sequence<0>, std::index_sequence<1>, std::index_sequence<2>>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices<2, decltype(combined2)>()), std::tuple<std::index_sequence<0>, std::index_sequence<1>, std::index_sequence<>>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices<1, decltype(combined2)>()), std::tuple<std::index_sequence<0>, std::index_sequence<>, std::index_sequence<>>>);
}

TEST(connectSecondaryImpl, getIndicesWithPrimary)
{
  auto node1 = node<false, 2>();
  auto node2 = node<false, 3>();
  auto node3 = node<false, 1>();

  auto combined = replace<1>(node1, replace<1>(node2, node3));

  static_assert(std::is_same_v<decltype(pipes::getIndices<4, decltype(combined)>()), std::tuple<std::index_sequence<0>, std::index_sequence<1, 2, 3>>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices<3, decltype(combined)>()), std::tuple<std::index_sequence<0>, std::index_sequence<1, 2>>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices<2, decltype(combined)>()), std::tuple<std::index_sequence<0>, std::index_sequence<1>>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices<1, decltype(combined)>()), std::tuple<std::index_sequence<0>, std::index_sequence<>>>);

  auto combined2 = replace<1>(node2, node3);

  static_assert(std::is_same_v<decltype(pipes::getIndices<3, decltype(combined2)>()), std::tuple<std::index_sequence<0>, std::index_sequence<1>, std::index_sequence<2>>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices<2, decltype(combined2)>()), std::tuple<std::index_sequence<0>, std::index_sequence<1>, std::index_sequence<>>>);
  static_assert(std::is_same_v<decltype(pipes::getIndices<1, decltype(combined2)>()), std::tuple<std::index_sequence<0>, std::index_sequence<>, std::index_sequence<>>>);
}
TEST(connectNew, connectSecondaryImpl)
{
  const auto node1 = node<false, 1>();
  const auto node2 = node<false, 2>();
  const auto node3 = node<false, 3>();
  {
    auto result = pipes::connectSecondaryImpl(node1, node1);
    auto expected = replace<0>(node1, node1);
    static_assert(std::is_same_v<decltype(result), decltype(expected)>);
  }
  {
    auto result = pipes::connectSecondaryImpl(node2, node1, node2);
    auto expected = replace<1>(replace<0>(node2, node1), node2);
    static_assert(std::is_same_v<decltype(result), decltype(expected)>);
  }
  {
    auto result = pipes::connectSecondaryImpl(node3, node1, node2);
    auto expected = replace<1>(replace<0>(node3, node1), node2);
    static_assert(std::is_same_v<decltype(result), decltype(expected)>);
  }
}
TEST(connectNew, connect_secondary)
{
  const auto node1 = node<false, 1>();
  const auto node2 = node<false, 2>();
  const auto node3 = node<false, 3>();
  {
    auto result = pipes::connectImpl(pipes::secondary_constant(), node1, node1);
    auto expected = replace<0>(node1, node1);
    static_assert(std::is_same_v<decltype(result), decltype(expected)>);
  }
  {
    auto result = pipes::connectImpl(pipes::secondary_constant(), node2, node1, node2);
    auto expected = replace<1>(replace<0>(node2, node1), node2);
    static_assert(std::is_same_v<decltype(result), decltype(expected)>);
  }
  {
    auto result = pipes::connectImpl(pipes::secondary_constant(), node3, node1, node2);
    auto expected = replace<1>(replace<0>(node3, node1), node2);
    static_assert(std::is_same_v<decltype(result), decltype(expected)>);
  }
}


template<bool hasPrimary, std::size_t secondaries>
constexpr auto nodeT()
{
  return pipes::Type<decltype(node<hasPrimary, secondaries>())>();
}

template<bool hasPrimary, std::size_t secondaries, class T>
constexpr bool isNode(pipes::Type<T>)
{
  return pipes::canPrimaryConnect<pipes::remove_cv_ref_t<T>> == hasPrimary && pipes::openCount<pipes::remove_cv_ref_t<T>> == secondaries;
}

#define T(x) pipes::Type<decltype(x)>()

TEST(connectNew, counts)
{
  const auto f1 = node<false, 1>();
  const auto f2 = node<false, 2>();
  const auto f3 = node<false, 3>();

  const auto t1 = node<true, 1>();
  const auto t2 = node<true, 2>();
  const auto t3 = node<true, 3>();

  static_assert(isNode<false, 1>(T(f1)));
  static_assert(isNode<false, 2>(T(f2)));
  static_assert(isNode<false, 3>(T(f3)));

  static_assert(isNode<true, 1>(T(t1)));
  static_assert(isNode<true, 2>(T(t2)));
  static_assert(isNode<true, 3>(T(t3)));

  {
    const auto n = pipes::connectPrimary(node<true, 1>(), node<false, 1>());
    static_assert(isNode<false, 2>(T(n)));
  }
  {
    const auto n = pipes::connectPrimary(node<true, 2>(), node<false, 3>());
    static_assert(isNode<false, 5>(T(n)));
  }
  {
    const auto n = pipes::connectPrimary(node<true, 1>(), node<true, 1>());
    static_assert(isNode<true, 2>(T(n)));
  }
  {
    const auto n = pipes::connectPrimary(node<true, 2>(), node<true, 3>());
    static_assert(isNode<true, 5>(T(n)));
  }
   //secondary
  {
    const auto n = pipes::connectSecondary(node<true, 1>(), node<false, 1>());
    static_assert(isNode<true, 1>(T(n)));
  }
  {
    const auto n = pipes::connectSecondary(node<true, 1>(), node<true, 1>());
    static_assert(isNode<true, 2>(T(n)));
  }
}
