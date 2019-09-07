#include <gtest/gtest.h>

#include <pipes2/pipes.hpp>

namespace pipes = tillh::pipes2;


class moveOnly : public testing::Test
{
public:
  struct uniqueInt
  {
    std::unique_ptr<int> i = nullptr;
  };

  auto makeUnique(const std::vector<int>& is)
  {
    auto retval = std::vector<uniqueInt>{};
    is >>= pipes::transform([](int i) {return uniqueInt{std::make_unique<int>(i)}; }) >>= retval;
    return retval;
  }
};

bool operator==(const moveOnly::uniqueInt& lhs, const moveOnly::uniqueInt& rhs)
{
  return *lhs.i == *rhs.i;
}

std::ostream& operator<<(std::ostream& stream, const moveOnly::uniqueInt& i)
{
  stream << *i.i;
  return stream;
}

TEST_F(moveOnly, transform)
{
  auto base = makeUnique({0,1,2,3,4});
  const auto expected = makeUnique({1,2,3,4,5});

  auto f = [](uniqueInt&& ip) {
    *ip.i += 1;
    return std::move(ip);
  };

  auto result = std::vector<uniqueInt>{};
  std::move(base) >>= pipes::transform(f) >>= result;

  EXPECT_EQ(expected, result);
}

TEST_F(moveOnly, filter)
{
  auto base = makeUnique({0,1,2,3,4});
  const auto expected = makeUnique({0,1,2});

  auto f = [](const uniqueInt& ip) {
    return *(ip.i) < 3;
  };

  auto result = std::vector<uniqueInt>{};
  std::move(base) >>= pipes::filter(f) >>= result;

  EXPECT_EQ(expected, result);
}

TEST_F(moveOnly, partition)
{
  auto base = makeUnique({0,1,2,3,4});
  const auto expected1 = makeUnique({0,1,2});
  const auto expected2 = makeUnique({3,4});

  auto f = [](const uniqueInt& ip) {
    return *(ip.i) < 3;
  };

  auto result1 = std::vector<uniqueInt>{};
  auto result2 = std::vector<uniqueInt>{};
  std::move(base) >>= pipes::partition(f, result1, result2);

  EXPECT_EQ(expected1, result1);
  EXPECT_EQ(expected2, result2);
}

TEST_F(moveOnly, switch_)
{
  auto base = makeUnique({0,1,2,3,4});
  const auto expected1 = makeUnique({0,1,2});
  const auto expected2 = makeUnique({3,4});

  auto f = [](const uniqueInt& ip) {
    return *(ip.i) < 3;
  };

  auto result1 = std::vector<uniqueInt>{};
  auto result2 = std::vector<uniqueInt>{};
  std::move(base) >>= pipes::switch_(pipes::case_(f, result1), pipes::default_(result2));

  EXPECT_EQ(expected1, result1);
  EXPECT_EQ(expected2, result2);
}

TEST_F(moveOnly, override)
{
  auto base = makeUnique({0,1,2,3,4});
  const auto expected = makeUnique({1,2,3,4,5});

  auto f = [](uniqueInt&& ip) {
    *ip.i += 1;
    return std::move(ip);
  };

  auto result = makeUnique({0,0,0,0,0});

  std::move(base) >>= pipes::transform(f) >>= pipes::override(result);

  EXPECT_EQ(expected, result);
}
