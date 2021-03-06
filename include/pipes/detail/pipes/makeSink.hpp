#pragma once

#include <type_traits>
#include <iterator>
#include <tuple>

#include "pipes/detail/util/metaprogramming.hpp"
#include "pipes/detail/util/is_pushable.hpp"
#include "pipes/detail/util/is_output_iterator.hpp"

#include "pipes/detail/core/Output.hpp"

#include "pipes/detail/pipes/sinks.hpp"

namespace tillh
{
  namespace pipes
  {
    template<class Op>
    auto makeSinkOutput(Op op)
    {
      return Output<Op, std::tuple<>>(op, std::tuple<>{});
    }

    template<class It>
    auto makeIteratorSink(It it)
    {
      static_assert(util::is_output_iterator<It>, "");
      return makeSinkOutput(IteratorSink<It>(it));
    }

    template<class Pushable, std::enable_if_t<util::is_pushable<Pushable>, bool> = true>
    auto makeSink(Pushable & pushable)
    {
      return makeIteratorSink(std::back_inserter(pushable));
    }

    template<class It, std::enable_if_t<util::is_output_iterator<It>, bool> = true>
    auto makeSink(It it)
    {
      return makeIteratorSink(it);
    }

    template<class Stream, std::enable_if_t<std::is_base_of_v<std::ostream, util::remove_cv_ref_t<Stream>>, bool> = true>
    auto makeSink(Stream & stream)
    {
      return makeSinkOutput(ToStream<util::remove_cv_ref_t<Stream>>(stream));
    }
  }
}