#include <gtest/gtest.h>

#include <pipes2/pipes.hpp>

using namespace tillh::pipes2;

TEST(pipeTraits, transform)
{
  const auto trans = transform([](auto i) {return i + 1; });
  using type = remove_cv_ref_t<decltype(trans)>;
  static_assert(canPrimaryConnect(Type<type>()));
  static_assert(is_node_v<type>);
  static_assert(!is_range_v<type>);
  static_assert(!detail::is_detected_v<begin_expression, type>);
  static_assert(firstOpen(Type<type>()) == 0);
}

TEST(pipeTraits, filter)
{
  const auto filte = filter([](auto i) {return (i % 2) == 0; });
  using type = remove_cv_ref_t<decltype(filte)>;
  static_assert(canPrimaryConnect(Type<type>()));
  static_assert(is_node_v<type>);
  static_assert(!is_range_v<type>);
  static_assert(!detail::is_detected_v<begin_expression, type>);
  static_assert(firstOpen(Type<type>()) == 0);
}

TEST(pipeTraits, demux1)
{
  auto target = std::vector<int>{};
  const auto demu = demux(target);
  using type = remove_cv_ref_t<decltype(demu)>;
  static_assert(is_node_v<type>);
  static_assert(is_output_v<type>);
  static_assert(!is_range_v<type>);
}

TEST(pipeTraits, demux2)
{
  auto target1 = std::vector<int>{};
  auto target2 = std::vector<int>{};
  const auto demu = demux(target1, target2);
  using type = remove_cv_ref_t<decltype(demu)>;
  static_assert(is_node_v<type>);
  static_assert(is_output_v<type>);
  static_assert(!is_range_v<type>);
}

class applyPipes : public testing::Test
{
public:

  struct Transform
  {
    int operator()(int i) const { return i + 1; }
    std::vector<int> each(std::vector<int> src)
    {
      auto ret = std::vector<int>();
      std::transform(src.begin(), src.end(), std::back_inserter(ret), *this);
      return ret;
    }
  };
  struct Filter
  {
    bool operator()(int i) const { return (i % 2) != 0; }
    std::vector<int> each(const std::vector<int>& src)
    {
      auto ret = std::vector<int>();
      std::copy_if(src.begin(), src.end(), std::back_inserter(ret), *this);
      return ret;
    }    
    std::vector<int> eachNot(const std::vector<int>& src)
    {
      auto ret = std::vector<int>();
      const auto notThis = [this](auto i) {return !(*this)(i); };
      std::copy_if(src.begin(), src.end(), std::back_inserter(ret), notThis);
      return ret;
    }
  };
};

class applySimplePipes : public applyPipes
{
public:
  const std::vector<int> base = {1,2,3,4,5};
  std::vector<int> result = {};
};

TEST_F(applySimplePipes, transform)
{
  auto out = transform(Transform()) >>= result;
  base >>= out;

  const auto expected = Transform().each(base);
  EXPECT_EQ(result, expected);
}

TEST_F(applySimplePipes, filter)
{
  base >>= filter(Filter()) >>= result;

  const auto expected = Filter().each(base);
  EXPECT_EQ(result, expected);
}

TEST_F(applySimplePipes, transformFilter)
{
  base >>= transform(Transform()) >>= filter(Filter()) >>= result;

  const auto expected = Filter().each(Transform().each(base));
  EXPECT_EQ(result, expected);
}

TEST_F(applySimplePipes, filterTransform)
{
  base >>= filter(Filter()) >>= transform(Transform()) >>= result;

  const auto expected = Transform().each(Filter().each(base));
  EXPECT_EQ(result, expected);
}

TEST_F(applySimplePipes, demux1)
{
  base >>= demux(result);

  const auto expected = base;
  EXPECT_EQ(result, expected);
}

TEST_F(applySimplePipes, demux2)
{
  auto result1 = std::vector<int>{};
  auto result2 = std::vector<int>{};

  base >>= demux(result1, transform(Transform()) >>= result2);

  const auto expected1 = base;
  const auto expected2 = Transform().each(base);
  EXPECT_EQ(result1, expected1);
  EXPECT_EQ(result2, expected2);
}

TEST_F(applySimplePipes, tee)
{
  auto result1 = std::vector<int>{};
  auto result2 = std::vector<int>{};

  base >>= tee(result1) >>= transform(Transform()) >>= result2;

  const auto expected1 = base;
  const auto expected2 = Transform().each(base);
  EXPECT_EQ(result1, expected1);
  EXPECT_EQ(result2, expected2);
}

TEST_F(applySimplePipes, partition)
{
  auto yes = std::vector<int>{};
  auto no = std::vector<int>{};

  auto pipe = partition(Filter(), yes, no);

  constexpr auto t = Type<decltype(pipe)>();
  static_assert(!canPrimaryConnect(t));
  static_assert(!canSecondaryConnect(t));

  base >>= pipe;

  const auto expectedYes = Filter().each(base);
  const auto expectedNo = Filter().eachNot(base);

  EXPECT_EQ(yes, expectedYes);
  EXPECT_EQ(no, expectedNo);
}

