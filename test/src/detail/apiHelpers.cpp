#include <gtest/gtest.h>

#include <pipes2/pipes.hpp>

using namespace tillh::pipes2;

TEST(apiHelpers, canSendVector)
{
  static_assert(!detail::sends<std::vector<int>>);
  static_assert(detail::canSend<std::vector<int>>);
}