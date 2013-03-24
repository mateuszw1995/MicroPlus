#include "header.h"

TEST(Text, IsInitiallyEmpty) {
	gui::text_interface tx(gui::text_printer::style(0, pixel_32()));
	EXPECT_TRUE(tx.get_str().empty());
}

TEST(Text, WhiteList) {
	gui::text_interface tx(gui::text_printer::style(0, pixel_32()));
	tx.whitelist = L"1234567890.";
	tx.insert(gui::formatted_text(L"myip:abhgdjshgfjshg2dsadas5dasda5g  dsf s s d .     2or+++5--5.   255.sdadasdasdasd255"));
	std::wstring myip(L"255.255.255.255"), 
		otherstr(L" returns an iterator just past this element"),
		badstr(L" 43253r534e5.,/.,/?><?tu213r4ns56 a56n456 7it//////////erator /42;][];['];[]just 123132131\\|||p|\"|\"|\"|:\"as4234t 55this element");
	EXPECT_EQ(myip, wstr(tx.get_str()));
	tx.whitelist = L"ABCDEFGHIJKLMNOPRS TUVWXYZabcdefghijklmnoprstuvwxyz";
	tx.select_all();
	for(size_t i = 0; i < badstr.size(); ++i)
		tx.character(badstr[i]);
	EXPECT_EQ(otherstr, wstr(tx.get_str()));
}