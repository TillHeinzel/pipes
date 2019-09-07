#pragma once

#include <type_traits>

#include "pipes/detail/util/metaprogramming.hpp"

namespace tillh
{
  namespace pipes
  {
    template<class T>
    struct isSender : std::false_type {};

    template<class T>
    constexpr bool sends = isSender<util::remove_cv_ref_t<T>>::value;

    template<class T>
    struct isReceiver : std::false_type {};

    template<class T>
    constexpr bool receives = isReceiver<util::remove_cv_ref_t<T>>::value;

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
    constexpr bool isCompleted = isCompletedT<util::remove_cv_ref_t<T>>::value;
  }
}

namespace tillh
{
  namespace pipes
  {
    template<class T>
    struct canPrimaryConnectT : std::false_type
    {
      static_assert(util::fail_assert<T>);
    };

    template<class T>
    constexpr static bool canPrimaryConnect = canPrimaryConnectT<T>::value;

    template<class T, class Sfinae = void>
    struct canSecondaryConnectT : std::false_type
    {
      static_assert(util::fail_assert<T>);
    };

    template<class T>
    constexpr static bool canSecondaryConnect = canSecondaryConnectT<T>::value;
  }
}