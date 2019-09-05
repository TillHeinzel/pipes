#pragma once

#include <type_traits>

#include "pipes2/detail/util/is_range.hpp"
#include "pipes2/detail/util/is_pushable.hpp"
#include "pipes2/detail/util/is_output_iterator.hpp"

#include "pipes2/detail/core/connect.hpp"
#include "pipes2/detail/core/evaluate.hpp"
#include "pipes2/detail/core/Input.hpp"
#include "pipes2/detail/core/Node.hpp"
#include "pipes2/detail/core/Output.hpp"

#include "pipes2/detail/pipes/sources.hpp"
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

namespace tillh::pipes2::detail
{
  template<class T>
  auto makeSourceInput(T&& t)
  {
    return makeInput(std::forward<T>(t), OpenConnectionPlaceHolder());
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

namespace tillh::pipes2::detail
{
  template<class T>
  struct isSender : std::false_type {};

  template<class Source, class Tree>
  struct isSender<Input<Source, Tree>> : std::true_type {};

  template<class Op, class Connections, class PrimaryConnection>
  struct isSender<Node<Op, Connections, PrimaryConnection>> : std::true_type {};

  template<class T>
  struct isReceiver : std::false_type {};

  template<class Op, class Connections>
  struct isReceiver<Output<Op, Connections>> : std::true_type {};

  template<class Op, class Connections, class PrimaryConnection>
  struct isReceiver<Node<Op, Connections, PrimaryConnection>> : std::true_type {};

  template<>
  struct isReceiver<OpenConnectionPlaceHolder> : std::true_type {};

  template<class T>
  constexpr bool sends = isSender<remove_cv_ref_t<T>>::value;

  template<class T>
  constexpr bool receives = isReceiver<remove_cv_ref_t<T>>::value;

  template<class T>
  using makeSourceExpression = decltype(makeSource(std::declval<T>()));

  template<class T>
  constexpr bool canSend = is_detected_v<makeSourceExpression, T>;

  template<class T>
  using makeSinkExpression = decltype(makeSink(std::declval<T>()));

  template<class T>
  constexpr bool canReceive = is_detected_v<makeSinkExpression, T>;

  template<class T, class SFINAE = void>
  struct isCompletedT : std::false_type {};

  template<class Source, class Op, class Connections>
  struct isCompletedT<Input<Source, Output<Op, Connections>>> : std::true_type {};

  template<class T>
  constexpr bool isCompleted = isCompletedT<remove_cv_ref_t<T>>::value;
}


namespace tillh::pipes2::detail
{
  template<class T, std::enable_if_t<receives<T>, bool> = true>
  auto ensureValidOutput(T t)
  {
    return t;
  }

  template<class T, std::enable_if_t<!receives<T>&& canReceive<T>, bool> = true>
  auto ensureValidOutput(T && t)
  {
    return makeSink(std::forward<T>(t));
  }
}
