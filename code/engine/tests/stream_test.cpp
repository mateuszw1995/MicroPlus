#include "header.h" 

TEST(Stream, StandardOperations) {
	EXPECT_EQ(12345678,  num<int>(std::string  ("12345678")));
	EXPECT_EQ(12345678, wnum<int>(std::wstring(L"12345678")));
	EXPECT_STREQ("12345678", misc::str(12345678).c_str());
	EXPECT_EQ(std::wstring(L"12345678"), wstr(12345678));
}

TEST(Stream, OverloadedOperations) {
	gui::formatted_char abc;
	abc.r = 24;
	abc.c = L'Z';

	gui::fstr fs;
	fs.resize(13, abc);

	EXPECT_EQ(std::wstring(13, L'Z'), wstr(fs));
	EXPECT_STREQ("[453535,86794]", str(point(453535, 86794)).c_str());
}