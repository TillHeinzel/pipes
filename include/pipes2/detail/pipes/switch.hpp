#pragma once

namespace tillh::pipes2
{
  template<class Condition, class Output>
  struct Case
  {
    Condition condition;
    Output output;
  };

  struct DefaultCondition
  {
    template<class T>
    bool operator()(const T&) const { return true; }
  };

  template<class Condition, class Output>
  auto makeCase(Condition condition, Output output)
  {
    return Case<Condition, Output>{condition, output};
  }

  template<class... Conditions>
  class Switch
  {
    static_assert(is_unique_v<Conditions...>);
  public:

    Switch(std::tuple<Conditions...> conditions): conditions_(std::move(conditions)) {}

    template<class T, class... Outputs>
    void push(T&& t, Outputs& ... outputs) const
    {
      static_assert(sizeof...(Outputs) == sizeof...(Conditions));

      push_recurse(std::forward<T>(t), makeCase(std::get<Conditions>(conditions_), outputs)...);
    }

  private:
    std::tuple<Conditions...> conditions_;

    template<class T, class Case, class... Cases>
    void push_recurse(T&& t, const Case& case_, const Cases& ... cases_) const
    {
      if(case_.condition(t))
      {
        case_.output.push(std::forward<T>(t));
        return;
      }

      if constexpr(sizeof...(Cases) == 0)
      {
        return;
      }
      else
      {
        push_recurse(std::forward<T>(t), cases_...);
      }
    }
  };

  template<class... Conditions>
  auto makeSwitch(std::tuple<Conditions...> conditions)
  {
    return Switch<Conditions...>(std::move(conditions));
  }
}
