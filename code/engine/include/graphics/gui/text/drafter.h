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
					struct line {
						unsigned hover(int x, const std::vector<int>& sectors) const;  /* return sector that is the closest x  */
						rect_xywh get_rect() const; /* actual line rect */

						int top, right, height() const, bottom() const, /* coordinates */
							asc, desc;
						unsigned begin, end;
						bool wrapped;
						line();
						void set(int y, int asc, int desc);
						void adjust(font*);
					};

					std::vector<font::glyph*> cached;
					std::vector<line> lines;
					std::vector<int> sectors;

					word_separator word_wrapper_separator;

					unsigned wrap_width;
					
					/* WARNING! Setting kerning flag to true may reuslt in performance hit if rendering huge amounts of text */
					bool kerning;

					drafter();

					/* returns offset that clipper must be moved to show whole caret */
					point view_caret(unsigned caret_pos, const rect_ltrb& clipper) const; 
					unsigned get_line(unsigned caret_pos) const;
					unsigned map_to_line(const point&) const;
					unsigned map_to_caret_pos(const point&) const;
					rect_ltrb get_bbox() const;
				
					void draw(const fstr&);

					/* if any is -1, there's no line visible */
					pair<int, int> get_line_visibility(const rect_ltrb& clipper) const;
				private:
					int max_x;
					void find_ascdesc(const gui::fstr& source, int i, int j, int&, int&) const;
					int get_kern(const gui::fstr& source, unsigned code1, unsigned code2) const;
					font* getf(const gui::fstr& source, unsigned i) const;
				};
			}
		}
	}
}