#pragma once

#include <type_traits>

#include "pipes2/detail/core/connect.hpp"
#include "pipes2/detail/core/evaluate.hpp"

#include "pipes2/detail/core/traits.hpp"

#include "pipes2/detail/pipes/makeSink.hpp"

namespace tillh::pipes2::detail
{
  template<class T, std::enable_if_t<receives<T>, bool> = true>
  auto ensureValidOutput(T t)
  {
    return t;
  }

  template<class T, std::enable_if_t<!receives<T>&& canReceive<T>, bool> = true>
  auto ensureValidOutput(T && t)
  {
    return makeSink(std::forward<T>(t));
  }
}
