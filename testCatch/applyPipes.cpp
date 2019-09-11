#include "catch.hpp"

#include <pipes/pipes.hpp>

#include "testUtils.hpp"

namespace pipes = tillh::pipes::api;

using namespace std::literals;

class applySimplePipes : public testing::Test
{
public:
  const std::vector<int> base = {1,2,3,4,5};
  std::vector<int> result = {};

  using Transform = testUtils::Transform;
  using Filter = testUtils::Filter;
};

TEST_F(applySimplePipes, transform)
{
  auto out = pipes::transform(Transform()) >>= result;
  base >>= out;

  const auto expected = Transform().each(base);
  EXPECT_EQ(result, expected);
}

TEST_F(applySimplePipes, filter)
{
  base >>= pipes::filter(Filter()) >>= result;

  const auto expected = Filter().each(base);
  EXPECT_EQ(result, expected);
}

TEST_F(applySimplePipes, transformFilter)
{
  base >>= pipes::transform(Transform()) >>= pipes::filter(Filter()) >>= result;

  const auto expected = Filter().each(Transform().each(base));
  EXPECT_EQ(result, expected);
}

TEST_F(applySimplePipes, filterTransform)
{
  base >>= pipes::filter(Filter()) >>= pipes::transform(Transform()) >>= result;

  const auto expected = Transform().each(Filter().each(base));
  EXPECT_EQ(result, expected);
}

TEST_F(applySimplePipes, demux1)
{
  base >>= pipes::demux(result);

  const auto expected = base;
  EXPECT_EQ(result, expected);
}

TEST_F(applySimplePipes, demux2)
{
  auto result1 = std::vector<int>{};
  auto result2 = std::vector<int>{};

  base >>= pipes::demux(result1, pipes::transform(Transform()) >>= result2);

  const auto expected1 = base;
  const auto expected2 = Transform().each(base);
  EXPECT_EQ(result1, expected1);
  EXPECT_EQ(result2, expected2);
}

TEST_F(applySimplePipes, tee)
{
  auto result1 = std::vector<int>{};
  auto result2 = std::vector<int>{};

  base >>= pipes::tee(result1) >>= pipes::transform(Transform()) >>= result2;

  const auto expected1 = base;
  const auto expected2 = Transform().each(base);
  EXPECT_EQ(result1, expected1);
  EXPECT_EQ(result2, expected2);
}

TEST_F(applySimplePipes, partition)
{
  auto yes = std::vector<int>{};
  auto no = std::vector<int>{};

  auto pipe = pipes::partition(Filter(), yes, no);
  
  base >>= pipe;

  const auto expectedYes = Filter().each(base);
  const auto expectedNo = Filter().eachNot(base);

  EXPECT_EQ(yes, expectedYes);
  EXPECT_EQ(no, expectedNo);
}

TEST(applyPipes, join)
{
  auto base = std::vector<std::vector<int>>{{1,2}, {3,4}, {5,6}};
  auto expected = std::vector<int>{1,2,3,4,5,6};
  auto result = std::vector<int>{};

  base >>= pipes::join() >>= result;

  EXPECT_EQ(expected, result);
}

TEST(applyPipes, unzip)
{
  const auto base = std::map<int, char>{{1, '1'},{2,'2'},{3,'3'}};
  const auto expectedKeys = std::vector<int>{1,2,3};
  const auto expectedValues = std::vector<char>{'1','2','3'};

  auto keys = std::vector<int>{};
  auto values = std::vector<char>{};
  base >>= pipes::unzip(keys, values);

  EXPECT_EQ(expectedKeys, keys);
  EXPECT_EQ(expectedValues, values);
}

class applySwitch : public testing::Test
{
public:
  std::vector<int> base = {1,2,3,4,5,6};
};

TEST_F(applySwitch, switch_DefaultOnly)
{
  auto result = std::vector<int>{};
  auto defaultCase = pipes::default_(result);

  auto theSwitch = pipes::switch_(defaultCase);

  base >>= theSwitch;

  EXPECT_EQ(result, base);
}

TEST_F(applySwitch, switch_MoreComplex)
{
  auto result1 = std::vector<int>{};
  auto result2 = std::vector<int>{};
  auto resultDefault = std::vector<int>{};

  auto case1 = pipes::case_([](int i) {return i < 3; }, result1);
  auto case2 = pipes::case_([](int i) {return i > 4; }, result2);
  auto defaultCase = pipes::default_(resultDefault);

  auto theSwitch = pipes::switch_(case1, case2, defaultCase);

  base >>= theSwitch;

  auto expected1 = std::vector<int>{1,2};
  auto expected2 = std::vector<int>{5,6};
  auto expectedDefault = std::vector<int>{3,4};

  EXPECT_EQ(expected1, result1);
  EXPECT_EQ(expected2, result2);
  EXPECT_EQ(expectedDefault, resultDefault);
}
