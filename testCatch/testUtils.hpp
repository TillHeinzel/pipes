#pragma once

#include <vector>
#include <algorithm>

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
