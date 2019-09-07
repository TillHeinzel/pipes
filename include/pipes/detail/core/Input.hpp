#pragma once

#include "pipes/detail/util/metaprogramming.hpp"

namespace tillh
{
  namespace pipes
  {
    template<class Source, class Node>
    struct Input
    {
      template<class Source_, class Node_>
      Input(Source_&& source_, Node_&& node_): source(std::forward<Source_>(source_)), node(std::forward<Node_>(node_)) {}

      Source source;
      Node node;
    };

    template<class T>
    struct is_input : std::false_type {};

    template<class Source, class Node>
    struct is_input<Input<Source, Node>> : std::true_type {};

    template<class T>
    constexpr bool is_input_v = is_input<T>::value;

    template<class Source, class Node>
    auto makeInput(Source&& source, Node node)
    {
      return Input<util::remove_cv_ref_t<Source>, Node>(std::forward<Source>(source), node);
    }
  }
}
