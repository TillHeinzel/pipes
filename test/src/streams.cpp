#include <gtest/gtest.h>

#include <string>

#include <pipes2/pipes.hpp>

namespace pipes = tillh::pipes2;

TEST(streams, instream)
{
  const auto in = std::string{"test1 test2 test3"};
  const auto expected = std::vector<std::string>{"TEST1", "TEST2", "TEST3"};

  auto toUpper = [](const std::string& s)
  {
    std::string result(s.size(), '_');
    std::transform(begin(s), end(s), begin(result), [](char c) { return std::toupper(c); });
    return result;
  };

  auto result = std::vector<std::string>{};
  std::istringstream(in) >>= pipes::transform(toUpper) >>= result;

  EXPECT_EQ(expected, result);
}

TEST(streams, typedInStream)
{
  const auto in = std::string{"0 1 2 3 4"};
  const auto expected = std::vector<int>{1,2,3,4,5};

  auto f = [](int i) { return i + 1; };

  auto result = std::vector<int>{};
  pipes::fromStream<int>(std::istringstream(in)) >>= pipes::transform(f) >>= result;

  EXPECT_EQ(expected, result);
}

TEST(streams, outStream)
{
  const auto in = std::vector{0,1,2,3,4};
  const auto expected = std::string{"12345"};

  auto f = [](int i) { return i + 1; };

  auto result = std::ostringstream();
  in >>= pipes::transform(f) >>= result;

  EXPECT_EQ(expected, std::string(result.str()));
}

TEST(streams, toStream)
{
  const auto in = std::vector{0,1,2,3,4};
  const auto expected = std::string{"12345"};

  auto f = [](int i) { return i + 1; };

  auto result = std::ostringstream();
  in >>= pipes::transform(f) >>= pipes::toStream(result);

  EXPECT_EQ(expected, std::string(result.str()));
}

TEST(streams, cout)
{
  const auto in = std::vector{0,1,2,3,4};
  const auto expected = std::string{"12345"};

  auto f = [](int i) { return i + 1; };

  auto result = std::ostringstream();
  in >>= pipes::transform(f) >>= pipes::tee(std::cout) >>= pipes::toStream(result);
  std::cout << std::endl;

  EXPECT_EQ(expected, std::string(result.str()));
}
