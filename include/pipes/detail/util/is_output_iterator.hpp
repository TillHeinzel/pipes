#pragma once

#include "pipes/detail/util/is_dereferencable.hpp"
#include "pipes/detail/util/is_incrementable.hpp"

namespace tillh
{
  namespace pipes
  {
    namespace util
    {
      template<class T, class Sfinae = void>
      struct is_output_iterator : std::false_type {};

      template<class T>
      struct is_output_iterator<T, std::enable_if_t<
        std::is_copy_constructible_v<T>&&
        std::is_copy_assignable_v<T>&&
        std::is_destructible_v<T>&&
        std::is_swappable_v<T>&&

        is_dereferencable_v<T>&&
        is_iterator_incrementable_v<T>
        >> : std::true_type{};

      template<class T>
      constexpr static bool is_output_iterator_v = is_output_iterator<T>::value;
    }
  }
}