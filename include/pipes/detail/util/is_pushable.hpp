#pragma once

#include <functional>

#include "pipes/detail/util/metaprogramming.hpp"

namespace tillh
{
  namespace pipes
  {
    namespace util
    {
      template<class T>
      using push_back_expression = decltype(std::declval<T>().push_back(std::declval<typename T::value_type>()));
      
      template<class Pushable>
      constexpr bool is_pushable = is_detected<push_back_expression, Pushable>;
    }
  }
}
