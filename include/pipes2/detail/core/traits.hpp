#pragma once

#include <type_traits>

#include "pipes2/detail/util/metaprogramming.hpp"

namespace tillh::pipes2::detail
{
  template<class T>
  struct isSender : std::false_type {};

  template<class T>
  constexpr bool sends = isSender<remove_cv_ref_t<T>>::value;

  template<class T>
  struct isReceiver : std::false_type {};

  template<class T>
  constexpr bool receives = isReceiver<remove_cv_ref_t<T>>::value;

  template<class T, class Sfinae = void>
  struct canMakeIntoSender : std::false_type {};

  template<class T>
  constexpr bool canSend = canMakeIntoSender<T>::value;

  template<class T, class Sfinae = void>
  struct canMakeIntoReceiver : std::false_type {};

  template<class T>
  constexpr bool canReceive = canMakeIntoReceiver<T>::value;

  template<class T, class SFINAE = void>
  struct isCompletedT : std::false_type {};

  template<class T>
  constexpr bool isCompleted = isCompletedT<remove_cv_ref_t<T>>::value;
}

namespace tillh::pipes2
{
  template<class T>
  struct canPrimaryConnectT : std::false_type
  {
    static_assert(fail_assert<T>);
  };

  template<class T>
  constexpr static bool canPrimaryConnect_v = canPrimaryConnectT<T>::value;

  template<class T, class Sfinae = void>
  struct canSecondaryConnectT : std::false_type {};

  template<class T>
  constexpr static bool canSecondaryConnect_v = canPrimaryConnectT<T>::value;
}
