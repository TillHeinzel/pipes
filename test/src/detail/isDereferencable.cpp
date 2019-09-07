#include <gtest/gtest.h>

#include <vector>
#include <map>
#include <list>
#include <set>

#include "pipes/detail/util/is_dereferencable.hpp"

using namespace tillh::pipes::util;

TEST(isDereferencable, pointer)
{
  using Type = int*;
  static_assert(is_dereferencable_v<Type>);
}

TEST(isDereferencable, vectorIterator)
{
  using Type = std::vector<int>::iterator;
  static_assert(is_dereferencable_v<Type>);
}

TEST(isDereferencable, vectorBackInserter)
{
  auto v = std::vector{0,1,2,3};
  using Type = decltype(std::back_inserter(v));
  static_assert(is_dereferencable_v<Type>);
}

TEST(isDereferencable, notInt)
{
  using Type = int;
  static_assert(!is_dereferencable_v<Type>);
}

TEST(isDereferencable, notVector)
{
  using Type = std::vector<int>;
  static_assert(!is_dereferencable_v<Type>);
}

TEST(isDereferencable, notVoid)
{
  using Type = void;
  static_assert(!is_dereferencable_v<Type>);
}
 