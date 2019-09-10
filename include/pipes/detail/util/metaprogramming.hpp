#pragma once

#include <type_traits>
#include <tuple>

#include "pipes/detail/util/FWD.hpp"

namespace tillh
{
  namespace pipes
  {
    namespace util
    {
      template<class... T>
      struct fail_assertT : std::false_type {};

      template<class... T>
      constexpr static bool fail_assert = false;

      template< class T >
      struct remove_cv_ref
      {
        typedef std::remove_cv_t<std::remove_reference_t<T>> type;
      };

      template<class T> using remove_cv_ref_t = typename remove_cv_ref<T>::type;

      template<std::size_t pickIndex, std::size_t currentIndex, class Tuple, class T >
      auto pick(Tuple&& tuple, T&& t)
      {
        if constexpr(pickIndex == currentIndex)
        {
          return FWD(t);
        }
        else
        {
          return std::get<currentIndex>(FWD(tuple));
        }
      }

      template<std::size_t index, std::size_t... Is, class Tuple, class T>
      auto tuple_replace_impl(Tuple&& tuple, T&& t, std::index_sequence<Is...>)
      {
        return std::make_tuple(std::move(pick<index, Is>(FWD(tuple), FWD(t)))...);
      }

      template<std::size_t index, class Tuple, class T>
      auto tuple_replace(Tuple&& tuple, T&& t)
      {
        return tuple_replace_impl<index>(FWD(tuple), FWD(t), std::make_index_sequence<std::tuple_size_v<util::remove_cv_ref_t<Tuple>>>());
      }

      template<class T>
      struct Type
      {
        constexpr Type() = default;
        using type = T;
      };

      template<class... Ts>
      struct TypeList {};

      template<class... Ts>
      constexpr auto typeList(Type<std::tuple<Ts...>>)
      {
        return TypeList<Ts...>{};
      }

      template<class... Ts, class T>
      constexpr auto append(TypeList<Ts...>, Type<T>)
      {
        return TypeList<Ts..., T>();
      }
      
      template<typename...>
      using try_to_instantiate = void;

      using disregard_this = void;

      template<template<typename...> class Expression, typename Attempt, typename... Ts>
      struct is_detected_T : std::false_type {};

      template<template<typename...> class Expression, typename... Ts>
      struct is_detected_T<Expression, try_to_instantiate<Expression<Ts...>>, Ts...> : std::true_type {};

      template<template<typename...> class Expression, typename... Ts>
      constexpr bool is_detected = is_detected_T<Expression, disregard_this, Ts...>::value;
    }
  }
}

namespace tillh
{
  namespace pipes
  {
    namespace util
    {
      template <typename...>
      struct is_one_of;

      template <typename F>
      struct is_one_of<F>
      {
        static constexpr bool value = false;
      };

      template <typename F, typename S, typename... T>
      struct is_one_of<F, S, T...>
      {
        static constexpr bool value = std::is_same<F, S>::value
          || is_one_of<F, T...>::value;
      };

      template <typename...>
      struct is_unique : std::false_type {};

      template <>
      struct is_unique<> : std::true_type {};

      template<typename F, typename... T>
      struct is_unique<F, T...>
      {
        static constexpr bool value = is_unique<T...>::value
          && !is_one_of<F, T...>::value;
      };

      template<class... Ts>
      constexpr bool is_unique_v = is_unique<Ts...>::value;
    }
  }
}

namespace tillh
{
  namespace pipes
  {
    namespace util
    {
      template<class T, class... Ts>
      decltype(auto) getFirst(T&& t, Ts&& ...)
      {
        return std::forward<T>(t);
      }
    }
  }
}
namespace tillh
{
  namespace pipes
  {
    namespace util
    {
      template<std::size_t I>
      using int_constant = std::integral_constant<std::size_t, I>;

      template<class... Ts>
      using sum = int_constant<(Ts::value + ...)>;
    }
  }
}
namespace tillh
{
  namespace pipes
  {
    namespace util
    {
      template<class Tuple>
      constexpr static std::size_t tuple_back = std::tuple_size_v<Tuple> -1;

      template<class Tuple>
      using last_element = std::tuple_element_t<tuple_back<Tuple>, Tuple>;

      template<class Tuple>
      decltype(auto) getLast(Tuple&& tuple)
      {
        return std::get<tuple_back<util::remove_cv_ref_t<Tuple>>>(FWD(tuple));
      }
    }
  }
}