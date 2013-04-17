#include "header.h"


std::wstring random_wstr(int len) {
	std::wstring rand_wstr;
	rand_wstr.resize(len);
	
	for(int i = 0; i < len; ++i)
		rand_wstr[i] = rand();

	return rand_wstr;
}

TEST(Text, IsInitiallyEmpty) {
	textbox tx(rect(), style(0, pixel_32()));
	EXPECT_TRUE(tx.editor.get_str().empty());
}

TEST(Text, WhiteList) {
	font f;
	textbox tx(rect(), style(&f, pixel_32()));

	auto& e = tx.editor;
	e.whitelist = L"1234567890.";
	e.insert(format(L"myip:abhgdjshgfjshg2dsadas5dasda5g  dsf s s d .     2or+++5--5.   255.sdadasdasdasd255", style(&f, pixel_32())));
	std::wstring myip(L"255.255.255.255"), 
		otherstr(L" returns an iterator just past this element"),
		badstr(L" 43253r534e5.,/.,/?><?tu213r4ns56 a56n456 7it//////////erator /42;][];['];[]just 123132131\\|||p|\"|\"|\"|:\"as4234t 55this element");
	EXPECT_EQ(myip, wstr(e.get_str()));
	e.whitelist = L"ABCDEFGHIJKLMNOPRS TUVWXYZabcdefghijklmnoprstuvwxyz";
	e.select_all();
	for(size_t i = 0; i < badstr.size(); ++i)
		e.character(badstr[i]);
	EXPECT_EQ(otherstr, wstr(e.get_str()));
}


TEST(Text, WordJumping) {
	word_separator wsep;
	font f;
	fstr fs = format(L"Abhjfh ds. fdsf df. dgf .dfg sdfgs.d", style(&f, pixel_32()));
	EXPECT_EQ(0, wsep.get_left_word(fs, 0));
	EXPECT_EQ(1, wsep.get_left_word(fs, 1));
	EXPECT_EQ(2, wsep.get_left_word(fs, 2));
	EXPECT_EQ(3, wsep.get_left_word(fs, 3));
	EXPECT_EQ(4, wsep.get_left_word(fs, 4));
	EXPECT_EQ(5, wsep.get_left_word(fs, 5));
	EXPECT_EQ(6, wsep.get_left_word(fs, 6));
	EXPECT_EQ(1, wsep.get_left_word(fs, 7));
	EXPECT_EQ(1, wsep.get_left_word(fs, 11));
	EXPECT_EQ(4, wsep.get_right_word(fs, 11));
	EXPECT_EQ(1, wsep.get_right_word(fs, 14));
	EXPECT_EQ(6, wsep.get_right_word(fs, 0));
	EXPECT_EQ(0, wsep.get_right_word(fs, fs.length()));
	EXPECT_EQ(1, wsep.get_right_word(fs, fs.length()-1));
	EXPECT_EQ(1, wsep.get_right_word(fs, fs.length()-2));
	fs = format(L"Abhjf\nh\n ds.\n fdsf df. dgf .dfg sdfgs.d", style(&f, pixel_32()));
}

