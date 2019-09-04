#include <gtest/gtest.h>

#include <vector>
#include <map>
#include <list>
#include <set>

#include "pipes2/detail/util/is_range.hpp"

using namespace tillh::pipes2;

TEST(isRange, vector)
{
  using Type = std::vector<int>;
  static_assert(detail::is_detected_v<begin_expression, Type>);
  static_assert(detail::is_detected_v<end_expression, Type>);
  static_assert(is_range_v<Type>);
}

TEST(isRange, map)
{
  using Type = std::map<int, int>;
  static_assert(detail::is_detected_v<begin_expression, Type>);
  static_assert(detail::is_detected_v<end_expression, Type>);
  static_assert(is_range_v<Type>);
}

TEST(isRange, set)
{
  using Type = std::set<int>;
  static_assert(detail::is_detected_v<begin_expression, Type>);
  static_assert(detail::is_detected_v<end_expression, Type>);
  static_assert(is_range_v<Type>);
}

TEST(isRange, list)
{
  using Type = std::list<int>;
  static_assert(detail::is_detected_v<begin_expression, Type>);
  static_assert(detail::is_detected_v<end_expression, Type>);
  static_assert(is_range_v<Type>);
}

TEST(isRange, noInt)
{
  using Type = int;
  static_assert(!detail::is_detected_v<begin_expression, Type>);
  static_assert(!detail::is_detected_v<end_expression, Type>);
  static_assert(!is_range_v<Type>);
}

TEST(isRange, noFloat)
{
  using Type = float;
  static_assert(!detail::is_detected_v<begin_expression, Type>);
  static_assert(!detail::is_detected_v<end_expression, Type>);
  static_assert(!is_range_v<Type>);
}

TEST(isRange, noBool)
{
  using Type = bool;
  static_assert(!detail::is_detected_v<begin_expression, Type>);
  static_assert(!detail::is_detected_v<end_expression, Type>);
  static_assert(!is_range_v<Type>);
}
