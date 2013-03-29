#pragma once
#include <algorithm>
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

				font* drafter::getf(unsigned i) {
					return (i < (*source).length() && (*source)[i].font_used) ? (*source)[i].font_used : caret->default_style.f;
					//return (*source)[i].font_used;
				}

				int drafter::draft::line::height() const {
					return asc-desc;
				}

				int drafter::draft::line::bottom() const {
					return top+height();
				}

				drafter::draft::line::line() : begin(0) {}

				rect_xywh drafter::draft::line::get_rect() const {
					return rect_ltrb(0, top, right, bottom());
				}

				void drafter::draft::line::set(int _y, int _asc, int _desc) {
					top = _y;
					asc = _asc;
					desc = _desc;
				}

				unsigned drafter::draft::line::anchor(int x, const std::vector<int>& sectors) const {
					unsigned it = hover(x, sectors);

					if(!wrapped) {
						if((it >= end) || (it > begin && sectors[it] - x > x - sectors[it-1])) 
							--it;
					}
					else if(wrapped) {
						if(((it > begin && (it < end ? sectors[it] : right) - x > x - sectors[it-1])))
							--it;
					}

					return it;
				}

				unsigned drafter::draft::line::hover(int x, const std::vector<int>& sectors) const {
					return upper_bound(sectors.begin() + begin, sectors.begin() + end, x) - sectors.begin();
				}

				unsigned drafter::draft::get_line(unsigned i) {
					line l;
					l.begin = i;
					return upper_bound(lines.begin(), lines.end(), l, [](const line& x, const line& y){return x.begin < y.begin;}) - lines.begin() - 1;
				}

				drafter::caret_info::caret_info(style default_style) : default_style(default_style), pos(0), selection_offset(0) {}

				drafter::draft::draft() : first_line_visible(-1), last_line_visible(-1), max_x(0) {}

				drafter::drafter(fstr* source, drafter::caret_info* caret)
					: target_caret(caret), source(source), wrap_width(0),
					kerning(true), align_caret_height(true), caret_width(0),
					highlight_current_line(false),
					highlight_during_selection(false), active(false)
				{}

				void drafter::draft::view_line(unsigned line, rect& clipper) {	
					if(!sectors.empty() && !lines.empty()) {
						const rect_ltrb& rc = clipper.get_rect_absolute();
						pointf& pen = clipper.pen;
						pointf tp = pen;
						if(line <= unsigned(first_line_visible))
							pen.y = float((lines[line].top - lines[0].top));
						else if(line >= unsigned(last_line_visible))
							pen.y = float((lines[line].top - lines[0].top) + rc.h() - lines[line].height());

						int car = caret_rect.x;
						if(car <= rc.l)
							pen.x = float((car - sectors[lines[line].begin]));
						if(car >= rc.r)
							pen.x = float((car - sectors[lines[line].begin]) + rc.w() - 1);

						pen.y = max(pen.y, 0.f);
						pen.x = max(pen.x, 0.f);
					}
				}

				unsigned drafter::draft::map_mouse(const point& p, bool nearest) {
					if(lines.empty()) return 0;
					line l;
					l.top = p.y;
					l.desc = l.asc = 0;
					auto it = lower_bound(lines.begin() + first_line_visible, lines.begin() + last_line_visible, l, [](const line& x, const line& y){return x.top+x.height() < y.top+y.height();});
					if(it == lines.end()) --it;
					return nearest ? (*it).anchor(p.x, sectors) : (*it).hover(p.x, sectors)-1;
				}

				void drafter::find_ascdesc(int l, int r, int& asc, int& desc) {
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

				int drafter::get_kern(unsigned i, unsigned l, const draft& current_draft) {
					if(kerning && i > current_draft.lines[l].begin && getf(i) == getf(i-1)) {
						auto& vk = current_draft.cached[i]->info->kerning;
						for(unsigned k = 0; k < vk.size(); ++k)
							if(vk[k].first == (*source)[i-1].c)
								return vk[k].second;
					}
					return 0;
				}

				void drafter::draft::clear() {
					cached.clear();
					lines.clear();
					sectors.clear();
					first_line_visible = -1;
					last_line_visible = -1;
				}

				void drafter::draw(rect* clipper, draft& out) {
					point g_pen(clipper ? clipper->get_absolute_xy() - clipper->pen + pos : pos);

					int asc, desc;
					bool crlf = false;
					unsigned line_x = g_pen.x, l = 0, i;

					/* whole structural data clears */
					out.clear();

					if(!target_caret && source->empty()) return; /* we have nothing to draw, even caret */

					/* reserve enough space to avoid reallocation */
					out.cached.reserve((*source).size());

					/* update glyph data so each glyph object correspond to a string character */
					for(unsigned i = 0; i < (*source).size(); ++i)
						out.cached.push_back(&getf(i)->get_glyph((*source)[i].c));

					/* add a new, empty, initial line */
					out.lines.push_back(draft::line());

					/* set right bounding box coordinate to current line_x, further expanded */
					out.max_x = line_x;

					/* for every character */
					for(i = 0; i < out.cached.size(); ++i) {
						crlf = is_newline((*source)[i].c);

						/* shortcut */
						auto& g = *out.cached[i];

						out.lines[l].right = g_pen.x;
						g_pen.x += get_kern(i, l, out) + g.info->adv;
						bool wrap = (clipper && wrap_width > 0 && g_pen.x >= clipper->get_rect_absolute().l + wrap_width - clipper->pen.x);

						/* if we have encountered a newline character or there is need to wrap, we have to break the current line and 
						create another */
						if(crlf || wrap) {

							/* push new line object */
							out.lines.push_back(draft::line());

							out.lines[l].wrapped = wrap;
							out.lines[l].end = out.lines[l+1].begin = i+1-wrap;

							g_pen.x = line_x;
							if(wrap) {
								int left = word_wrapper_separator.get_left_word(*source, i, out.lines[l].begin);
								if((out.lines[l].end - left) > out.lines[l].begin) {
									out.lines[l].end -= left;
									out.lines[l+1].begin -= left;

									for(unsigned k = out.lines[l+1].begin; k <= out.lines[l+1].begin+left; ++k)
										g_pen.x += out.cached[k]->info->adv + get_kern(k, l+1, out);
								}
								else g_pen.x += g.info->adv + get_kern(i, l, out);
							}

							++l;
						} else out.max_x = max(out.max_x, g_pen.x);
					}

					out.lines[l].wrapped = false;
					out.lines[l].end = i+1;

					for(l = 0; l < out.lines.size(); ++l) {
						g_pen.x = line_x;
						find_ascdesc(out.lines[l].begin, out.lines[l].end-1, asc, desc);
						out.lines[l].set(g_pen.y, asc, desc);
						if(clipper && out.lines[l].top + out.lines[l].height() >= clipper->get_clipped_rect().t && out.first_line_visible == -1) out.first_line_visible = l;
						if(clipper && out.lines[l].top + out.lines[l].height() >= clipper->get_clipped_rect().t && out.lines[l].top <= clipper->get_clipped_rect().b) out.last_line_visible = l;

						g_pen.y += asc;

						for(i = out.lines[l].begin; i < out.lines[l].end && i < out.cached.size(); ++i) {
							g_pen.x += get_kern(i, l, out);
							out.sectors.push_back(g_pen.x);

							g_pen.x += out.cached[i]->info->adv;
						}
						g_pen.y -= desc;
					}

					out.sectors.push_back(g_pen.x);

					if(target_caret) {
						unsigned caret_line = out.get_line(target_caret->pos);

						if(align_caret_height)
							out.caret_rect = rect_xywh(out.sectors[target_caret->pos], out.lines[caret_line].top, caret_width, out.lines[caret_line].height());
						else {
							int pos = max(1u, target_caret->pos);
							out.caret_rect = rect_xywh(out.sectors[target_caret->pos], out.lines[caret_line].top + out.lines[caret_line].asc - getf(pos-1)->parent->ascender, 
								caret_width, getf(pos-1)->parent->ascender - getf(pos-1)->parent->descender);
						}
					}

					if(out.last_line_visible == -1)  out.last_line_visible = l;
					out.last_line_visible = min((unsigned)out.last_line_visible, out.lines.size()-1);
					if(out.first_line_visible == -1)  out.first_line_visible = 0;

					if(clipper) clipper->bounding_box.contain(out.get_bbox() - (clipper ? clipper->get_absolute_xy() - clipper->pen : point(0, 0)));
				}

				rect_ltrb drafter::draft::get_bbox() {
					if(sectors.empty() || lines.empty()) return rect_ltrb(0,0,0,0);
					return rect_ltrb(sectors[0], lines[0].top, max_x, (*lines.rbegin()).top + (*lines.rbegin()).height());
				}
			}
		}
	}
}