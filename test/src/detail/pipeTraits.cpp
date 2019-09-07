#include <gtest/gtest.h>

#include <string>

#include <pipes2/pipes.hpp>

#include "is_node.hpp"

namespace pipes = tillh::pipes2;
using namespace pipes;

using namespace std::literals;

TEST(pipeTraits, transform)
{
  const auto trans = transform([](auto i) {return i + 1; });
  using type = remove_cv_ref_t<decltype(trans)>;
  static_assert(canPrimaryConnect<type>);
  static_assert(is_node_v<type>);
  static_assert(!is_range_v<type>);
  static_assert(!detail::is_detected_v<begin_expression, type>);
}

TEST(pipeTraits, filter)
{
  const auto filte = filter([](auto i) {return (i % 2) == 0; });
  using type = remove_cv_ref_t<decltype(filte)>;
  static_assert(canPrimaryConnect<type>);
  static_assert(is_node_v<type>);
  static_assert(!is_range_v<type>);
  static_assert(!detail::is_detected_v<begin_expression, type>);
}

TEST(pipeTraits, demux1)
{
  auto target = std::vector<int>{};
  const auto demu = demux(target);
  using type = remove_cv_ref_t<decltype(demu)>;
  static_assert(is_node_v<type>);
  static_assert(is_output_v<type>);
  static_assert(!is_range_v<type>);
}

TEST(pipeTraits, demux2)
{
  auto target1 = std::vector<int>{};
  auto target2 = std::vector<int>{};
  const auto demu = demux(target1, target2);
  using type = remove_cv_ref_t<decltype(demu)>;
  static_assert(is_node_v<type>);
  static_assert(is_output_v<type>);
  static_assert(!is_range_v<type>);
}