#pragma once
#include <algorithm>
#include "text_printer.h"
#include "../../window/window.h"

#undef min
#undef max
namespace db {
	using namespace math;
	namespace graphics {
		using namespace io::input;
		namespace gui {
			bool is_newline(unsigned i) {
				return (i == 0x000A || i == 0x000D);
			}
			
			font* text_printer::getf(unsigned i) {
				return (i < (*source).length() && (*source)[i].font_used) ? (*source)[i].font_used : caret->default_style.f;
				//return (*source)[i].font_used;
			}

			int text_printer::line::height() const {
				return asc-desc;
			}

			int text_printer::line::bottom() const {
				return top+height();
			}

			text_printer::line::line() : begin(0) {}

			rect_xywh text_printer::line::get_rect() const {
				return rect_ltrb(0, top, right, bottom());
			}

			void text_printer::line::set(int _y, int _asc, int _desc) {
				top = _y;
				asc = _asc;
				desc = _desc;
			}
			
			unsigned text_printer::line::anchor(int x, const std::vector<int>& sectors) const {
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
					
			unsigned text_printer::line::hover(int x, const std::vector<int>& sectors) const {
				return upper_bound(sectors.begin() + begin, sectors.begin() + end, x) - sectors.begin();
			}

			int text_printer::get_line(unsigned i) {
				line l;
				l.begin = i;
				return upper_bound(lines.begin(), lines.end(), l, [](const line& x, const line& y){return x.begin < y.begin;}) - lines.begin() - 1;
			}

			text_printer::caret_info::caret_info(style default_style) : default_style(default_style), line(0), pos(0), selection_offset(0) {}

			text_printer::text_printer(fstr* source, caret_info* caret)
				: caret(caret), source(source), wrap_width(0),
				q_begin(0), q_end(0), s_begin(0), s_end(0), caret_quad(0), kerning(true), align_caret_height(true), caret_width(0),
				caret_mat(material(null_texture, pixel_32(0, 0, 0, 255))), 
				first_line_visible(0),
				last_line_visible(0),
				highlight_current_line(false),
				highlight_during_selection(false), active(false)
			{}
			
			text_printer::blinker::blinker() : blink(false), interval_ms(250), blink_func(regular_blink) {
				reset();
			}
			
			void text_printer::blinker::regular_blink(blinker& b, quad& caret) {
				for(int i = 0; i < 4; ++i)
					caret.p[i].col.a = b.caret_visible ? 255 : 0;
			}
				
			void text_printer::blinker::update(quad& caret) {
				if(timer.get_miliseconds() > interval_ms) {
					caret_visible = !caret_visible;
					timer.microseconds();
				}
				if(blink_func) blink_func(*this, caret);
			}

			void text_printer::blinker::reset() {
				timer.microseconds();
				caret_visible = true;
			}

			void text_printer::view_line(unsigned line, rect& clipper) {	
				if(!sectors.empty() && !lines.empty()) {
					const rect_ltrb& rc = clipper.get_rect_absolute();
					pointf& pen = clipper.pen;
					pointf tp = pen;
					if(line <= unsigned(first_line_visible))
						pen.y = float((lines[line].top - lines[0].top));
					else if(line >= unsigned(last_line_visible))
						pen.y = float((lines[line].top - lines[0].top) + rc.h() - lines[line].height());

					int car = min(sectors.size()-1, caret->pos);
					if(sectors[car] <= rc.l)
						pen.x = float((sectors[car] - sectors[lines[line].begin]));
					if(sectors[car] >= rc.r)
						pen.x = float((sectors[car] - sectors[lines[line].begin]) + rc.w() - 1);

					pen.y = max(pen.y, 0.f);
					pen.x = max(pen.x, 0.f);
				}
			}
			
			rect* text_printer::get_parent() {
				return p;
			}

			unsigned text_printer::map_mouse(const point& p, bool nearest) {
				if(lines.empty()) return 0;
				line l;
				l.top = p.y;
				l.desc = l.asc = 0;
				auto it = lower_bound(lines.begin() + first_line_visible, lines.begin() + last_line_visible, l, [](const line& x, const line& y){return x.top+x.height() < y.top+y.height();});
				if(it == lines.end()) --it;
				return nearest ? (*it).anchor(p.x, sectors) : (*it).hover(p.x, sectors)-1;
			}

			void text_printer::find_ascdesc(int l, int r, int& asc, int& desc) {
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

			int text_printer::get_kern(unsigned i, unsigned l) {
				if(kerning && i > lines[l].begin && getf(i) == getf(i-1)) {
					auto& vk = cached[i]->info->kerning;
					for(unsigned k = 0; k < vk.size(); ++k)
						if(vk[k].first == (*source)[i-1].c)
							return vk[k].second;
				}
				return 0;
			}

			void text_printer::update_rectangles(rect* clipper) {
				point g_pen(clipper ? clipper->get_absolute_xy() - clipper->pen + pos : pos);

				int asc, desc;
				bool crlf = false;
				unsigned line_x = g_pen.x, l = 0, i;
				
				lines.clear();
				sectors.clear();
				cached.clear();
				first_line_visible = -1;
				 last_line_visible = -1;
				if(!caret && source->empty()) return;
				cached.reserve((*source).size());

				for(unsigned i = 0; i < (*source).size(); ++i)
						cached.push_back(&getf(i)->get_glyph((*source)[i].c));
				
				lines.push_back(line());
				max_x = line_x;
				for(i = 0; i < cached.size(); ++i) {
					crlf = is_newline((*source)[i].c);
					 
					auto& g = *cached[i];

					lines[l].right = g_pen.x;
					g_pen.x += get_kern(i, l) + g.info->adv;
					bool wrap = (clipper && wrap_width > 0 && g_pen.x >= clipper->get_rect_absolute().l + wrap_width - clipper->pen.x);
					
					if(crlf || wrap) {
						lines.push_back(line());
						lines[l].wrapped = wrap;
						lines[l].end = lines[l+1].begin = i+1-wrap;
						
						g_pen.x = line_x;
						if(wrap) {
							int left = word_wrapper_separator.get_left_word(*source, lines[l].begin, i);
							if((lines[l].end - left) > lines[l].begin) {
								lines[l].end -= left;
								lines[l+1].begin -= left;
								
								for(unsigned k = lines[l+1].begin; k <= lines[l+1].begin+left; ++k)
									g_pen.x += cached[k]->info->adv + get_kern(k, l+1);
							}
							else g_pen.x += g.info->adv + get_kern(i, l);
						}
						
						++l;
					} else max_x = max(max_x, g_pen.x);
				}
				
				lines[l].wrapped = false;
				lines[l].end = i+1;

				for(l = 0; l < lines.size(); ++l) {
					g_pen.x = line_x;
					find_ascdesc(lines[l].begin, lines[l].end-1, asc, desc);
					lines[l].set(g_pen.y, asc, desc);
					if(clipper && lines[l].top + lines[l].height() >= clipper->get_clipped_rect().t && first_line_visible == -1) first_line_visible = l;
					if(clipper && lines[l].top + lines[l].height() >= clipper->get_clipped_rect().t && lines[l].top <= clipper->get_clipped_rect().b)  last_line_visible = l;

					g_pen.y += asc;

					for(i = lines[l].begin; i < lines[l].end && i < cached.size(); ++i) {
						g_pen.x += get_kern(i, l);
						sectors.push_back(g_pen.x);

						g_pen.x += cached[i]->info->adv;
					}
					g_pen.y -= desc;
				}

				sectors.push_back(g_pen.x);

				if(caret) {
					unsigned caret_line = get_line(caret->pos);

					if(align_caret_height)
						caret_rect = rect_xywh(sectors[caret->pos], lines[caret_line].top, caret_width, lines[caret_line].height());
					else {
						int pos = max(1u, caret->pos);
						caret_rect = rect_xywh(sectors[caret->pos], lines[caret_line].top + lines[caret_line].asc - getf(pos-1)->parent->ascender, 
							caret_width, getf(pos-1)->parent->ascender - getf(pos-1)->parent->descender);
					}
				}

				if(last_line_visible == -1)  last_line_visible = l;
				last_line_visible = min((unsigned)last_line_visible, lines.size()-1);
				if(first_line_visible == -1)  first_line_visible = 0;

				if(clipper) clipper->bounding_box.contain(get_bbox() - (clipper ? clipper->get_absolute_xy() - clipper->pen : point(0, 0)));
			}

			void text_printer::draw_proc(std::vector<quad>& v, rect* clipper) {
				if(lines.empty() || sectors.empty()) return;
				v.reserve(v.size() + (*source).size() + (caret != nullptr)); // one for caret
				
				bool selecting = false;
				unsigned select_left = 0, select_right = 0, caret_line = 0;
				rect_ltrb selector;
				vector<quad> selects;

				if(caret) {
					select_left = select_right = caret->pos;
					(caret->selection_offset < 0 ? select_left : select_right) += caret->selection_offset;
					caret_line = get_line(caret->pos);

					if(active && highlight_current_line)
						rect::add_quad(highlight_mat, rect_ltrb(sectors[0], lines[caret_line].top, numeric_limits<int>::max(), lines[caret_line].bottom()), clipper, selects);
				}

				q_begin = v.size();
				
				size_t i = 0, l = first_line_visible;

				if(caret && lines[l].begin > select_left && lines[l].begin < select_right) {
					selector = rect_xywh(sectors[lines[l].begin], lines[l].top, 0, lines[l].height());
					selecting = true;
				}

				for(;l <= unsigned(last_line_visible); ++l) {
					if(caret && selecting)
							selector = rect_xywh(sectors[lines[l].begin], lines[l].top, 0, lines[l].height());

					for(i = lines[l].begin; i < lines[l].end && i < cached.size(); ++i) {

						if(caret) {
							if(i == select_left && caret->selection_offset && !selecting) {
								selector = rect_xywh(sectors[i == select_left ? i : lines[l].begin], lines[l].top, 0, lines[l].height());
								selecting = true;
							}

							else if(i == select_right && selecting) {
								selector.r = sectors[i];
								rect::add_quad(active ? selection_bg_mat : selection_inactive_bg_mat, selector, clipper, selects);  
								selecting = false;
							}
						}

						auto& g = *cached[i];

						if(g.tex.get_rect().w) {
							rect::add_quad(
								material(&g.tex, 
								selecting ? selected_text_color : pixel_32((*source)[i].r, (*source)[i].g, (*source)[i].b, (*source)[i].a)), 
								rect_xywh (sectors[i] + g.info->bear_x, lines[l].top+lines[l].asc - g.info->bear_y, g.info->width, g.info->height), 
								clipper, v);
						}

					}

					if(caret && selecting) {
						selector.r = sectors[i-(l+1 != lines.size())] + (l+1 != lines.size())*cached[i-1]->info->adv;
						rect::add_quad(active ? selection_bg_mat : selection_inactive_bg_mat, selector, clipper, selects);  
						if(i == select_right)
							selecting = false;
					}
				}

				q_end = v.size();
				if(caret) {
					v.insert(v.begin() + q_begin, selects.begin() + (active && highlight_current_line && !highlight_during_selection && selects.size() > 1), selects.end());
					s_begin = q_begin;

					q_begin += selects.size();
					q_end   += selects.size();

					s_end   = q_begin;

					if(active) {
						caret_quad = q_end;
						if(rect::add_quad(caret_mat, caret_rect, clipper, v).good()) blink.update(*v.rbegin());
					} else caret_quad = -1;
				}
			}
			

			rect_ltrb text_printer::get_bbox() {
				if(sectors.empty() || lines.empty()) return rect_ltrb(0,0,0,0);
				return rect_ltrb(sectors[0], lines[0].top, max_x, (*lines.rbegin()).top + (*lines.rbegin()).height());
			}


			int text_printer::get_selection_begin() {
				return s_begin;
			} 

			int text_printer::get_selection_end() {
				return s_end;
			}

			int text_printer::get_quads_begin() {
				return q_begin;
			}
			
			int text_printer::get_quads_end() {
				return q_end;
			}

			int text_printer::get_caret_quad() {
				return caret_quad;
			}

			unsigned text_printer::get_first_line_visible() {
				return first_line_visible;
			}

			unsigned text_printer::get_last_line_visible() {
				return last_line_visible;
			}
		}
	}
}
