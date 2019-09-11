#include "../catch.hpp"

#include <pipes/pipes.hpp>

#include "detailUtils.hpp"

using namespace tillh::pipes::api;
using namespace tillh::pipes;

using Open = OpenConnectionPlaceHolder;
using Closed = DummyOutput;
auto makeOutput() { return Closed{DummyOp(), {}}; }

template<std::size_t index, class Node1, class Node2>
auto replace(Node1 node1, Node2 node2)
{
  return makeNode(node1.op, util::tuple_replace<index>(node1.connections, node2));
}

template<bool hasPrimary, std::size_t secondaries>
constexpr auto node()
{
  return makeNode<hasPrimary, secondaries>(DummyOp());
}

TEST_CASE("isNode")
{
  SECTION("node")
  {
    using NodeT = Node<DummyOp, std::tuple<>>;
    static_assert(is_node_v<NodeT>);
  }

  SECTION("int fails")
  {
    static_assert(!is_node_v<int>);
  }
}

TEST_CASE("canSecondaryConnect trait")
{
  SECTION("Single open connection")
  {
    using NodeT = Node<DummyOp, std::tuple<Open>>;
    static_assert(canSecondaryConnect<NodeT>);
  }

  SECTION("single closed connection fails")
  {
    using NodeT = Node<DummyOp, std::tuple<Closed>>;
    static_assert(!canSecondaryConnect<NodeT>);
  }

  SECTION("secondOpen")
  {
    using NodeT = Node<DummyOp, std::tuple<Closed, Open>>;
    static_assert(canSecondaryConnect<NodeT>);
  }

  SECTION("Two closed connections fail")
  {
    using NodeT = Node<DummyOp, std::tuple<Closed, Closed>>;
    static_assert(!canSecondaryConnect<NodeT>);
  }

  SECTION("Many closed connections fails")
  {
    using NodeT = Node<DummyOp, std::tuple<Closed, Closed, Closed, Closed, Closed, Closed, Closed>>;
    static_assert(!canSecondaryConnect<NodeT>);
  }

  SECTION("Many connections with a single open one")
  {
    using NodeT = Node<DummyOp, std::tuple<Closed, Closed, Closed, Open, Closed, Closed, Closed>>;
    static_assert(canSecondaryConnect<NodeT>);
  }

  SECTION("Many connections with several open ones")
  {
    using NodeT = Node<DummyOp, std::tuple<Closed, Closed, Closed, Open, Closed, Closed, Open, Closed>>;
    static_assert(canSecondaryConnect<NodeT>);
  }
}

TEST_CASE("connectSecondaryImpl")
{
  SECTION("singleNodeToOutput")
  {
    auto node1 = node<false, 1>();
    using NodeT = decltype(node1);
    static_assert(canSecondaryConnect<NodeT>);
    static_assert(!canPrimaryConnect<NodeT>);

    auto node2 = makeOutput();
    static_assert(is_output_v<decltype(node2)>);

    auto result = connectImpl(secondary_constant(), node1, node2);
    static_assert(is_output_v<decltype(result)>);
  }

  SECTION("depthFirst")
  {
    auto node1 = node<false, 2>();
    static_assert(canSecondaryConnect<decltype(node1)>);

    auto node2 = node<false, 1>();
    static_assert(canSecondaryConnect<decltype(node2)>);

    auto node3 = connectSecondaryImpl(node1, node2);
    static_assert(canSecondaryConnect<decltype(node3)>);

    auto output = makeOutput();
    auto result = connectImpl(secondary_constant(), node3, output);
    static_assert(canSecondaryConnect<decltype(result)>);
  }

  SECTION("openCount")
  {
    auto node1 = node<false, 2>();
    static_assert(openCount<decltype(node1)> == 2);

    auto node2 = node<false, 1>();
    static_assert(openCount<decltype(node2)> == 1);

    auto node3 = replace<0>(node1, node2);
    static_assert(openCount<decltype(node3)> == 2);

    auto output = makeOutput();
    auto result = replace<0>(node1, replace<0>(node2, output));
    static_assert(openCount<decltype(result)> == 1);
  }

  SECTION("getIndices_x")
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


  SECTION("getIndices")
  {
    auto node1 = node<false, 2>();
    auto node2 = node<false, 3>();
    auto node3 = node<false, 1>();

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

  SECTION("getIndicesWithPrimary")
  {
    auto node1 = node<false, 2>();
    auto node2 = node<false, 3>();
    auto node3 = node<false, 1>();

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
}


template<bool hasPrimary, std::size_t secondaries>
constexpr auto nodeT()
{
  return util::Type<decltype(node<hasPrimary, secondaries>())>();
}

template<bool hasPrimary, std::size_t secondaries, class T>
constexpr bool isNode(util::Type<T>)
{
  return canPrimaryConnect<util::remove_cv_ref_t<T>> == hasPrimary && openCount<util::remove_cv_ref_t<T>> == secondaries;
}

#define T(x) util::Type<decltype(x)>()

TEST_CASE("connect")
{
  SECTION("connectSecondaryImpl")
  {
    const auto node1 = node<false, 1>();
    const auto node2 = node<false, 2>();
    const auto node3 = node<false, 3>();
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
  SECTION("connect_secondary")
  {
    const auto node1 = node<false, 1>();
    const auto node2 = node<false, 2>();
    const auto node3 = node<false, 3>();
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

  SECTION("counts")
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
      const auto n = connectPrimary(node<true, 1>(), node<false, 1>());
      static_assert(isNode<false, 2>(T(n)));
    }
    {
      const auto n = connectPrimary(node<true, 2>(), node<false, 3>());
      static_assert(isNode<false, 5>(T(n)));
    }
    {
      const auto n = connectPrimary(node<true, 1>(), node<true, 1>());
      static_assert(isNode<true, 2>(T(n)));
    }
    {
      const auto n = connectPrimary(node<true, 2>(), node<true, 3>());
      static_assert(isNode<true, 5>(T(n)));
    }
    //secondary
    {
      const auto n = connectSecondary(node<true, 1>(), node<false, 1>());
      static_assert(isNode<true, 1>(T(n)));
    }
    {
      const auto n = connectSecondary(node<true, 1>(), node<true, 1>());
      static_assert(isNode<true, 2>(T(n)));
    }
  }
}
