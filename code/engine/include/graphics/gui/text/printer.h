#pragma once
#include "../rect.h"
// got to revise gui systems in terms of rectangle update'ing
namespace db {
	namespace graphics {
		namespace gui {
			namespace text {
				struct caret_info;
				class ui;
				class drafter;
				struct printer : public rect {
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
					
					struct {
						int first_character, last_character, 
							selections_first, selections_last,
							highlight,
							caret;
					} quad_indices;

					printer(const rect&);

					void draw_text(const draw_info& in, const ui&) const;
					
					/* if caret is 0, draw no caret */
					void draw_text(const draw_info& in, 
						const drafter&, 
						const fstr& colors, 
						const caret_info* caret = 0) const;
					
					virtual void update_rectangles() = 0;
					virtual void draw_proc(const draw_info&) = 0;
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

			class text_rect : public text::printer {
				fstr str;
				text::drafter draft;
				bool update_str;

				text_rect(const rect&);
				virtual void update_rectangles();
				virtual void draw_proc(const draw_info&);
			};
		}
	}
}