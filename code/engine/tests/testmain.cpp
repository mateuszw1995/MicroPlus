#include "../include/misc/container/circular_buffer.h"
#include <gtest\gtest.h>

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);

	auto result = RUN_ALL_TESTS();
	std::cin >> result;
	return result;
}
