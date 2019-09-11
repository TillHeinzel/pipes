#include "../catch.hpp"

#include <vector>
#include <map>
#include <list>
#include <set>

#include "pipes/detail/util/is_dereferencable.hpp"

using namespace tillh::pipes::util;

TEST_CASE("isDereferencable")
{
  SECTION("pointer")
  {
    using Type = int*;
    static_assert(is_dereferencable<Type>);
  }

  SECTION("vectorIterator")
  {
    using Type = std::vector<int>::iterator;
    static_assert(is_dereferencable<Type>);
  }

  SECTION("vectorBackInserter")
  {
    auto v = std::vector{0,1,2,3};
    using Type = decltype(std::back_inserter(v));
    static_assert(is_dereferencable<Type>);
  }

  SECTION("notInt")
  {
    using Type = int;
    static_assert(!is_dereferencable<Type>);
  }

  SECTION("notVector")
  {
    using Type = std::vector<int>;
    static_assert(!is_dereferencable<Type>);
  }

  SECTION("notVoid")
  {
    using Type = void;
    static_assert(!is_dereferencable<Type>);
  }
}