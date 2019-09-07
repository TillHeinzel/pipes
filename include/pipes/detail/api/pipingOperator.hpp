#pragma once
#include <type_traits>

#include "pipes/detail/core/connect.hpp"
#include "pipes/detail/core/traits.impl.hpp"

namespace tillh
{
  namespace pipes
  {
    template<class Lhs, class Rhs, std::enable_if_t<(sends<Lhs> || canSend<Lhs>) && (receives<Rhs> || canReceive<Rhs>), bool> = true>
    auto operator>>=(Lhs && lhs, Rhs && rhs)
    {
      return connectPrimary(FWD(lhs), FWD(rhs));
    }
  }
}
