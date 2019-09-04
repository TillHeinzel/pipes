#pragma once

namespace tillh::pipes2
{
  template<class F>
  class Transform
  {
  public:
    Transform(F f): f_(f) {}

    template<class T, class Output>
    void push(T&& t, Output& output) const
    {
      //static_assert(is_output_v<Output>);
      //static_assert(std::is_same_v<std::invoke_result_t<F, decltype(std::forward<T>(t))>, void>);
      output.push(f_(std::forward<T>(t)));
    }

  private:
    const F f_;
  };

  template<class F>
  class Filter
  {
  public:
    Filter(F f): f_(f) {}

    template<class T, class Output>
    void push(T&& t, Output& output) const
    {
      static_assert(std::is_same_v<std::invoke_result_t<F, decltype(std::forward<T>(t))>, bool>);
      if(f_(t)) output.push(std::forward<T>(t));
    }

  private:
    const F f_;
  };

  class Demux
  {
  public:
    template<class T, class... Outputs>
    void push(const T& t, Outputs& ... outputs) const
    {
      (outputs.push(t), ...);
    }
  };

  class Join
  {
  public:
    template<class T, class Output>
    void push(T&& t, Output& output) const
    {
      for(auto&& e : t)
      {
        output.push(std::forward<decltype(e)>(e));
      }
    }
  };

  class Unzip
  {
  public:
    template<class T, class... Outputs>
    void push(const T& t, Outputs& ... outputs) const
    {
      static_assert(sizeof...(outputs) == std::tuple_size_v<T>);

      push_impl(t, std::make_index_sequence<sizeof...(outputs)>(), outputs...);
    }

  private:
    template<class T, class... Outputs, std::size_t... Is>
    void push_impl(const T& t, std::index_sequence<Is...>, Outputs& ... outputs) const
    {
      (outputs.push(std::get<Is>(t)), ...);
    }
  };
}
