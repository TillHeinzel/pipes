#pragma once

#include <type_traits>
#include <iterator>
#include <tuple>

#include "pipes2/detail/util/metaprogramming.hpp"
#include "pipes2/detail/util/is_pushable.hpp"
#include "pipes2/detail/util/is_output_iterator.hpp"

#include "pipes2/detail/core/Output.hpp"

#include "pipes2/detail/pipes/sinks.hpp"

namespace tillh::pipes2::detail
{
  template<class Op>
  auto makeSinkOutput(Op op)
  {
    return Output<Op, std::tuple<>>(op, std::tuple<>{});
  }

  template<class It>
  auto makeIteratorSink(It it)
  {
    static_assert(is_output_iterator_v<It>, "");
    return makeSinkOutput(IteratorSink<It>(it));
  }

  template<class Pushable, std::enable_if_t<is_pushable_v<Pushable>, bool> = true>
  auto makeSink(Pushable & pushable)
  {
    return makeIteratorSink(std::back_inserter(pushable));
  }

  template<class It, std::enable_if_t<is_output_iterator_v<It>, bool> = true>
  auto makeSink(It it)
  {
    return makeIteratorSink(it);
  }

  template<class Stream, std::enable_if_t<std::is_base_of_v<std::ostream, remove_cv_ref_t<Stream>>, bool> = true>
  auto makeSink(Stream& stream)
  {
    return makeSinkOutput(ToStream<remove_cv_ref_t<Stream>>(stream));
  }
}
