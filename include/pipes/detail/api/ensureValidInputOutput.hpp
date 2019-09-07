#pragma once

#include <type_traits>

#include "pipes/detail/core/connect.hpp"
#include "pipes/detail/core/evaluate.hpp"

#include "pipes/detail/core/traits.hpp"

#include "pipes/detail/pipes/makeSink.hpp"

namespace tillh::pipes
{
  template<class T, std::enable_if_t<detail::receives<T>, bool> = true>
  decltype(auto) ensureValidOutput(T && t)
  {
    return FWD(t);
  }

  template<class T, std::enable_if_t<!detail::receives<T>&& detail::canReceive<T>, bool> = true>
  auto ensureValidOutput(T && t)
  {
    return detail::makeSink(FWD(t));
  }

  template<class T>
  struct ensureValidOutputT
  {
    decltype(auto) operator() () { return ensureValidOutput(FWD(t)); }
    T t;
  };
  template<class T>
  ensureValidOutputT<T&&> ensureValidOutputF(T&& t)
  {
    return {FWD(t)};
  }
}

namespace tillh::pipes
{
  template<class T, std::enable_if_t<detail::sends<T>, bool> = true>
  decltype(auto) ensureValidInput(T && t)
  {
    return FWD(t);
  }

  template<class T, std::enable_if_t<!detail::sends<T>&& detail::canSend<T>, bool> = true>
  auto ensureValidInput(T && t)
  {
    return detail::makeSource(FWD(t));
  }

  template<class T>
  struct ensureValidInputT
  {
    decltype(auto) operator() () { return ensureValidInput(FWD(t)); }
    T t;
  };

  template<class T>
  ensureValidInputT<T&&> ensureValidInputF(T&& t)
  {
    return {FWD(t)};
  }
}
