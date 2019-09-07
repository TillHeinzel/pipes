#include <gtest/gtest.h>

#include <pipes2/pipes.hpp>

using namespace tillh::pipes2;

TEST(outputAsIterator_Traits, push_back)
{
  auto result = std::vector<int>();
  auto iterator = push_back(result);
  using type = decltype(iterator);
  static_assert(is_output_iterator_v<type>);
}

TEST(outputAsIterator_Traits, discard)
{
  auto iterator = discard();
  using type = decltype(iterator);
  static_assert(is_output_iterator_v<type>);
}

TEST(outputAsIterator_Traits, override)
{
  auto result = std::vector<int>();
  auto iterator = override(result);
  using type = decltype(iterator);
  static_assert(is_output_iterator_v<type>);
}

TEST(outputAsIterator_Traits, custom)
{
  auto result = std::vector<int>();
  auto iterator = custom([](auto) {});
  using type = decltype(iterator);
  static_assert(is_output_iterator_v<type>);
}

TEST(outputAsIterator_Traits, mapAggregator)
{
  auto result = std::map<int, int>();
  auto iterator = map_aggregator(result, [](auto, auto) {});
  using type = decltype(iterator);
  static_assert(is_output_iterator_v<type>);
}

TEST(outputAsIterator_Traits, closedCombination)
{
  auto result = std::vector<int>();
  auto iterator = transform([](auto i) {return i; }) >>= push_back(result);
  using type = decltype(iterator);
  static_assert(is_output_iterator_v<type>);
}

TEST(outputAsIterator_Traits, failOpen)
{
  auto result = std::vector<int>();
  auto iterator = transform([](auto i) {return i; });
  using type = decltype(iterator);
  static_assert(!is_output_iterator_v<type>);
}

TEST(outputAsIterator_Traits, failOpenCombination)
{
  auto result = std::vector<int>();
  auto iterator = tee(push_back(result));
  using type = decltype(iterator);
  static_assert(!is_output_iterator_v<type>);
}