TEST(Text, WordSelection) {
	font f;
	font_file ff;
	ff.create(make_pair(0, 10));
	f.build(&ff);
	textbox tx(rect(), style(&f, pixel_32()));
	fstr fs = format(L"Abhjfh ds..\n fdsf df.\n\n\n dgf .dfg sdfgs.d", style(&f, pixel_32()));
	
	auto& e = tx.editor;
	e.insert(fs);
	e.select_word(0);
	EXPECT_EQ(0, e.get_left_selection());
	EXPECT_EQ(6, e.get_right_selection());
	EXPECT_EQ(6, e.get_caret_pos());
	EXPECT_TRUE(iswalnum(L'h'));
	EXPECT_TRUE(iswalnum(e.get_str()[2].c));
	EXPECT_FALSE(iswspace(L'h'));
	EXPECT_FALSE(iswspace(e.get_str()[2].c));
	e.select_word(2);
	EXPECT_EQ(0, e.get_left_selection());
	EXPECT_EQ(6, e.get_right_selection());
	EXPECT_EQ(6, e.get_caret_pos());
	e.select_word(5);
	EXPECT_EQ(0, e.get_left_selection());
	EXPECT_EQ(6, e.get_right_selection());
	EXPECT_EQ(6, e.get_caret_pos());
	e.select_word(6);
	EXPECT_EQ(6, e.get_left_selection());
	EXPECT_EQ(7, e.get_right_selection());
	EXPECT_EQ(7, e.get_caret_pos());
	e.select_word(7);
	EXPECT_EQ(7, e.get_left_selection());
	EXPECT_EQ(9, e.get_right_selection());
	EXPECT_EQ(9, e.get_caret_pos());
	e.select_word(9);
	EXPECT_EQ(9, e.get_left_selection());
	EXPECT_EQ(11, e.get_right_selection());
	EXPECT_EQ(11, e.get_caret_pos());
	e.select_word(11);
	EXPECT_EQ(9, e.get_left_selection());
	EXPECT_EQ(11, e.get_right_selection());
	EXPECT_EQ(11, e.get_caret_pos());
	e.select_word(12);
	EXPECT_EQ(12, e.get_left_selection());
	EXPECT_EQ(13, e.get_right_selection());
	EXPECT_EQ(13, e.get_caret_pos());
	e.select_word(22);
	EXPECT_EQ(21, e.get_left_selection());
	EXPECT_EQ(22, e.get_right_selection());
	EXPECT_EQ(22, e.get_caret_pos());
	e.select_line(22);
	EXPECT_EQ(22, e.get_left_selection());
	EXPECT_EQ(23, e.get_right_selection());
	EXPECT_EQ(22, e.get_caret_pos());
}

TEST(Text, UndoRedo) {
	font f;
	font_file ff;
	ff.create(make_pair(0, 10));
	f.build(&ff);
	textbox tx(rect(), style(&f, pixel_32()));
	
	srand(time(0));
	fstr initial_fstr = format(random_wstr(rand()).c_str(), style(&f, pixel_32(212, 100, 0, 192)));
	auto& e = tx.editor;
	e.insert(initial_fstr);

	for(int i = 0; i < 10; ++i) {
		style rand_style = style(&f, pixel_32(rand()%256, rand()%256, rand()%256, rand()%256));
		switch(rand()%6) {
		case 0: e.insert(format(random_wstr(rand()%100).c_str(), rand_style)); break;
		case 1: rand()%2 ? e.caret_left(rand()%20+10, true)  : e.caret_left_word(true); e.insert(format(random_wstr(rand()%50).c_str(), rand_style)); break;
		case 2: rand()%2 ? e.caret_right(rand()%20+10, true) : e.caret_right_word(true);  e.insert(format(random_wstr(rand()%50).c_str(), rand_style)); break;
		case 3: for(int j = 0, r = rand()%23; j < r; ++j) e.character(rand()); break;
		case 4: for(int j = 0, r = rand()%28; j < r; ++j) { (rand()%2) ? e.backspace() : e.del(); }; break; 
		case 5: rand()&2 ? e.home(true) : e.end(true);
		}
	}
	fstr end_fstr = e.get_str();

	for(int i = 0; i < 2; ++i) {
		while(e.undo()) {}
		EXPECT_EQ(wstring(L""), wstr(e.get_str()));
		while(e.redo()) {}
		EXPECT_EQ(wstr(end_fstr), wstr(e.get_str()));
	}
}

