#pragma once

#include <pipes2/detail/core/Node.hpp>
#include <pipes2/detail/core/Output.hpp>

struct DummyOp
{
  template<class T, class... Output>
  void push(T&&, Output& ... ) {}
};

template<bool hasPrimary, std::size_t nOutputs>
auto makeDummyNode()
{
  return tillh::pipes2::makeNode<hasPrimary, nOutputs>(DummyOp());
}

using DummyOutput = tillh::pipes2::Output<DummyOp, std::tuple<>>;
