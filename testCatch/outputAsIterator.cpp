#include "catch.hpp"

#include <pipes/pipes.hpp>

namespace pipes = tillh::pipes::api;

TEST(outputAsIterators, push_back)
{
  const auto base = std::vector<int>{1,2,3,4,5};
  auto result = std::vector<int>{};

  std::copy(base.begin(), base.end(), pipes::push_back(result));

  EXPECT_EQ(result, base);
}

TEST(outputAsIterators, override)
{
  const auto base = std::vector<int>{1,2,3,4,5};
  auto result = std::vector<int>{0,0,0,0,0};

  std::copy(base.begin(), base.end(), pipes::override(result));

  EXPECT_EQ(result, base);
}

TEST(outputAsIterators, custom)
{
  const auto base = std::vector<int>{1,2,3,4,5};
  auto result = std::vector<int>{};

  std::copy(base.begin(), base.end(), pipes::custom([&result](auto i) {result.push_back(i); }));

  EXPECT_EQ(result, base);
}

TEST(outputAsIterators, combination)
{
  const auto base = std::vector<int>{1,2,3,4,5};
  auto result = std::vector<int>{};

  std::copy(base.begin(), base.end(), pipes::filter([](auto i) {return i < 4; }) >>= pipes::push_back(result));

  auto expected = std::vector{1,2,3};
  EXPECT_EQ(result, expected);
}