TEST(Text, BoundsCheck) {
	font f;
	font_file ff;
	ff.create(make_pair(0, 10));
	f.build(&ff);
	textbox tx(rect(), style(&f, pixel_32()));
	
	auto& e = tx.editor;
	EXPECT_EQ(wstring(L""), wstr(e.get_str()));
	e.select_all();
	e.backspace();
	EXPECT_EQ(wstring(L""), wstr(e.get_str()));
	e.select_all();
	e.del();
	EXPECT_EQ(wstring(L""), wstr(e.get_str()));
	e.select_all();
	e.character(L'L');
	EXPECT_EQ(wstring(L"L"), wstr(e.get_str()));
	e.select_all();
	e.backspace();
	EXPECT_EQ(wstring(L""), wstr(e.get_str()));
	
	e.select_word(0);
	e.backspace();
	EXPECT_EQ(wstring(L""), wstr(e.get_str()));
	e.select_word(0);
	e.del();
	EXPECT_EQ(wstring(L""), wstr(e.get_str()));
	e.select_word(0);
	e.character(L'L');
	EXPECT_EQ(wstring(L"L"), wstr(e.get_str()));
	e.select_word(1);
	e.backspace();
	EXPECT_EQ(wstring(L""), wstr(e.get_str()));
	e.character(L'a');
	e.character(L'b');
	e.character(L'c');
	e.character(L'd');
	e.select_word(4);
	EXPECT_EQ(4, e.get_caret_pos());
	EXPECT_EQ(0, e.get_left_selection());
	e.caret_right_word();
	e.character(L'\n');
	e.select_word(4);
	EXPECT_EQ(4, e.get_caret_pos());
	EXPECT_EQ(0, e.get_left_selection());

	e.guarded_redraw();
	e.select_all();
	e.del();
	
	e.select_line(0);
	e.backspace();
	EXPECT_EQ(wstring(L""), wstr(e.get_str()));
	e.select_line(0);
	e.del();
	EXPECT_EQ(wstring(L""), wstr(e.get_str()));
	e.select_line(0);
	e.character(L'L');
	EXPECT_EQ(wstring(L"L"), wstr(e.get_str()));
	e.select_line(0);
	e.backspace();
	EXPECT_EQ(wstring(L""), wstr(e.get_str()));

	e.backspace();
	e.select_all();
	EXPECT_EQ(wstring(L""), wstr(e.get_str()));
	e.del();
	EXPECT_EQ(wstring(L""), wstr(e.get_str()));
	e.del();
	e.caret_left(4234, false);
	EXPECT_EQ(wstring(L""), wstr(e.get_str()));
	e.caret_right(321, true);
	e.character(L'L');
	EXPECT_EQ(wstring(L"L"), wstr(e.get_str()));
	e.backspace();
	EXPECT_EQ(wstring(L""), wstr(e.get_str()));
	e.del();
	EXPECT_EQ(wstring(L""), wstr(e.get_str()));
	e.backspace();
	e.character(L'L');
	EXPECT_EQ(wstring(L"L"), wstr(e.get_str()));
	e.del();
	EXPECT_EQ(wstring(L"L"), wstr(e.get_str()));
	e.del();
	e.del();
	e.character(L'A');
	EXPECT_EQ(wstring(L"LA"), wstr(e.get_str()));
	e.backspace();
	EXPECT_EQ(wstring(L"L"), wstr(e.get_str()));
	e.del();
	e.del();
	e.backspace();
	e.character(L'L');
	e.backspace();
	EXPECT_EQ(wstring(L""), wstr(e.get_str()));
	e.backspace();
	e.backspace();
	e.insert(format(L"ABCDEFGHABCDEFGHABCDEFGH", style(&f, pixel_32())));
	while(e.undo());
	EXPECT_EQ(wstring(L""), wstr(e.get_str()));
	while(e.redo());
	EXPECT_EQ(wstring(L"ABCDEFGHABCDEFGHABCDEFGH"), wstr(e.get_str()));
	e.caret_right(321, true);
	e.caret_right_word(true);
	e.caret_left(2, false);
	e.caret_right(23, true);
	e.del();
	EXPECT_EQ(wstring(L"ABCDEFGHABCDEFGHABCDEF"), wstr(e.get_str()));
	e.end(true);
	e.caret_left(3, false);
	e.home(true);
	e.home(true);
	e.home(true);
	e.insert(format(L"abc", style(&f, pixel_32())));
	EXPECT_EQ(wstring(L"abcDEF"), wstr(e.get_str()));
}