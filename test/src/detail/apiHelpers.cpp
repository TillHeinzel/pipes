#include <gtest/gtest.h>

#include <pipes/pipes.hpp>

using namespace tillh::pipes;

TEST(apiHelpers, canSendVector)
{
  static_assert(!detail::sends<std::vector<int>>);
  static_assert(detail::canSend<std::vector<int>>);
}