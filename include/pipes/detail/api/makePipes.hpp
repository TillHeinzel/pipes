#pragma once

#include "pipes/detail/util/is_range.hpp"

#include "pipes/detail/core/Node.hpp"

#include "pipes/detail/pipes/throughput.hpp"
#include "pipes/detail/pipes/switch.hpp"
#include "pipes/detail/pipes/sinks.hpp"
#include "pipes/detail/pipes/sources.hpp"
#include "pipes/detail/pipes/makeSource.hpp"
#include "pipes/detail/pipes/makeSink.hpp"

#include "pipes/detail/api/ensureValidInputOutput.hpp"

// throughput pipes
namespace tillh
{
  namespace pipes
  {
    namespace api
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
        return connectSecondary(node, FWD(outputs)...);
      }

      template<class Output>
      auto tee(Output&& output)
      {
        auto node = makeNode<true, 1>(Demux());
        return connectSecondary(node, FWD(output));
      }

      template<class F, class Output>
      auto case_(F f, Output&& output)
      {
        return makeCase(f, ensureValidOutput(FWD(output)));
      }

      template<class Output>
      auto default_(Output&& output)
      {
        return makeCase(DefaultCondition(), ensureValidOutput(FWD(output)));
      }

      template<class... Conditions, class... Outputs>
      auto switch_(Case<Conditions, Outputs>... cases)
      {
        auto node = makeNode<false, sizeof...(Conditions)>(makeSwitch(std::make_tuple(std::move(cases.condition)...)));
        return connectSecondary(node, std::move(cases.output)...);
      }

      template<class Condition, class TrueBranch, class FalseBranch>
      auto partition(Condition condition, TrueBranch&& trueBranch, FalseBranch&& falseBranch)
      {
        return switch_(case_(condition, FWD(trueBranch)), default_(FWD(falseBranch)));
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
        return connectSecondary(node, FWD(outputs)...);
      }

      inline static constexpr OpenConnectionPlaceHolder _ = {};
    }
  }
}

// endpipes

namespace tillh
{
  namespace pipes
  {
    namespace api
    {
      inline auto discard()
      {
        return makeSinkOutput(Discard());
      }

      template<class Pushable>
      auto push_back(Pushable& pushable)
      {
        static_assert(util::is_pushable<Pushable>);
        return makeIteratorSink(std::back_inserter(pushable));
      }

      template<class Collection>
      auto override(Collection& collection)
      {
        static_assert(util::is_range<Collection>);
        return makeIteratorSink(collection.begin());
      }

      template<class F>
      auto custom(F&& f)
      {
        return makeSinkOutput(Custom<util::remove_cv_ref_t<F>>(std::forward<F>(f)));
      }

      template<class Map, class F>
      auto map_aggregator(Map& map, F aggregationFunction)
      {
        return makeSinkOutput(MapAggregator(map, aggregationFunction));
      }

      template<class It>
      auto iterator(It it)
      {
        static_assert(util::is_output_iterator<It>);
        return makeIteratorSink(it);
      }

      template<class Stream>
      auto toStream(Stream& stream)
      {
        static_assert(std::is_base_of_v<std::ostream, util::remove_cv_ref_t<Stream>>);
        return makeSink(stream);
      }
    }
  }
}

// sourcepipes

namespace tillh
{
  namespace pipes
  {
    namespace api
    {
      template<class T = std::string, class Stream>
      auto fromStream(Stream&& stream)
      {
        static_assert(std::is_base_of_v<std::istream, util::remove_cv_ref_t<Stream>>);
        static_assert(!std::is_const_v<decltype(stream)>);
        return makeSource<T>(std::forward<Stream>(stream));
      }
    }
  }
}