#pragma once
#include "rect.h"
#include "font.h"
#include "word_separator.h"

namespace db {
	namespace graphics {
		namespace gui {
			class text_printer {
				rect* p;
				std::vector<font::glyph*> cached;
				
				int q_begin, q_end, s_begin, s_end, caret_quad;
				int first_line_visible, last_line_visible, max_x;

				void update_text();
				void find_ascdesc(int i, int j, int&, int&);
			public:
				struct line {
					unsigned anchor(int x, const std::vector<int>& sectors) const; /* return sector that is the closest x */
					unsigned hover(int x, const std::vector<int>& sectors) const;  /* return sector that x exactly is on  */
					rect_xywh get_rect() const; /* get the actual line coordinates */

				private:
					friend class text_printer;
					friend class text_interface;
					int top, right, height() const, bottom() const, /* coordinates */
						asc, desc;
					unsigned begin, end;
					bool wrapped;
					line();
					void set(int y, int asc, int desc);
				};
				
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

				struct caret_info {
					unsigned pos, line;
					int selection_offset;
					style default_style;
					caret_info(style);
				} *caret;

				/* USER SETTINGS */
				
				fstr* source;
				blinker blink;
				pointf pos;

				word_separator word_wrapper_separator;

				pixel_32 selected_text_color;
				material caret_mat, 
					highlight_mat, 
					selection_bg_mat,
					selection_inactive_bg_mat;/* material for line highlighting */
				unsigned caret_width, wrap_width;
				bool active, kerning, 
					align_caret_height, /* whether caret should be always of line height */
					highlight_current_line, 
					highlight_during_selection;
				
				/* optional access */
				std::vector<line> lines;
				std::vector<int> sectors;
				rect_ltrb caret_rect;

				text_printer::text_printer(fstr* source, caret_info* inout_caret = 0);

				/* GUI utilities */
				void update_rectangles(rect* clipper);
				void draw_proc(std::vector<quad>& v, rect* clipper);
				
				void view_line(unsigned line, rect& clipper); /* tries to scroll pen so whole line is at view */

				/* getters */
				font* getf(unsigned i);
				int get_line(unsigned character_pos);
				unsigned map_mouse(const point&, bool nearest);
				int get_kern(unsigned code1, unsigned code2);
				rect_ltrb get_bbox(); /* same as rect::global */
				unsigned get_first_line_visible(), get_last_line_visible();
				int get_quads_begin(), get_quads_end(), get_selection_begin(), get_selection_end(), get_caret_quad();
				rect* get_parent();
			};
		}
	}
}