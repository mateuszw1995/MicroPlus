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
				
				drafter::source_info::source_info(const gui::fstr& source, const drafter::caret_info* target_caret, rect* clipper) : 
					source(source), target_caret(target_caret), clipper(clipper) {
				}

				font* drafter::source_info::getf(unsigned i) const {
					return (i < source.length() && source[i].font_used) ? source[i].font_used : target_caret->default_style.f;
				}
				
				int drafter::source_info::get_kern(const drafter& in, unsigned i, unsigned l) const {
					if(in.kerning && i > in.lines[l].begin && getf(i) == getf(i-1)) {
						auto& vk = in.cached[i]->info->kerning;
						for(unsigned k = 0; k < vk.size(); ++k)
							if(vk[k].first == source[i-1].c)
								return vk[k].second;
					}
					return 0;
				}
				
				void drafter::source_info::find_ascdesc(int l, int r, int& asc, int& desc) const {
					if(l == r) {
						if(l > 0) {
							asc =  getf(l-1)->parent->ascender;
							desc = getf(l-1)->parent->descender;
						}
						else {
							asc =  getf(l)->parent->ascender;
							desc = getf(l)->parent->descender;
						}
					}
					else {
						asc = getf(l)->parent->ascender, desc = getf(l)->parent->descender;

						for(int j = l; j < r; ++j) {
							asc =  max(asc, getf(j)->parent->ascender);
							desc = min(desc, getf(j)->parent->descender);
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

				unsigned drafter::get_line(unsigned i) {
					if(lines.empty()) return 0;
					line l;
					l.begin = i;
					return upper_bound(lines.begin(), lines.end(), l, [](const line& x, const line& y){return x.begin < y.begin;}) - lines.begin() - 1;
				}

				drafter::caret_info::caret_info(style default_style) : default_style(default_style), pos(0), selection_offset(0) {}

				drafter::drafter()
					:  wrap_width(0), 
					kerning(true), align_caret_height(true), caret_width(0),
					highlight_current_line(false),
					highlight_during_selection(false), active(false), first_line_visible(-1), last_line_visible(-1), max_x(0)
				{
					lines.push_back(line());
				}

				void drafter::view_line(unsigned line, rect& clipper) {	
					if(lines.empty() || sectors.empty()) return;

					const rect_ltrb& rc = clipper.get_rect_absolute();
					pointf& pen = clipper.pen;
					pointf tp = pen;
					if(line <= unsigned(first_line_visible))
						pen.y = float((lines[line].top - lines[0].top));
					else if(line >= unsigned(last_line_visible))
						pen.y = float((lines[line].top - lines[0].top) + rc.h() - lines[line].height());

					int car = caret_rect.l;
					if(car <= rc.l)
						pen.x = float((car - sectors[lines[line].begin]));
					if(car >= rc.r)
						pen.x = float((car - sectors[lines[line].begin]) + rc.w() - 1);

					pen.y = max(pen.y, 0.f);
					pen.x = max(pen.x, 0.f);
				}

				unsigned drafter::map_mouse(const point& p) {
					if(lines.empty() || sectors.empty()) return 0;
					line l;
					l.top = p.y;
					l.desc = l.asc = 0;
					auto it = lower_bound(lines.begin() + first_line_visible, lines.begin() + last_line_visible, l, [](const line& x, const line& y){return x.top+x.height() < y.top+y.height();});
					if(it == lines.end()) --it;
					return (*it).hover(p.x, sectors);
				}

				void drafter::draw(const source_info& in) {
					/* shortcuts */
					auto& source = in.source;
					auto* target_caret = in.target_caret;
					auto* clipper = in.clipper;

					/* whole structural data clears */
					cached.clear();
					lines.clear();
					sectors.clear();
					first_line_visible = -1;
					last_line_visible = -1;
					max_x = 0;

					if(!target_caret && source.empty()) return; /* we have nothing to draw, even caret */

					/* reserve enough space to avoid reallocation */
					cached.reserve(source.size());

					/* update glyph data so each glyph object correspond to a string character */
					for(unsigned i = 0; i < source.size(); ++i)
						cached.push_back(&in.getf(i)->get_glyph(source[i].c));

					/* add a new, empty, initial line */
					lines.push_back(line());

					/* if clipper is specified, set offset clipper's absolute xy and pen into account 
						(so it behaves like a child rectangle)
					*/
					point offset;
					if(in.clipper)
						offset = pos + in.clipper->get_absolute_xy() - in.clipper->pen;	
					/* otherwise our offset is just "pos" */
					else
						offset = pos;

					unsigned l = 0, i = 0;
					point pen(0, 0);
					/* FIRST PASS: ONLY GENERATE LINES DEPENDING ON NEWLINES AND WRAPPING WIDTH */
					/* for every character */
					for(i = 0; i < cached.size(); ++i) {
						/* shortcut */
						auto& g = *cached[i];

						/* advance pen taking kerning into consideration */
						pen.x += in.get_kern(*this, i, l) + g.info->adv;
						bool wrap = (wrap_width > 0 && pen.x >= wrap_width);

						/* if we have just encountered a newline character or there is need to wrap, we have to break the current line and 
						create another */
						if(is_newline(source[i].c) || wrap) {
							/* take care of the current line */
							lines[l].wrapped = wrap;
							/* this will change if we're wrapping */
							lines[l].right = pen.x + offset.x;
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
										int advance = cached[k]->info->adv + in.get_kern(*this, k, l+1);
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
						same things happen
					*/
					//lines[l].wrapped = false;

					/* we don't add 1 as i is already incremented after finishing the loop */
					lines[l].end = i;//+1;
					lines[l].right = pen.x + offset.x;
					max_x = max(max_x, lines[l].right);

					/* SECOND PASS: GENERATE SECTORS AND FILL LINE INFO DEPENDING ON CHARACTERS HEIGHT */
					for(l = 0; l < lines.size(); ++l) {
						g_pen.x = line_x;
						int asc, desc;
						in.find_ascdesc(lines[l].begin, lines[l].end, asc, desc);
						lines[l].set(g_pen.y, asc, desc);

						bool not_above_clipper = (clipper && lines[l].bottom() >= clipper->get_clipped_rect().t);
						if(not_above_clipper && first_line_visible == -1)					  first_line_visible = l;
						if(not_above_clipper && lines[l].top <= clipper->get_clipped_rect().b) last_line_visible = l;

						g_pen.y += asc;

						for(i = lines[l].begin; i < lines[l].end && i < cached.size(); ++i) {
							g_pen.x += in.get_kern(*this, i, l);
							sectors.push_back(g_pen.x);

							g_pen.x += cached[i]->info->adv;
						}
						g_pen.y -= desc;
					}

					sectors.push_back(g_pen.x);

					if(target_caret) {
						unsigned caret_line = get_line(target_caret->pos);

						if(align_caret_height)
							caret_rect = rect_xywh(sectors[target_caret->pos], lines[caret_line].top, caret_width, lines[caret_line].height());
						else {
							int pos = max(1u, target_caret->pos);
							caret_rect = rect_xywh(sectors[target_caret->pos], lines[caret_line].top + lines[caret_line].asc - in.getf(pos-1)->parent->ascender, 
								caret_width, in.getf(pos-1)->parent->ascender - in.getf(pos-1)->parent->descender);
						}
					}

					if(last_line_visible == -1)  last_line_visible = l;
					//last_line_visible = min((unsigned)last_line_visible, lines.size()-1);
					//if(first_line_visible == -1)  first_line_visible = 0;

					if(in.clipper) in.clipper->bounding_box.contain(get_bbox() - (in.clipper ? in.clipper->get_absolute_xy() - in.clipper->pen : point(0, 0)));
				}

				rect_ltrb drafter::get_bbox() {
					if(sectors.empty() || lines.empty()) return rect_ltrb(0,0,0,0);
					return rect_ltrb(sectors[0], lines[0].top, max_x, lines[lines.size()-1].top + lines[lines.size()-1].height());
				}
			}
		}
	}
}