#pragma once
#include <type_traits>

#include "pipes/detail/core/connect.hpp"

namespace tillh::pipes
{
  template<class Lhs, class Rhs, std::enable_if_t<(detail::sends<Lhs> || detail::canSend<Lhs>) && (detail::receives<Rhs> || detail::canReceive<Rhs>), bool> = true>
  auto operator>>=(Lhs && lhs, Rhs && rhs)
  {
    return connectPrimary(FWD(lhs), FWD(rhs));
  }
}
