#pragma once

#include <type_traits>

#include "pipes/detail/util/metaprogramming.hpp"

namespace tillh
{
  namespace pipes
  {
    namespace util
    {
      namespace adl
      {
        using std::begin;
        using std::end;

        template<typename T>
        using begin_expression = decltype(begin(std::declval<T>()));

        template<typename T>
        using end_expression = decltype(end(std::declval<T>()));
      }

      template<class Range>
      constexpr bool is_range = is_detected<adl::begin_expression, Range> && is_detected<adl::end_expression, Range>;
    }
  }
}