#pragma once

#include "pipes2/detail/util/is_range.hpp"

#include "pipes2/detail/core/Node.hpp"

#include "pipes2/detail/pipes/throughput.hpp"
#include "pipes2/detail/pipes/switch.hpp"
#include "pipes2/detail/pipes/sinks.hpp"
#include "pipes2/detail/pipes/sources.hpp"

#include "pipes2/detail/api/ensureValidOutput.hpp"

// throughput pipes
namespace tillh::pipes2
{
  template<class F>
  auto filter(F f)
  {
    return makeNode<true, 0>(Filter(f));
  }

  template<class F>
  auto transform(F f)
  {
    return makeNode<true, 0>(Transform(f));
  }

  template<class... Outputs>
  auto demux(Outputs&& ... outputs)
  {
    static_assert(sizeof...(Outputs) > 0, "demux requires at least one output");
    auto node = makeNode<false, sizeof...(Outputs)>(Demux());
    return detail::connectAllNonRecursive(node, detail::ensureValidOutput(std::forward<Outputs>(outputs))...);
  }

  template<class Output>
  auto tee(Output&& output)
  {
    auto node = makeNode<true, 1>(Demux());
    return connectSecondary(node, detail::ensureValidOutput(std::forward<Output>(output)));
  }

  template<class F, class Output>
  auto case_(F f, Output&& output)
  {
    return makeCase(f, detail::ensureValidOutput(std::forward<Output>(output)));
  }

  template<class Output>
  auto default_(Output&& output)
  {
    return makeCase(DefaultCondition(), detail::ensureValidOutput(std::forward<Output>(output)));
  }

  template<class... Conditions, class... Outputs>
  auto switch_(Case<Conditions, Outputs>... cases)
  {
    auto node = makeNode<false, sizeof...(Conditions)>(makeSwitch(std::make_tuple(std::move(cases.condition)...)));
    return detail::connectAllNonRecursive(node, std::move(cases.output)...);
  }

  template<class Condition, class OutputTrue, class OutputFalse>
  auto partition(Condition condition, OutputTrue&& trueBranch, OutputFalse&& falseBranch)
  {
    return switch_(case_(condition, std::forward<OutputTrue>(trueBranch)), default_(std::forward<OutputFalse>(falseBranch)));
  }

  inline auto join()
  {
    return makeNode<true, 0>(Join());
  }

  template<class... Outputs>
  auto unzip(Outputs&& ... outputs)
  {
    static_assert(sizeof...(Outputs) > 0, "unzip requires at least one output");
    auto node = makeNode<false, sizeof...(Outputs)>(Unzip());
    return detail::connectAllNonRecursive(node, detail::ensureValidOutput(std::forward<Outputs>(outputs))...);
  }

  inline static constexpr OpenConnectionPlaceHolder _ = {};
}

// endpipes
namespace tillh::pipes2
{
  inline auto discard()
  {
    return detail::makeSinkOutput(Discard());
  }

  template<class Pushable>
  auto push_back(Pushable& pushable)
  {
    static_assert(is_pushable_v<Pushable>);
    return detail::makeIteratorSink(std::back_inserter(pushable));
  }

  template<class Collection>
  auto override(Collection& collection)
  {
    static_assert(is_range_v<Collection>);
    return detail::makeIteratorSink(collection.begin());
  }

  template<class F>
  auto custom(F&& f)
  {
    return detail::makeSinkOutput(Custom<remove_cv_ref_t<F>>(std::forward<F>(f)));
  }

  template<class Map, class F>
  auto map_aggregator(Map& map, F aggregationFunction)
  {
    return detail::makeSinkOutput(MapAggregator(map, aggregationFunction));
  }

  template<class It>
  auto iterator(It it)
  {
    static_assert(is_output_iterator_v<It>);
    return detail::makeIteratorSink(it);
  }

  template<class Stream>
  auto toStream(Stream& stream)
  {
    static_assert(std::is_base_of_v<std::ostream, remove_cv_ref_t<Stream>>);
    return detail::makeSink(stream);
  }
}

// sourcepipes
namespace tillh::pipes2
{
  template<class T = std::string, class Stream>
  auto fromStream(Stream&& stream)
  {
    static_assert(std::is_base_of_v<std::istream, remove_cv_ref_t<Stream>>);
    static_assert(!std::is_const_v<decltype(stream)>);
    return detail::makeSource<T>(std::forward<Stream>(stream));
  }
}
