#include "header.h"

TEST(Text, IsInitiallyEmpty) {
	gui::text_interface tx(gui::style(0, pixel_32()));
	EXPECT_TRUE(tx.get_str().empty());
}

TEST(Text, WhiteList) {
	gui::text_interface tx(gui::style(0, pixel_32()));
	tx.whitelist = L"1234567890.";
	tx.insert(gui::formatted_text(L"myip:abhgdjshgfjshg2dsadas5dasda5g  dsf s s d .     2or+++5--5.   255.sdadasdasdasd255", gui::style(0, pixel_32())));
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


TEST(Text, WordJumping) {
	gui::word_separator wsep;
	gui::fstr fs = gui::formatted_text(L"Abhjfh ds. fdsf df. dgf .dfg sdfgs.d", gui::style(0, pixel_32()));
	EXPECT_EQ(0, wsep.get_left_word(wstr(fs).c_str(), 0, 0));
	EXPECT_EQ(1, wsep.get_left_word(fs, 0, 1));
	EXPECT_EQ(2, wsep.get_left_word(wstr(fs).c_str(), 0, 2));
	EXPECT_EQ(3, wsep.get_left_word(fs, 0, 3));
	EXPECT_EQ(4, wsep.get_left_word(wstr(fs).c_str(), 0, 4));
	EXPECT_EQ(5, wsep.get_left_word(fs, 0, 5));
	EXPECT_EQ(6, wsep.get_left_word(wstr(fs).c_str(), 0, 6));
	EXPECT_EQ(1, wsep.get_left_word(fs, 0, 7));
	EXPECT_EQ(4, wsep.get_right_word(wstr(fs).c_str(), wstr(fs).length(), 11));
	EXPECT_EQ(1, wsep.get_right_word(fs, wstr(fs).length(), 14));
	EXPECT_EQ(6, wsep.get_right_word(fs, wstr(fs).length(), 0));
}

std::wstring random_wstr(int len) {
	std::wstring rand_wstr;
	rand_wstr.resize(len);
	
	for(int i = 0; i < len; ++i)
		rand_wstr[i] = rand();

	return rand_wstr;
}

TEST(Text, UndoRedo) {
	srand(time(0));
	gui::text_interface tx(gui::style(0, pixel_32()));
	gui::fstr initial_fstr = gui::formatted_text(random_wstr(rand()).c_str(), gui::style(0, pixel_32(212, 100, 0, 192)));
	tx.insert(initial_fstr);

	for(int i = 0; i < 10000; ++i) {
		gui::style rand_style = gui::style(0, pixel_32(rand()%256, rand()%256, rand()%256, rand()%256));
		switch(rand()%6) {
		case 0: tx.insert(gui::formatted_text(random_wstr(rand()%100).c_str(), rand_style)); break;
		case 1: rand()%2 ? tx.caret_left(rand()%20+10, true)  : tx.caret_left_word(true); tx.insert(gui::formatted_text(random_wstr(rand()%50).c_str(), rand_style)); break;
		case 2: rand()%2 ? tx.caret_right(rand()%20+10, true) : tx.caret_right_word(true);  tx.insert(gui::formatted_text(random_wstr(rand()%50).c_str(), rand_style)); break;
		case 3: for(int j = 0, r = rand()%23; j < r; ++j) tx.character(rand()); break;
		case 4: for(int j = 0, r = rand()%28; j < r; ++j) { (rand()%2) ? tx.backspace() : tx.del(); }; break; 
		case 5: rand()&2 ? tx.home(true) : tx.end(true);
		}
	}
	gui::fstr end_fstr = tx.get_str();

	for(int i = 0; i < 100; ++i) {
		while(tx.undo()) {}
		EXPECT_EQ(wstr(tx.get_str()), wstr(initial_fstr));
		while(tx.redo()) {}
		EXPECT_EQ(wstr(tx.get_str()), wstr(end_fstr));
	}
}