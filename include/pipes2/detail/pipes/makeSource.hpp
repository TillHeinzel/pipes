#pragma once

#include <type_traits>
#include <utility>
#include <string>

#include "pipes2/detail/util/metaprogramming.hpp"
#include "pipes2/detail/util/is_range.hpp"

#include "pipes2/detail/core/Input.hpp"
#include "pipes2/detail/core/Node.hpp"

#include "pipes2/detail/pipes/sources.hpp"

namespace tillh::pipes2::detail
{
  template<class T>
  auto makeSourceInput(T&& t)
  {
    return makeInput(std::forward<T>(t), PrimaryOpenConnectionPlaceHolder());
  }

  template<class Range, std::enable_if_t<is_range_v<remove_cv_ref_t<Range>>, bool> = true>
  auto makeSource(Range && range)
  {
    if constexpr(std::is_rvalue_reference_v<decltype(range)>)
    {
      return makeSourceInput(MoveSource<Range>(range));
    }
    else
    {
      return makeSourceInput(CopySource<Range>(range));
    }
  }

  template<class T = std::string, class Stream, std::enable_if_t<std::is_base_of_v<std::istream, remove_cv_ref_t<Stream>>, bool> = true>
  auto makeSource(Stream && stream)
  {
    return makeSourceInput(StreamSource<remove_cv_ref_t<Stream>, T>(std::forward<Stream>(stream)));
  }
}
