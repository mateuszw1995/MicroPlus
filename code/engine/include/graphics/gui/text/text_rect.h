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

					void guarded_redraw();
					virtual rect_wh get_content_size();
					virtual void draw_proc(const draw_info&);
				};
			}
		}
	}
}