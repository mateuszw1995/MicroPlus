#include "../include/db.h"
#include "header.h"
#include <gtest\gtest.h>

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);

	int aa;
	/*
	gui::font f;
	gui::font_file ff;
	f.build(&ff);
	gui::controls::textbox tx(rect_xywh(), gui::material(), gui::style(&f, pixel_32()));

	srand(time(0));
	gui::fstr initial_fstr = gui::formatted_text(random_wstr(rand()).c_str(), gui::style(&f, pixel_32(212, 100, 0, 192)));
	tx.insert(initial_fstr);

	for(int i = 0; i < 10000; ++i) {
		gui::style rand_style = gui::style(&f, pixel_32(rand()%256, rand()%256, rand()%256, rand()%256));
		switch(rand()%6) {
		case 0: tx.insert(gui::formatted_text(random_wstr(rand()%100).c_str(), rand_style)); break;
		case 1: rand()%2 ? tx.caret_left(rand()%20+10, true)  : tx.caret_left_word(true); tx.insert(gui::formatted_text(random_wstr(rand()%50).c_str(), rand_style)); break;
		case 2: rand()%2 ? tx.caret_right(rand()%20+10, true) : tx.caret_right_word(true);  tx.insert(gui::formatted_text(random_wstr(rand()%50).c_str(), rand_style)); break;
		case 3: for(int j = 0, r = rand()%23; j < r; ++j) tx.character(rand()); break;
		case 4: for(int j = 0, r = rand()%28; j < r; ++j) { (rand()%2) ? tx.backspace() : tx.del(); }; break; 
		case 5: rand()&2 ? tx.home(true) : tx.end(true);
		}
	}
	gui::fstr end_fstr = tx.get_str();*/
	::testing::FLAGS_gtest_catch_exceptions = false;
	::testing::FLAGS_gtest_break_on_failure = true;
	auto result = RUN_ALL_TESTS();
	std::cin >> aa;
	return result;
}
