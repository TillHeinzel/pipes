#pragma once

#include "pipes2/detail/core/traits.hpp"

#include "pipes2/detail/core/Input.hpp"
#include "pipes2/detail/core/Node.hpp"
#include "pipes2/detail/core/Output.hpp"

#include "pipes2/detail/pipes/makeSink.hpp"
#include "pipes2/detail/pipes/makeSource.hpp"

namespace tillh::pipes2::detail
{
  template<class Source, class Tree>
  struct isSender<Input<Source, Tree>> : std::true_type {};

  template<class Op, class Connections, class PrimaryConnection>
  struct isSender<Node<Op, Connections, PrimaryConnection>> : std::true_type {};

  template<class Op, class Connections>
  struct isReceiver<Output<Op, Connections>> : std::true_type {};

  template<class Op, class Connections, class PrimaryConnection>
  struct isReceiver<Node<Op, Connections, PrimaryConnection>> : std::true_type {};

  template<>
  struct isReceiver<OpenConnectionPlaceHolder> : std::true_type {};

  template<class T>
  using makeSourceExpression = decltype(makeSource(std::declval<T>()));

  template<class T>
  struct canMakeIntoSender<T, std::enable_if_t<is_detected_v<makeSourceExpression, T>>> : std::true_type{};

  template<class T>
  using makeSinkExpression = decltype(makeSink(std::declval<T>()));

  template<class T>
  struct canMakeIntoReceiver<T, std::enable_if_t<is_detected_v<makeSinkExpression, T>>> : std::true_type {};
  
  template<class Source, class Op, class Connections>
  struct isCompletedT<Input<Source, Output<Op, Connections>>> : std::true_type {};
}
