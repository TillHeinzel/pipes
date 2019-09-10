#pragma once

#include "pipes/detail/util/is_dereferencable.hpp"
#include "pipes/detail/util/is_incrementable.hpp"

namespace tillh
{
  namespace pipes
  {
    namespace util
    {
      template<class T>
      constexpr static bool is_output_iterator =
        std::is_copy_constructible_v<T> &&
        std::is_copy_assignable_v<T> &&
        std::is_destructible_v<T> &&
        std::is_swappable_v<T> &&

        is_dereferencable<T> &&
        is_iterator_incrementable<T>;
    }
  }
}
