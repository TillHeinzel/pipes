#include "catch.hpp"

#include <pipes/pipes.hpp>

#include "testUtils.hpp"

namespace pipes = tillh::pipes::api;

class placeholders : public testing::Test
{
public:

  using Transform = testUtils::Transform;
  using Filter = testUtils::Filter;
};

TEST_F(placeholders, tee)
{
  const std::vector<int> base = {1,2,3,4,5};
  auto result1 = std::vector<int>{};
  auto result2 = std::vector<int>{};

  using namespace pipes; // for pipes::_
  auto halfPipe = tee(_) >>= transform(Transform()) >>= result2;

  base >>= halfPipe(result1);

  const auto expected1 = base;
  const auto expected2 = Transform().each(base);
  EXPECT_EQ(result1, expected1);
  EXPECT_EQ(result2, expected2);
}

TEST_F(placeholders, demux)
{
  const std::vector<int> base = {1,2,3,4,5};
  auto result1 = std::vector<int>{};
  auto result2 = std::vector<int>{};
  auto result3 = std::vector<int>{};

  using namespace pipes; // for pipes::_
  auto halfPipe = demux(_, transform(Transform()) >>= result2, _);

  base >>= halfPipe(result1, filter(Filter()) >>= result3);

  const auto expected1 = base;
  const auto expected2 = Transform().each(base);
  const auto expected3 = Filter().each(base);
  EXPECT_EQ(result1, expected1);
  EXPECT_EQ(result2, expected2);
  EXPECT_EQ(result3, expected3);
}

TEST_F(placeholders, primaryOpen)
{
  const std::vector<int> base = {1,2,3,4,5};
  auto result1 = std::vector<int>{};
  auto result2 = std::vector<int>{};

  using namespace pipes; // for pipes::_
  auto halfPipe = demux(filter(Filter()), transform(Transform()));
  auto halfPipe2 = halfPipe(result1);
  auto fullPipe = halfPipe2(result2);

  base >>= fullPipe;

  const auto expected1 = Filter().each(base);
  const auto expected2 = Transform().each(base);
  EXPECT_EQ(result1, expected1);
  EXPECT_EQ(result2, expected2);
}

TEST_F(placeholders, primaryConnectWithOpenSecondaries)
{
  const std::vector<int> base = {1,2,3,4,5};
  auto result1 = std::vector<int>{};
  auto result2 = std::vector<int>{};

  using namespace pipes; // for pipes::_
  auto halfPipe = tee(_) >>= demux(_);
  auto halfPipe2 = halfPipe(filter(Filter()) >>= result1);
  auto fullPipe = halfPipe2(transform(Transform()) >>= result2);

  base >>= fullPipe;

  const auto expected1 = Filter().each(base);
  const auto expected2 = Transform().each(base);
  EXPECT_EQ(result1, expected1);
  EXPECT_EQ(result2, expected2);
}

TEST_F(placeholders, primaryConnectWithPrimaryOpenInSecondaries)
{
  const std::vector<int> base = {1,2,3,4,5};
  auto result1 = std::vector<int>{};
  auto result2 = std::vector<int>{};

  using namespace pipes; // for pipes::_
  auto halfPipe = tee(filter(Filter())) >>= demux(transform(Transform()));
  auto halfPipe2 = halfPipe(result1);
  auto fullPipe = halfPipe2(result2);

  base >>= fullPipe;

  const auto expected1 = Filter().each(base);
  const auto expected2 = Transform().each(base);
  EXPECT_EQ(result1, expected1);
  EXPECT_EQ(result2, expected2);
}
