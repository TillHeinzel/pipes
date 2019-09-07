#pragma once

#include "pipes/detail/util/metaprogramming.hpp"

namespace tillh
{
  namespace pipes
  {
    namespace util
    {
      template<class T>
      using deref_expression = decltype(*std::declval<T>());

      template<class T>
      struct is_dereferencable : is_detected<deref_expression, T> {};

      template<class T>
      constexpr static bool is_dereferencable_v = is_dereferencable<T>::value;
    }
  }
}