#pragma once
#include "../rect.h"
#include "../font.h"
#include "word_separator.h"
// ui relates on draft object (result) only
// if bugs viewing the caret, check the viewcaret where "car" variable was changed to caret_rect
namespace db {
	namespace graphics {
		namespace gui {
			namespace text {
				struct drafter {
					struct caret_info {
						unsigned pos;
						int selection_offset;
						style default_style;
						caret_info(style);
					};

					class draft {
						friend class drafter;
						void clear();
					public:
						int first_line_visible, last_line_visible, max_x;

						struct line {
							unsigned anchor(int x, const std::vector<int>& sectors) const; /* return sector that is the closest x */
							unsigned hover(int x, const std::vector<int>& sectors) const;  /* return sector that x exactly is on  */
							rect_xywh get_rect() const; /* actual line rect */

							int top, right, height() const, bottom() const, /* coordinates */
								asc, desc;
							unsigned begin, end;
							bool wrapped;
							line();
							void set(int y, int asc, int desc);
						};

						std::vector<font::glyph*> cached;
						std::vector<line> lines;
						std::vector<int> sectors;
						rect_ltrb caret_rect;
						
						draft();

						void view_line(unsigned line, rect& clipper); /* tries to scroll clipper's pen so whole line is at view */
						unsigned get_line(unsigned caret_pos);
						unsigned map_mouse(const point& mouse, bool nearest);
						rect_ltrb get_bbox(); /* absolute */
					};

					gui::fstr* source;
					pointf pos;

					caret_info* target_caret; /* nullptr - display neither caret nor selection */
					word_separator word_wrapper_separator;

					unsigned caret_width, wrap_width;
					bool active, kerning, 
						align_caret_height, /* whether caret should be always of line height */
						highlight_current_line, 
						highlight_during_selection;

					drafter(gui::fstr* source, drafter::caret_info* inout_caret = 0);

					void draw(rect* clipper, draft& output);
				
				private:
					void find_ascdesc(int i, int j, int&, int&);
					int get_kern(unsigned code1, unsigned code2, const draft& current_draft);
					font* getf(unsigned i);
				};
			}
		}
	}
}