#pragma once
#include "../rect.h"
// got to revise gui systems in terms of rectangle update'ing
namespace db {
	namespace graphics {
		namespace gui {
			namespace text {
				struct caret_info;
				class ui;
				struct drafter;
				struct printer : public rect {
					/* these integers hold information at what index particular quads are pushed to the resulting vector
					may be useful for further processing
					still needs implementing
					*/
					struct index_info {
						int first_character, last_character, 
							selections_first, selections_last,
							highlight,
							caret;
					} quad_indices;

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
					
					printer(const rect&);

					void draw_text(std::vector<quad>& out, const ui&) const;
					
					/* if caret is 0, draw no caret */
					void draw_text(std::vector<quad>& out, 
						const drafter&, 
						const fstr& colors, 
						const caret_info* caret = 0) const;
					
					virtual void draw_proc(const draw_info&) = 0;
		 		};
				
				/* 
				parent shifts position and clips the text
				wrapping_width = 0 means no wrapping
				 parent = 0 means no clipping/shifting
				returns text's bounding box (without clipping)
				*/

				rect_wh quick_print(std::vector<quad>& v,
										const fstr& str, 
										point pos, 
										unsigned wrapping_width = 0,
										rect_xywh* parent = 0);

				rect_wh quick_print(std::vector<quad>& v,
										const std::wstring& wstr,
										gui::style style,
										point pos, 
										unsigned wrapping_width = 0,
										rect_xywh* parent = 0);
			}
		}
	}
}