#pragma once

#include "pipes/detail/util/metaprogramming.hpp"

#include "pipes/detail/core/Node.hpp"

namespace tillh
{
  namespace pipes
  {
    template<class T>
    struct openCountT : util::int_constant<0> {};

    template<>
    struct openCountT<OpenConnectionPlaceHolder> : util::int_constant<1> {};

    template<class Op, class... Connections>
    struct openCountT<Node<Op, std::tuple<Connections...>>> : util::sum<openCountT<Connections>...> {};

    template<class T>
    constexpr static std::size_t openCount = openCountT<T>::value;
  }
}
