#pragma once

namespace tillh::pipes2
{
  template<class Range>
  class CopySource
  {
  public:
    constexpr CopySource(const Range& range) : range_(range) {}

    template<class Output>
    void run(Output& output) const
    {
      for(const auto& obj : range_) output.push(obj);
    }

  private:
    const Range& range_;
  };
}