TEST_F(applyPipes, join)
{
  auto base = std::vector<std::vector<int>>{{1,2}, {3,4}, {5,6}};
  auto expected = std::vector<int>{1,2,3,4,5,6};
  auto result = std::vector<int>{};

  base >>= join() >>= result;

  EXPECT_EQ(expected, result);
}

TEST_F(applyPipes, unzip)
{
  const auto base = std::map<int, char>{{1, '1'},{2,'2'},{3,'3'}};
  const auto expectedKeys = std::vector<int>{1,2,3};
  const auto expectedValues = std::vector<char>{'1','2','3'};

  auto keys = std::vector<int>{};
  auto values = std::vector<char>{};
  base >>= unzip(keys, values);

  EXPECT_EQ(expectedKeys, keys);
  EXPECT_EQ(expectedValues , values);
}

class applySwitch : public testing::Test
{
public:
  std::vector<int> base = {1,2,3,4,5,6};
};

TEST_F(applySwitch, switch_DefaultOnly)
{
  auto result = std::vector<int>{};
  auto defaultCase = default_(result);

  auto theSwitch = switch_(defaultCase);

  base >>= theSwitch;

  EXPECT_EQ(result, base);
}

TEST_F(applySwitch, switch_MoreComplex)
{
  auto result1 = std::vector<int>{};
  auto result2 = std::vector<int>{};
  auto resultDefault = std::vector<int>{};

  auto case1 = case_([](int i) {return i < 3; }, result1);
  auto case2 = case_([](int i) {return i > 4; }, result2);
  auto defaultCase = default_(resultDefault);

  auto theSwitch = switch_(case1, case2, defaultCase);

  base >>= theSwitch;

  auto expected1 = std::vector<int>{1,2};
  auto expected2 = std::vector<int>{5,6};
  auto expectedDefault = std::vector<int>{3,4};

  EXPECT_EQ(expected1, result1);
  EXPECT_EQ(expected2, result2);
  EXPECT_EQ(expectedDefault, resultDefault);
}

TEST(endPipes, push_back)
{
  const auto base = std::vector<int>{1,2,3,4,5};
  auto result = std::vector<int>{};

  base >>= push_back(result);

  EXPECT_EQ(result, base);
}

TEST(endPipes, override)
{
  const auto base = std::vector<int>{1,2,3,4,5};
  auto result = std::vector<int>{0,0,0,0,0};

  base >>= override(result);

  EXPECT_EQ(result, base);
}

TEST(endPipes, overridePartial)
{
  const auto base = std::vector<int>{1,2,3};
  auto result = std::vector<int>{0,0,0,0,0};
  const auto expected = std::vector<int>{1,2,3,0,0};

  base >>= override(result);

  EXPECT_EQ(result, expected);
}

TEST(endPipes, custom)
{
  const auto base = std::vector<int>{1,2,3,4,5};
  auto result = std::vector<int>{};

  base >>= custom([&result](int i) {result.push_back(i); });

  EXPECT_EQ(result, base);
}

TEST(endPipes, mapAggregator)
{
  const auto entries1 = std::vector<std::pair<int, std::string>>{{1, "a"}, {2, "b"}, {3, "c"}, {4, "d"}};
  const auto entries2 = std::vector<std::pair<int, std::string>>{{2, "b"}, {3, "c"}, {4, "d"}, {5, "e"}};

  const auto expected = std::map<int, std::string>{{1, "a"}, {2, "bb"}, {3, "cc"}, {4, "dd"}, {5, "e"}};

  auto concatenateStrings = [](const std::string& lhs, const std::string& rhs) { return lhs + rhs; };

  auto results = std::map<int, std::string>{};

  entries1 >>= map_aggregator(results, concatenateStrings);
  entries2 >>= map_aggregator(results, concatenateStrings);

  EXPECT_EQ(results, expected);
}

TEST(endPipes, iteratorBackInserter)
{
  const auto base = std::vector<int>{1,2,3,4,5};
  auto result = std::vector<int>{};

  base >>= iterator(std::back_inserter(result));

  EXPECT_EQ(result, base);
}

TEST(endPipes, iteratorBegin)
{
  const auto base = std::vector<int>{1,2,3,4,5};
  auto result = std::vector<int>{0,0,0,0,0};

  base >>= iterator(std::begin(result));

  EXPECT_EQ(result, base);
}

TEST(endPipes, pipeToBackInserter)
{
  const auto base = std::vector<int>{1,2,3,4,5};
  auto result = std::vector<int>{};

  base >>= filter([](auto) {return true; }) >>= std::back_inserter(result);

  EXPECT_EQ(result, base);
}

TEST(endPipes, pipeToBegin)
{
  const auto base = std::vector<int>{1,2,3,4,5};
  auto result = std::vector<int>{0,0,0,0,0};

  base >>= filter([](auto) {return true; }) >>= std::begin(result);

  EXPECT_EQ(result, base);
}
