#include "catch.hpp"

#include <vector>
#include <map>
#include <list>
#include <set>

#include "pipes/detail/util/is_incrementable.hpp"

using namespace tillh::pipes::util;

TEST(isIncrementable, vectorIterator)
{
  using Type = std::vector<int>::iterator;
  static_assert(is_iterator_incrementable<Type>);
}

TEST(isIncrementable, vectorBackInserter)
{
  auto v = std::vector{0,1,2,3};
  using Type = decltype(std::back_inserter(v));
  static_assert(is_iterator_incrementable<Type>);
}

TEST(isIncrementable, int)
{
  using Type = int;
  static_assert(!is_iterator_incrementable<Type>);
}

TEST(isIncrementable, notVector)
{
  using Type = std::vector<int>;
  static_assert(!is_iterator_incrementable<Type>);
}

TEST(isIncrementable, notVoid)
{
  using Type = void;
  static_assert(!is_iterator_incrementable<Type>);
}