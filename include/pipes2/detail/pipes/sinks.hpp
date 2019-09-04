#pragma once

namespace tillh::pipes2
{
  template<class Pushable>
  class PushBack
  {
  public:
    explicit PushBack(Pushable& pushable)
      : pushable_(pushable)
    {}

    template<class T>
    void push(T&& t) const
    {
      const_cast<Pushable&>(pushable_).push_back(std::forward<T>(t));
    }

  private:
    Pushable& pushable_;
  };
  
  class Discard
  {
  public:
    template<class T>
    void push(T&&) const {}
  };

  template<class Collection>
  class Override
  {
  public:
    explicit Override(Collection& collection)
      : collection_(collection)
    {}

    template<class T>
    void push(T&& t) const
    {
      if(iterator_ == collection_.end()) throw std::out_of_range("collection to be overridden has reached its end");
      *iterator_ = std::forward<T>(t);
      ++iterator_;
    }

  private:
    Collection& collection_;
    mutable typename Collection::iterator iterator_ = collection_.begin();
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
    F f_;
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

      auto& map = const_cast<Map&>(map_);

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
    Map& map_;
    F f_;
  };
}
