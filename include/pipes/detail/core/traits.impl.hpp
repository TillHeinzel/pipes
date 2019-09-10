#pragma once

#include <type_traits>

#include "pipes/detail/core/traits.hpp"

#include "pipes/detail/core/Input.hpp"
#include "pipes/detail/core/Node.hpp"
#include "pipes/detail/core/Output.hpp"

#include "pipes/detail/pipes/makeSink.hpp"
#include "pipes/detail/pipes/makeSource.hpp"

namespace tillh
{
  namespace pipes
  {
    template<class Source, class Tree>
    struct isSender<Input<Source, Tree>> : std::true_type {};

    template<class Op, class Connections>
    struct isSender<Node<Op, Connections>> : std::true_type {};

    template<class Op, class Connections>
    struct isReceiver<Output<Op, Connections>> : std::true_type {};

    template<class Op, class Connections>
    struct isReceiver<Node<Op, Connections>> : std::true_type {};

    template<>
    struct isReceiver<OpenConnectionPlaceHolder> : std::true_type {};

    template<class T>
    using makeSourceExpression = decltype(makeSource(std::declval<T>()));

    template<class T>
    struct canMakeIntoSender<T, std::enable_if_t<util::is_detected<makeSourceExpression, T>>> : std::true_type {};

    template<class T>
    using makeSinkExpression = decltype(makeSink(std::declval<T>()));

    template<class T>
    struct canMakeIntoReceiver<T, std::enable_if_t<util::is_detected<makeSinkExpression, T>>> : std::true_type {};

    template<class Source, class Op, class Connections>
    struct isCompletedT<Input<Source, Output<Op, Connections>>> : std::true_type {};
  }
}

namespace tillh
{
  namespace pipes
  {
    template<class Op, class Connections>
    struct canPrimaryConnectT<Output<Op, Connections>> : std::false_type {};

    template<>
    struct canPrimaryConnectT<PrimaryOpenConnectionPlaceHolder> : std::true_type {};

    template<>
    struct canPrimaryConnectT<OpenConnectionPlaceHolder> : std::false_type {};

    template<class Op, class Connections>
    struct canPrimaryConnectT<Node<Op, Connections>> : canPrimaryConnectT<util::last_element<Connections>> {};
  }
}

namespace tillh
{
  namespace pipes
  {
    template<>
    struct canSecondaryConnectT<OpenConnectionPlaceHolder> : std::true_type {};

    template<>
    struct canSecondaryConnectT<PrimaryOpenConnectionPlaceHolder> : std::false_type {};

    template<class Op, class Connections>
    struct canSecondaryConnectT<Output<Op, Connections>> : std::false_type {};

    template<class Op, class... Connections>
    struct canSecondaryConnectT<Node<Op, std::tuple<Connections...>>> : std::disjunction<canSecondaryConnectT<Connections>...> {};
  }
}
