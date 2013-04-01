#pragma once
#include "../font.h"
#include "../system.h"
#include "word_separator.h"
// ui relates on draft object (result) only
// if bugs viewing the caret, check the viewcaret where "car" variable was changed to caret_rect
namespace db {
	namespace graphics {
		namespace gui {
			struct rect;
			namespace text {
				struct drafter {
					struct caret_info {
						unsigned pos;
						int selection_offset;
						style default_style;
						caret_info(style);
					};

					struct line {
						unsigned hover(int x, const std::vector<int>& sectors) const;  /* return sector that is the closest x  */
						rect_xywh get_rect() const; /* actual line rect */

						int top, right, height() const, bottom() const, /* coordinates */
							asc, desc;
						unsigned begin, end;
						bool wrapped;
						line();
						void set(int y, int asc, int desc);
					};
					
					struct source_info {
						const gui::fstr& source;
						const drafter::caret_info* target_caret;
						/* is not const because its bounding box is expanded */
						rect* clipper;
						source_info(const gui::fstr& source, const drafter::caret_info* target_caret = 0, rect* clipper = 0);
					private:
						friend struct drafter;
						void find_ascdesc(int i, int j, int&, int&) const;
						int get_kern(const drafter&, unsigned code1, unsigned code2) const;
						font* getf(unsigned i) const;
					};

					std::vector<font::glyph*> cached;
					std::vector<line> lines;
					std::vector<int> sectors;
					rect_ltrb caret_rect;

					pointf pos;

					word_separator word_wrapper_separator;

					unsigned wrap_width;
					bool kerning;
					int first_line_visible, last_line_visible, max_x;

					drafter();

					void view_line(unsigned line, rect& clipper); /* tries to scroll clipper's pen so whole line is at view */
					unsigned get_line(unsigned caret_pos);
					unsigned map_mouse(const point& mouse);
					rect_ltrb get_bbox(); /* absolute */
				
					void draw(const source_info&);
					
					/* -1: there's no line visible */
					int get_first_line_visible(), get_last_line_visible();
				};
			}
		}
	}
}