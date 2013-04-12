#pragma once
#include "printer.h"
#include "drafter.h"
namespace db {
	namespace graphics {
		namespace gui {
			namespace text {
				class text_rect : public text::printer {
					fstr _str;
					text::drafter draft;
					bool update_str;
				public:
					text_rect(const printer& = rect(), const fstr& = fstr());
					
						  fstr& str();
					const fstr& get_str();

					void guarded_redraw();
					virtual rect_wh get_content_size();
					virtual void draw_proc(const draw_info&);
				};
			}
		}
	}
}