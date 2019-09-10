#pragma once

#include <utility>
#include <algorithm>
#include <tuple>

#include "pipes/detail/util/metaprogramming.hpp"

namespace tillh
{
  namespace pipes
  {
    template<std::size_t add, std::size_t... Is>
    constexpr auto addTo(std::index_sequence<Is...>)
    {
      return std::index_sequence<(Is + add)...>();
    }

    template<std::size_t accu, std::size_t max, std::size_t currentCount>
    constexpr auto getIndices_x()
    {
      if constexpr(accu >= max)
      {
        return std::make_index_sequence<0>();
      }
      else
      {
        return addTo<accu>(std::make_index_sequence<std::min(currentCount, max - accu)>());
      }
    }

    template<std::size_t max, std::size_t accu>
    constexpr auto getIndices_impl()
    {
      return std::tuple<>();
    }

    template<std::size_t max, std::size_t accu, class T, class... Ts>
    constexpr auto getIndices_impl(util::Type<T>, util::Type<Ts>...)
    {
      return std::tuple_cat(std::make_tuple(getIndices_x<accu, max, openCount<T>>()), getIndices_impl<max, accu + openCount<T>>(util::Type<Ts>()...));
    }

    template<std::size_t max, class... Ts>
    constexpr auto getIndices(util::TypeList<Ts...>)
    {
      return getIndices_impl<max, 0>(util::Type<Ts>()...);
    }

    template<std::size_t max, class Node>
    constexpr auto getIndices()
    {
      static_assert(max <= openCount<Node>);

      return getIndices<max>(
        util::typeList(
          util::Type<typename Node::Connections>()
        ));
    }
  }
}