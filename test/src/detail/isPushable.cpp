#include <gtest/gtest.h>

#include <vector>
#include <map>
#include <list>
#include <set>

#include "pipes2/detail/util/is_pushable.hpp"

using namespace tillh::pipes2;

TEST(isPushable, vector)
{
  using Type = std::vector<int>;
  static_assert(detail::is_detected_v<push_back_expression, Type>);
  static_assert(is_pushable_v<Type>);
}

TEST(isPushable, list)
{
  using Type = std::list<int>;
  static_assert(detail::is_detected_v<push_back_expression, Type>);
  static_assert(is_pushable_v<Type>);
}

TEST(isPushable, noMap)
{
  using Type = std::map<int, int>;
  static_assert(!detail::is_detected_v<push_back_expression, Type>);
  static_assert(!is_pushable_v<Type>);
}

TEST(isPushable, noSet)
{
  using Type = std::set<int>;
  static_assert(!detail::is_detected_v<push_back_expression, Type>);
  static_assert(!is_pushable_v<Type>);
}

TEST(isPushable, noInt)
{
  using Type = int;
  static_assert(!detail::is_detected_v<push_back_expression, Type>);
  static_assert(!is_pushable_v<Type>);
}

TEST(isPushable, noFloat)
{
  using Type = float;
  static_assert(!detail::is_detected_v<push_back_expression, Type>);
  static_assert(!is_pushable_v<Type>);
}

TEST(isPushable, noBool)
{
  using Type = bool;
  static_assert(!detail::is_detected_v<push_back_expression, Type>);
  static_assert(!is_pushable_v<Type>);
}
