#pragma once

namespace tillh::pipes2
{
  template<class Iterator>
  class IteratorSink
  {
  public:
    IteratorSink(Iterator iterator): iterator_(iterator) {}

    template<class T>
    void push(T&& t) const
    {
      auto& iterator = const_cast<Iterator&>(iterator_);
      *iterator = std::forward<T>(t);
      ++iterator;
    }

  private:
    Iterator iterator_;
  };

  class Discard
  {
  public:
    template<class T>
    void push(T&&) const {}
  };

  template<class F>
  class Custom
  {
  public:
    explicit Custom(F f): f_(f) {}

    template<class T>
    void push(T&& t) const
    {
      f_(std::forward<T>(t));
    }

  private:
    CopyableCallable<F> f_;
  };

  template<class Map, class F>
  class MapAggregator
  {
  public:
    MapAggregator(Map& map, F f): map_(map), f_(f) {}

    template<class T>
    void push(T&& t) const
    {
      // assumes T is a pair. todo: make static assert for that

      auto& map = const_cast<Map&>(map_.get());

      const auto it = map.find(t.first);
      if(it == map.end())
      {
        map.emplace(std::forward<T>(t));
      }
      else
      {
        // todo: add if constexpr for non-copyable types
        it->second = f_(it->second, std::forward<T>(t).second);
      }
    }

  private:
    // use reference_wrapper to allow copy assignment, so Output<MapAggregator> can fulfill outputIterator
    std::reference_wrapper<Map> map_;
    CopyableCallable<F> f_;
  };

  template<class Stream>
  class ToStream
  {
  public:
    ToStream(Stream& stream): stream_(stream) {}

    template<class T>
    void push(T&& t) const
    {
      const_cast<Stream&>(stream_.get()) << std::forward<T>(t);
    }

  private:
    std::reference_wrapper<Stream> stream_;
  };
}
