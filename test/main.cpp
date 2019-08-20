
#include <gtest/gtest.h>

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);

	std::srand(unsigned(std::time(nullptr))); // use current time as seed for random generator

	int result = RUN_ALL_TESTS();
	return result;
}

