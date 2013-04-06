#pragma once
#include <algorithm>
#include "../rect.h"
#include "drafter.h"
#include "../../../window/window.h"

#undef min
#undef max
namespace db {
	using namespace math;
	namespace graphics {
		using namespace io::input;
		namespace gui {
			namespace text {
				bool is_newline(unsigned i) {
					return (i == 0x000A || i == 0x000D);
				}
				
				font* drafter::getf(const gui::fstr& source, unsigned i) const {
					//return (i < source.length() && source[i].font_used) ? source[i].font_used : target_caret->default_style.f;
					return source[i].font_used;
				}
				
				int drafter::get_kern(const gui::fstr& source, unsigned i, unsigned l) const {
					if(kerning && i > lines[l].begin && getf(source, i) == getf(source, i-1)) {
						auto& vk = cached[i]->info->kerning;
						for(unsigned k = 0; k < vk.size(); ++k)
							if(vk[k].first == source[i-1].c)
								return vk[k].second;
					}
					return 0;
				}
				
				void drafter::find_ascdesc(const gui::fstr& in, int l, int r, int& asc, int& desc) const {
					if(l == r) {
						if(l > 0) {
							asc =  getf(in, l-1)->parent->ascender;
							desc = getf(in, l-1)->parent->descender;
						}
						else {
							asc =  getf(in, l)->parent->ascender;
							desc = getf(in, l)->parent->descender;
						}
					}
					else {
						asc = getf(in, l)->parent->ascender, desc = getf(in, l)->parent->descender;

						for(int j = l; j < r; ++j) {
							asc =  max(asc, getf(in, j)->parent->ascender);
							desc = min(desc, getf(in, j)->parent->descender);
						}
					}
				}

				int drafter::line::height() const {
					return asc-desc;
				}

				int drafter::line::bottom() const {
					return top+height();
				}

				drafter::line::line() : begin(0), end(0), top(0), right(0), asc(0), desc(0), wrapped(false) {}

				rect_xywh drafter::line::get_rect() const {
					return rect_ltrb(0, top, right, bottom());
				}

				void drafter::line::set(int _y, int _asc, int _desc) {
					top = _y;
					asc = _asc;
					desc = _desc;
				}
				
				void drafter::line::adjust(font* f) {
					asc =  max(asc, f->parent->ascender);
					desc = min(desc, f->parent->descender);
				}

				bool drafter::line::empty() const {
					return begin == end;
				}

				unsigned drafter::line::hover(int x, const std::vector<int>& sectors) const {
					if(end - begin == 0) 
						return 0; /* obvious if we have no sectors */

					unsigned iter = upper_bound(sectors.begin() + begin, sectors.begin() + end, x) - sectors.begin();

					/* if iter is equal to begin there's no "previous" sector we can compare to */
					if(iter == begin) return iter;
					
					/* snap result to the closest sector */
					if(!wrapped && sectors[iter] - x > x - sectors[iter-1])
						--iter;
					else if(wrapped && (iter < end ? sectors[iter] : right) - x > x - sectors[iter-1])
						--iter;

					return iter;
				}

				unsigned drafter::get_line(unsigned i) const {
					if(lines.empty()) return 0;
					line l;
					l.begin = i;
					return upper_bound(lines.begin(), lines.end(), l, [](const line& x, const line& y){return x.begin < y.begin;}) - lines.begin() - 1;
				}

				drafter::drafter()
					:  wrap_width(0), 
					kerning(true), max_x(0)
				{
					lines.push_back(line());
				}

				point drafter::view_caret(unsigned caret_pos, const rect_ltrb& clipper) const {
					point offset(0, 0);

					if(!clipper.good() || !clipper.hover(get_bbox()))
						return offset;
					
					/* we are now sure that both rectangles intersect */
					
					/* shortcut */
					auto& l = lines[get_line(caret_pos)];

					/* if requested line's top is above or it won't fit at all, snap clipper to it's top */
					if(clipper.h() < l.height() || l.top < clipper.t) 
						offset.y = l.top - clipper.t; 
					/* otherwise if its bottom is under clipper's bottom, snap to line's bottom */
					else if(l.bottom() > clipper.b) 
						offset.y = l.bottom() - clipper.b; 

					int car = sectors[caret_pos];
					if(car <= clipper.l)
						offset.x = car - clipper.l - 1;
					else if(car >= clipper.r)
						offset.x = car - clipper.r + 1;

					return offset;
				}

				unsigned drafter::map_to_line(const point& p) const {
					if(lines.empty() || sectors.empty() || p.y < 0) return 0;
					line l;
					l.top = p.y;
					l.desc = l.asc = 0;
					int res = lower_bound(lines.begin(), lines.end(), l, [](const line& x, const line& y){return x.bottom() < y.bottom();}) - lines.begin();
					if(res == lines.size()) --res;
					return res;
				}

