#include "catch.hpp"

#include <pipes/pipes.hpp>

namespace pipes = tillh::pipes::api;

TEST(endPipes, push_back)
{
  const auto base = std::vector<int>{1,2,3,4,5};
  auto result = std::vector<int>{};

  base >>= pipes::push_back(result);

  EXPECT_EQ(result, base);
}

TEST(endPipes, override)
{
  const auto base = std::vector<int>{1,2,3,4,5};
  auto result = std::vector<int>{0,0,0,0,0};

  base >>= pipes::override(result);

  EXPECT_EQ(result, base);
}

TEST(endPipes, overridePartial)
{
  const auto base = std::vector<int>{1,2,3};
  auto result = std::vector<int>{0,0,0,0,0};
  const auto expected = std::vector<int>{1,2,3,0,0};

  base >>= pipes::override(result);

  EXPECT_EQ(result, expected);
}

TEST(endPipes, custom)
{
  const auto base = std::vector<int>{1,2,3,4,5};
  auto result = std::vector<int>{};

  base >>= pipes::custom([&result](int i) {result.push_back(i); });

  EXPECT_EQ(result, base);
}

TEST(endPipes, mapAggregator)
{
  const auto entries1 = std::vector<std::pair<int, std::string>>{{1, "a"}, {2, "b"}, {3, "c"}, {4, "d"}};
  const auto entries2 = std::vector<std::pair<int, std::string>>{{2, "b"}, {3, "c"}, {4, "d"}, {5, "e"}};

  const auto expected = std::map<int, std::string>{{1, "a"}, {2, "bb"}, {3, "cc"}, {4, "dd"}, {5, "e"}};

  auto concatenateStrings = [](const std::string& lhs, const std::string& rhs) { return lhs + rhs; };

  auto results = std::map<int, std::string>{};

  entries1 >>= pipes::map_aggregator(results, concatenateStrings);
  entries2 >>= pipes::map_aggregator(results, concatenateStrings);

  EXPECT_EQ(results, expected);
}

TEST(endPipes, iteratorBackInserter)
{
  const auto base = std::vector<int>{1,2,3,4,5};
  auto result = std::vector<int>{};

  base >>= pipes::iterator(std::back_inserter(result));

  EXPECT_EQ(result, base);
}

TEST(endPipes, iteratorBegin)
{
  const auto base = std::vector<int>{1,2,3,4,5};
  auto result = std::vector<int>{0,0,0,0,0};

  base >>= pipes::iterator(std::begin(result));

  EXPECT_EQ(result, base);
}

TEST(endPipes, pipeToBackInserter)
{
  const auto base = std::vector<int>{1,2,3,4,5};
  auto result = std::vector<int>{};

  base >>= pipes::filter([](auto) {return true; }) >>= std::back_inserter(result);

  EXPECT_EQ(result, base);
}

TEST(endPipes, pipeToBegin)
{
  const auto base = std::vector<int>{1,2,3,4,5};
  auto result = std::vector<int>{0,0,0,0,0};

  base >>= pipes::filter([](auto) {return true; }) >>= std::begin(result);

  EXPECT_EQ(result, base);
}
