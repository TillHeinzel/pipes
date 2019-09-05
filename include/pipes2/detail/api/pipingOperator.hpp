#pragma once
#include <type_traits>

#include "pipes2/detail/api/helpers.hpp"

namespace tillh::pipes2
{
  template<class Lhs, class Rhs, std::enable_if_t<detail::sends<Lhs> && detail::receives<Rhs>, bool> = true>
  auto operator>>=(Lhs && lhs, Rhs && rhs)
  {
    auto result = connectPrimary(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
    if constexpr(detail::isCompleted<decltype(result)>)
    {
      result.source.run(result.node);
      return;
    }
    else
    {
      return result;
    }
  }

  template<class Lhs, class Rhs, std::enable_if_t<!detail::sends<Lhs> && detail::canSend<Lhs> && detail::receives<Rhs>, bool> = true>
  decltype(auto) operator>>=(Lhs && lhs, Rhs && rhs)
  {
    return operator>>=(detail::makeSource(std::forward<Lhs>(lhs)), std::forward<Rhs>(rhs));
  }

  template<class Lhs, class Rhs, std::enable_if_t<detail::sends<Lhs> && !detail::receives<Rhs> && detail::canReceive<Rhs>, bool> = true>
  decltype(auto) operator>>=(Lhs && lhs, Rhs && rhs)
  {
    return operator>>=(std::forward<Lhs>(lhs), detail::makeSink(std::forward<Rhs>(rhs)));
  }
}
