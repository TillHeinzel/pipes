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

  template<class Range>
  class MoveSource
  {
  public:
    constexpr MoveSource(Range& range): range_(range) {}

    template<class Output>
    void run(Output& output) const
    {
      for(auto& obj : range_) output.push(std::move(obj));
    }

  private:
    Range& range_;
  };

  template<class Stream, class ExtractAs>
  class StreamSource
  {
  public:
    StreamSource(Stream& stream):streamPtr(nullptr), stream_(stream) {}
    StreamSource(Stream&& stream):streamPtr(std::make_unique<Stream>(std::move(stream))), stream_(*streamPtr) {}

    template<class Output>
    void run(Output& output) const
    {
      for(auto it = std::istream_iterator<ExtractAs>{stream_.get()}; it != std::istream_iterator<ExtractAs>{}; ++it)
      {
        output.push(*it);
      }
    }

  private:
    std::unique_ptr<Stream> streamPtr;
    std::reference_wrapper<Stream> stream_;

  };
}
