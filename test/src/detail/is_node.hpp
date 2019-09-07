#pragma once

template<class T>
struct is_node : std::false_type {};

template<class Op, class Connections, class PrimaryConnection>
struct is_node<tillh::pipes2::Node<Op, Connections, PrimaryConnection>> : std::true_type {};

template<class T>
constexpr auto is_node_v = is_node<T>::value;

template<class Op, class Outputs>
struct is_node<tillh::pipes2::Output<Op, Outputs>> : std::true_type {};
