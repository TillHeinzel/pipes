#pragma once

#include "pipes/detail/util/metaprogramming.hpp"

namespace tillh::pipes
{
  template<class T>
  using deref_expression = decltype(*std::declval<T>());

  template<class T>
  struct is_dereferencable : detail::is_detected<deref_expression, T> {};

  template<class T>
  constexpr static bool is_dereferencable_v = is_dereferencable<T>::value;
}
