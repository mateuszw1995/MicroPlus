#pragma once
#include "printer.h"
#include "drafter.h"
namespace db {
	namespace graphics {
		namespace gui {
			namespace text {
				struct text_rect : public text::printer {
					fstr str;
					text::drafter draft;
					bool update_str;

					text_rect(const rect& = rect());
					virtual void update_rectangles();
					virtual void draw_proc(const draw_info&);
				};
			}
		}
	}
}