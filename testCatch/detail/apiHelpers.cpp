#include "catch.hpp"

#include <pipes/pipes.hpp>

using namespace tillh::pipes;

TEST(apiHelpers, canSendVector)
{
  static_assert(!sends<std::vector<int>>);
  static_assert(canSend<std::vector<int>>);
}