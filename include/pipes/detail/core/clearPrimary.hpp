#pragma once

#include "pipes/detail/core/Node.hpp"

namespace tillh
{
  namespace pipes
  {
    template<class T>
    decltype(auto) clearPrimary(T&& t)
    {
      return FWD(t);
    }

    inline auto clearPrimary(PrimaryOpenConnectionPlaceHolder)
    {
      return OpenConnectionPlaceHolder();
    }

    template<class Op, class Connections>
    auto clearPrimary(Node<Op, Connections>&& node)
    {
      return apply_last(std::move(node), [](auto&& connection) {return clearPrimary(FWD(connection)); });
    }
  }
}