				unsigned drafter::map_to_caret_pos(const point& p) const {
					return lines[map_to_line(p)].hover(p.x, sectors);
				}

				void drafter::draw(const fstr& source) {
					/* whole structural data clears */
					cached.clear();
					lines.clear();
					sectors.clear();
					max_x = 0;
					
					/* add a new, empty, initial line 
						there is ALWAYS at least one, even with empty string
					*/
					lines.push_back(line());

					/* we have nothing to draw */
					if(source.empty()) return; 

					/* reserve enough space to avoid reallocation */
					cached.reserve(source.size());

					/* update glyph data so each glyph object corresponds to a string character */
					for(unsigned i = 0; i < source.size(); ++i)
						cached.push_back(&getf(source, i)->get_glyph(source[i].c));


					unsigned l = 0, i = 0;
					point pen(0, 0);

					/* FIRST PASS: ONLY GENERATE LINES DEPENDING ON NEWLINE CHARACTERS AND WRAPPING WIDTH */
					/* for every character */
					for(i = 0; i < cached.size(); ++i) {
						/* shortcut */
						auto& g = *cached[i];

						/* advance pen taking kerning into consideration */
						pen.x += get_kern(source,  i, l) + g.info->adv;
						/* at this point "pen.x" means "where would caret be AFTER placing this character" */
						bool wrap = (wrap_width > 0 && pen.x >= int(wrap_width));

						/* if we have just encountered a newline character or there is need to wrap, we have to break the current line and 
						create another */
						if(is_newline(source[i].c) || wrap) {
							/* take care of the current line */
							lines[l].wrapped = wrap;
							/* this will be moved left if we're wrapping */
							lines[l].right = pen.x;
							/* end is exclusive, so we add 1 */
							lines[l].end = i+1;
							
							/* push new line object */
							lines.push_back(line());
							/* set it, begin is inclusive */
							lines[l+1].begin = lines[l].end;

							/* reset pen */
							pen.x = 0;

							if(wrap) {
								/* find word breaking character or return begin if there's no such in the current line */
								int left = word_wrapper_separator.get_left_word(source, i+1, lines[l].begin);
								/* if current line is not entirely a single word, we can move the last word to the next line */
								if( lines[l].end - left > lines[l].begin) {
									/* update lines' bounds with length of last word */
									lines[l].end -= left;
									lines[l+1].begin -= left;

									/* update pen so it's in front of the moved word */
									for(unsigned k = lines[l+1].begin; k < lines[l+1].begin+left; ++k) {
										int advance = cached[k]->info->adv + get_kern(source, k, l+1);
										pen.x += advance;
										/* also update current line's right coordinate as we're taking characters away */
										lines[l].right -= advance;
									}
								}
								/* otherwise we move only last character and update pen; 
									note there's no kerning because it's always first character in line
								*/
								else pen.x += g.info->adv;
							}
							
							/* expand text bounding box's right coordinate */
							max_x = max(max_x, lines[l].right);

							/* proceed to the newly created line */
							++l;
						}
					}

					/* break the last line as it will never hit condition of wrapping nor newlining 
						same things happen here
					*/
					/* we don't add 1 as i is already incremented after finishing the loop */
					lines[l].end = i;//+1;
					lines[l].right = pen.x;
					max_x = max(max_x, lines[l].right);

					/* SECOND PASS: GENERATE SECTORS AND FILL LINE INFO DEPENDING ON CHARACTERS HEIGHT */
					
					/* just to make sure */
					pen = point();
					for(l = 0; l < lines.size(); ++l) {
						lines[l].top = pen.y;

						/* roll pen back */
						pen.x = 0;
						for(i = lines[l].begin; i < lines[l].end && i < cached.size(); ++i) {
							/* update line's height so it is maximum of all characters' heights */
							lines[l].adjust(getf(source, i));

							pen.x += get_kern(source, i, l);
							sectors.push_back(pen.x);

							pen.x += cached[i]->info->adv;
						}
						pen.y += lines[l].height();
					}

					sectors.push_back(pen.x);
				}

				rect_wh drafter::get_bbox() const {
					if(sectors.empty() || lines.empty()) return rect_wh(0,0);
					return rect_wh(max_x, lines[lines.size()-1].bottom());
				}

				pair<int, int> drafter::get_line_visibility(const rect_ltrb& clipper) const {
					if(!clipper.good() || !clipper.hover(get_bbox())) 
						return make_pair(-1, -1);

					/* we are now sure that both rectangles intersect */
					return make_pair(map_to_line(point(0, clipper.t)), map_to_line(point(0, clipper.b)));
				}
			}
		}
	}
}