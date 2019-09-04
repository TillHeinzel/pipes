#pragma once

#include <functional>

#include "pipes2/detail/util/metaprogramming.hpp"

namespace tillh::pipes2
{
  template<class T>
  using push_back_expression = decltype(std::declval<T>().push_back(std::declval<typename T::value_type>()));

  template<class Pushable, class Sfinae = void>
  struct is_pushable : std::false_type {};

  template<class Pushable>
  struct is_pushable<Pushable, std::enable_if_t<detail::is_detected_v<push_back_expression, Pushable>>> : std::true_type {};

  template<class Pushable>
  constexpr bool is_pushable_v = is_pushable<Pushable>::value;
}
