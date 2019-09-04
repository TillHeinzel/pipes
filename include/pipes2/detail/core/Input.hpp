#pragma once

namespace tillh::pipes2
{
  template<class Source, class Node>
  struct Input
  {
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
  auto makeInput(Source source, Node node)
  {
    return Input<Source, Node>{source, node};
  }

}
