#include "catch.hpp"

#include <vector>
#include <map>
#include <list>
#include <set>

#include "pipes/detail/util/is_pushable.hpp"
#include "pipes/detail/util/metaprogramming.hpp"

using namespace tillh::pipes::util;

TEST(isPushable, vector)
{
  using Type = std::vector<int>;
  static_assert(is_detected<push_back_expression, Type>);
  static_assert(is_pushable<Type>);
}

TEST(isPushable, list)
{
  using Type = std::list<int>;
  static_assert(is_detected<push_back_expression, Type>);
  static_assert(is_pushable<Type>);
}

TEST(isPushable, noMap)
{
  using Type = std::map<int, int>;
  static_assert(!is_detected<push_back_expression, Type>);
  static_assert(!is_pushable<Type>);
}

TEST(isPushable, noSet)
{
  using Type = std::set<int>;
  static_assert(!is_detected<push_back_expression, Type>);
  static_assert(!is_pushable<Type>);
}

TEST(isPushable, noInt)
{
  using Type = int;
  static_assert(!is_detected<push_back_expression, Type>);
  static_assert(!is_pushable<Type>);
}

TEST(isPushable, noFloat)
{
  using Type = float;
  static_assert(!is_detected<push_back_expression, Type>);
  static_assert(!is_pushable<Type>);
}

TEST(isPushable, noBool)
{
  using Type = bool;
  static_assert(!is_detected<push_back_expression, Type>);
  static_assert(!is_pushable<Type>);
}
