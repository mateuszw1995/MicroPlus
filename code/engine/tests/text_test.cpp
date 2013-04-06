#include "header.h"

using namespace gui;
using namespace controls;

std::wstring random_wstr(int len) {
	std::wstring rand_wstr;
	rand_wstr.resize(len);
	
	for(int i = 0; i < len; ++i)
		rand_wstr[i] = rand();

	return rand_wstr;
}

TEST(Text, IsInitiallyEmpty) {
	textbox tx(rect(), style(0, pixel_32()));
	EXPECT_TRUE(tx.get_str().empty());
}

TEST(Text, WhiteList) {
	font f;
	textbox tx(rect(), style(&f, pixel_32()));
	tx.whitelist = L"1234567890.";
	tx.insert(formatted_text(L"myip:abhgdjshgfjshg2dsadas5dasda5g  dsf s s d .     2or+++5--5.   255.sdadasdasdasd255", style(&f, pixel_32())));
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
	word_separator wsep;
	font f;
	fstr fs = formatted_text(L"Abhjfh ds. fdsf df. dgf .dfg sdfgs.d", style(&f, pixel_32()));
	EXPECT_EQ(0, wsep.get_left_word(fs, 0));
	EXPECT_EQ(1, wsep.get_left_word(fs, 1));
	EXPECT_EQ(2, wsep.get_left_word(fs, 2));
	EXPECT_EQ(3, wsep.get_left_word(fs, 3));
	EXPECT_EQ(4, wsep.get_left_word(fs, 4));
	EXPECT_EQ(5, wsep.get_left_word(fs, 5));
	EXPECT_EQ(6, wsep.get_left_word(fs, 6));
	EXPECT_EQ(1, wsep.get_left_word(fs, 7));
	EXPECT_EQ(4, wsep.get_right_word(fs, 11));
	EXPECT_EQ(1, wsep.get_right_word(fs, 14));
	EXPECT_EQ(6, wsep.get_right_word(fs, 0));
	EXPECT_EQ(0, wsep.get_right_word(fs, fs.length()));
	EXPECT_EQ(1, wsep.get_right_word(fs, fs.length()-1));
	EXPECT_EQ(1, wsep.get_right_word(fs, fs.length()-2));
}


TEST(Text, UndoRedo) {
	font f;
	font_file ff;
	ff.create(make_pair(0, 10));
	f.build(&ff);
	textbox tx(rect(), style(&f, pixel_32()));
	
	srand(time(0));
	fstr initial_fstr = formatted_text(random_wstr(rand()).c_str(), style(&f, pixel_32(212, 100, 0, 192)));
	tx.insert(initial_fstr);

	for(int i = 0; i < 10; ++i) {
		style rand_style = style(&f, pixel_32(rand()%256, rand()%256, rand()%256, rand()%256));
		switch(rand()%6) {
		case 0: tx.insert(formatted_text(random_wstr(rand()%100).c_str(), rand_style)); break;
		case 1: rand()%2 ? tx.caret_left(rand()%20+10, true)  : tx.caret_left_word(true); tx.insert(formatted_text(random_wstr(rand()%50).c_str(), rand_style)); break;
		case 2: rand()%2 ? tx.caret_right(rand()%20+10, true) : tx.caret_right_word(true);  tx.insert(formatted_text(random_wstr(rand()%50).c_str(), rand_style)); break;
		case 3: for(int j = 0, r = rand()%23; j < r; ++j) tx.character(rand()); break;
		case 4: for(int j = 0, r = rand()%28; j < r; ++j) { (rand()%2) ? tx.backspace() : tx.del(); }; break; 
		case 5: rand()&2 ? tx.home(true) : tx.end(true);
		}
	}
	fstr end_fstr = tx.get_str();

	for(int i = 0; i < 2; ++i) {
		while(tx.undo()) {}
		EXPECT_EQ(wstring(L""), wstr(tx.get_str()));
		while(tx.redo()) {}
		EXPECT_EQ(wstr(end_fstr), wstr(tx.get_str()));
	}
}

