#include "../include/db.h"
#include <gtest\gtest.h>

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);

	int aa;
	auto result = RUN_ALL_TESTS();
	std::cin >> aa;
	return result;
}
