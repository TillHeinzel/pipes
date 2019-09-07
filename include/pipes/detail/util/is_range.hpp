#pragma once

#include <type_traits>

#include "pipes/detail/util/metaprogramming.hpp"

namespace tillh
{
  namespace pipes
  {
    namespace util
    {
      template<typename T>
      using begin_expression = decltype(std::begin(std::declval<T>()));

      template<typename T>
      using end_expression = decltype(std::end(std::declval<T>()));

      template<class Range, class SFINAE = void>
      struct is_range : std::false_type {};

      template<typename Range>
      struct is_range<Range, std::enable_if_t<is_detected_v<begin_expression, Range>&& is_detected_v<end_expression, Range>>> : std::true_type {};

      template<class Range>
      constexpr bool is_range_v = is_range<Range>::value;
    }
  }
}