#pragma once

#include "pipes/detail/util/metaprogramming.hpp"

namespace tillh
{
  namespace pipes
  {
    namespace util
    {
      template<class T>
      using pre_increment_expression = decltype(++(std::declval<T>()));

      template<class T>
      constexpr static bool is_iterator_incrementable = is_detected<pre_increment_expression, T>;
    }
  }
}