TEST(Text, BoundsCheck) {
	font f;
	font_file ff;
	ff.create(make_pair(0, 10));
	f.build(&ff);
	textbox tx(rect(), style(&f, pixel_32()));
	
	EXPECT_EQ(wstring(L""), wstr(tx.get_str()));
	tx.select_all();
	tx.backspace();
	EXPECT_EQ(wstring(L""), wstr(tx.get_str()));
	tx.select_all();
	tx.del();
	EXPECT_EQ(wstring(L""), wstr(tx.get_str()));
	tx.select_all();
	tx.character(L'L');
	EXPECT_EQ(wstring(L"L"), wstr(tx.get_str()));
	tx.select_all();
	tx.backspace();
	EXPECT_EQ(wstring(L""), wstr(tx.get_str()));
	
	tx.select_word(0);
	tx.backspace();
	EXPECT_EQ(wstring(L""), wstr(tx.get_str()));
	tx.select_word(0);
	tx.del();
	EXPECT_EQ(wstring(L""), wstr(tx.get_str()));
	tx.select_word(0);
	tx.character(L'L');
	EXPECT_EQ(wstring(L"L"), wstr(tx.get_str()));
	tx.select_word(0);
	tx.backspace();
	EXPECT_EQ(wstring(L""), wstr(tx.get_str()));

	
	tx.select_line(0);
	tx.backspace();
	EXPECT_EQ(wstring(L""), wstr(tx.get_str()));
	tx.select_line(0);
	tx.del();
	EXPECT_EQ(wstring(L""), wstr(tx.get_str()));
	tx.select_line(0);
	tx.character(L'L');
	EXPECT_EQ(wstring(L"L"), wstr(tx.get_str()));
	tx.select_line(0);
	tx.backspace();
	EXPECT_EQ(wstring(L""), wstr(tx.get_str()));

	tx.backspace();
	tx.select_all();
	EXPECT_EQ(wstring(L""), wstr(tx.get_str()));
	tx.del();
	EXPECT_EQ(wstring(L""), wstr(tx.get_str()));
	tx.del();
	tx.caret_left(4234, false);
	EXPECT_EQ(wstring(L""), wstr(tx.get_str()));
	tx.caret_right(321, true);
	tx.character(L'L');
	EXPECT_EQ(wstring(L"L"), wstr(tx.get_str()));
	tx.backspace();
	EXPECT_EQ(wstring(L""), wstr(tx.get_str()));
	tx.del();
	EXPECT_EQ(wstring(L""), wstr(tx.get_str()));
	tx.backspace();
	tx.character(L'L');
	EXPECT_EQ(wstring(L"L"), wstr(tx.get_str()));
	tx.del();
	EXPECT_EQ(wstring(L"L"), wstr(tx.get_str()));
	tx.del();
	tx.del();
	tx.character(L'A');
	EXPECT_EQ(wstring(L"LA"), wstr(tx.get_str()));
	tx.backspace();
	EXPECT_EQ(wstring(L"L"), wstr(tx.get_str()));
	tx.del();
	tx.del();
	tx.backspace();
	tx.character(L'L');
	tx.backspace();
	EXPECT_EQ(wstring(L""), wstr(tx.get_str()));
	tx.backspace();
	tx.backspace();
	tx.insert(formatted_text(L"ABCDEFGHABCDEFGHABCDEFGH", style(&f, pixel_32())));
	while(tx.undo());
	EXPECT_EQ(wstring(L""), wstr(tx.get_str()));
	while(tx.redo());
	EXPECT_EQ(wstring(L"ABCDEFGHABCDEFGHABCDEFGH"), wstr(tx.get_str()));
	tx.caret_right(321, true);
	tx.caret_right_word(true);
	tx.caret_left(2, false);
	tx.caret_right(23, true);
	tx.del();
	EXPECT_EQ(wstring(L"ABCDEFGHABCDEFGHABCDEF"), wstr(tx.get_str()));
	tx.end(true);
	tx.caret_left(3, false);
	tx.home(true);
	tx.home(true);
	tx.home(true);
	tx.insert(formatted_text(L"abc", style(&f, pixel_32())));
	EXPECT_EQ(wstring(L"abcDEF"), wstr(tx.get_str()));
}