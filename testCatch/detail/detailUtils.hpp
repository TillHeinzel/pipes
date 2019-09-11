#pragma once

#include <pipes/detail/core/Node.hpp>
#include <pipes/detail/core/Output.hpp>

struct DummyOp
{
  template<class T, class... Output>
  void push(T&&, Output& ...) {}
};

template<bool hasPrimary, std::size_t nOutputs>
auto makeDummyNode()
{
  return tillh::pipes::makeNode<hasPrimary, nOutputs>(DummyOp());
}

using DummyOutput = tillh::pipes::Output<DummyOp, std::tuple<>>;

template<class T>
struct is_output : std::false_type {};

template<class Op, class Outputs>
struct is_output<tillh::pipes::Output<Op, Outputs>> : std::true_type {};

template<class T>
constexpr static bool is_output_v = is_output<T>::value;

template<class T>
struct is_node : std::false_type {};

template<class Op, class Connections>
struct is_node<tillh::pipes::Node<Op, Connections>> : std::true_type {};

template<class T>
constexpr auto is_node_v = is_node<T>::value;

template<class Op, class Outputs>
struct is_node<tillh::pipes::Output<Op, Outputs>> : std::true_type {};
