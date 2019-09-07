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

namespace testUtils
{
  struct Transform
  {
    int operator()(int i) const { return i + 1; }
    std::vector<int> each(std::vector<int> src)
    {
      auto ret = std::vector<int>();
      std::transform(src.begin(), src.end(), std::back_inserter(ret), *this);
      return ret;
    }
  };
  struct Filter
  {
    bool operator()(int i) const { return (i % 2) != 0; }
    std::vector<int> each(const std::vector<int>& src)
    {
      auto ret = std::vector<int>();
      std::copy_if(src.begin(), src.end(), std::back_inserter(ret), *this);
      return ret;
    }
    std::vector<int> eachNot(const std::vector<int>& src)
    {
      auto ret = std::vector<int>();
      const auto notThis = [this](auto i) {return !(*this)(i); };
      std::copy_if(src.begin(), src.end(), std::back_inserter(ret), notThis);
      return ret;
    }
  };
}