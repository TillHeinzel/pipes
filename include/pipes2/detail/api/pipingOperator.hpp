#pragma once
#include <type_traits>

#include "pipes2/detail/core/connect.hpp"

#include "pipes2/detail/api/ensureValidOutput.hpp"

namespace tillh::pipes2
{
  template<class Lhs, class Rhs, std::enable_if_t<(detail::sends<Lhs> || detail::canSend<Lhs>) && (detail::receives<Rhs> || detail::canReceive<Rhs>), bool> = true>
  auto operator>>=(Lhs && lhs, Rhs && rhs)
  {
    return connectPrimary(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
  }
}
