#pragma once
#include "../system.h"
#include "drafter.h"
// draw with flag whether DRAFT or RENDER or just two alternative functions
// got to revise gui systems in terms of rectangle update'ing
namespace db {
	namespace graphics {
		namespace gui {
			namespace text {
				class printer {
					int q_begin, q_end, s_begin, s_end, caret_quad;
				public:
					/* defines how the caret should blink and whether should blink at all */
					struct blinker {
						bool blink, caret_visible;
						int interval_ms;

						static void regular_blink(blinker&, quad& caret);
						void (*blink_func)(blinker&, quad&);

						window::timer timer;
						blinker();
						void update(quad& caret);
						void reset();
					};

					drafter draft;
					blinker blink;
					pixel_32 selected_text_color;
					
					unsigned caret_width; 
					
					bool active, kerning, 
						align_caret_height, /* whether caret should be always of line height */
						highlight_current_line, 
						highlight_during_selection;
					
					material caret_mat, 
						highlight_mat, 
						selection_bg_mat,
						selection_inactive_bg_mat; /* material for line highlighting */

					printer();

					void draw_quads(const drafter::source_info&, std::vector<quad>& v);
					int get_quads_begin(), get_quads_end(), get_selection_begin(), get_selection_end(), get_caret_quad();
		 		};
				
				/* returns text's bounding box */

				rect_xywh quick_print(std::vector<quad>& v,
										const fstr& str, 
										point pos, 
										unsigned wrapping_width = 0,
										rect* clipper = 0);

				rect_xywh quick_print(std::vector<quad>& v,
										const std::wstring& wstr,
										gui::style style,
										point pos, 
										unsigned wrapping_width = 0,
										rect* clipper = 0);
			}
		}
	}
}