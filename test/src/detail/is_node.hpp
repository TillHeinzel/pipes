#pragma once

template<class T>
struct is_node : std::false_type {};

template<class Op, class Connections>
struct is_node<tillh::pipes::Node<Op, Connections>> : std::true_type {};

template<class T>
constexpr auto is_node_v = is_node<T>::value;

template<class Op, class Outputs>
struct is_node<tillh::pipes::Output<Op, Outputs>> : std::true_type {};
