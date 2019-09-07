#pragma once

#include "pipes/detail/util/metaprogramming.hpp"

namespace tillh
{
  namespace pipes
  {
    namespace util
    {
      template<class T>
      using pre_increment_expression = decltype(++std::declval<T>());

      template<class T, class Sfinae = void>
      struct is_iterator_incrementable : std::false_type {};

      template<class T>
      struct is_iterator_incrementable<T, std::enable_if_t<std::is_same_v<T&, pre_increment_expression<T>>>> : std::true_type {};

      template<class T>
      constexpr static bool is_iterator_incrementable_v = is_iterator_incrementable<T>::value;
    }
  }
}