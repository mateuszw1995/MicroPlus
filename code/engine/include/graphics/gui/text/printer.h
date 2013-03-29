#pragma once
#include "../rect.h"
#include "drafter.h"

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

					blinker blink;
					drafter draft;

					pixel_32 selected_text_color;
					material caret_mat, 
						highlight_mat, 
						selection_bg_mat,
						selection_inactive_bg_mat; /* material for line highlighting */

					printer(const drafter&);

					void draw_quads(std::vector<quad>& v, rect* clipper);
					int get_quads_begin(), get_quads_end(), get_selection_begin(), get_selection_end(), get_caret_quad();
		 		};
			}
		}
	}
}