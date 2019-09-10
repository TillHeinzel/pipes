#pragma once

#include "pipes/detail/util/metaprogramming.hpp"

namespace tillh
{
  namespace pipes
  {
    namespace util
    {
      template<class T>
      using deref_expression = decltype(*(std::declval<T>()));

      template<class T>
      constexpr static bool is_dereferencable = is_detected<deref_expression, T>;
    }
  }
}
