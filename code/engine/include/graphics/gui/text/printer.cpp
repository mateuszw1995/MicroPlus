#pragma once
#include <algorithm>
#include "printer.h"
#include "../../window/window.h"

#undef min
#undef max
namespace db {
	using namespace math;
	namespace graphics {
		using namespace io::input;
		namespace gui {
			namespace text {
				printer::printer(drafter* draft) : draft(draft),
					q_begin(0), q_end(0), s_begin(0), s_end(0), caret_quad(0),
					caret_mat(material(null_texture, pixel_32(0, 0, 0, 255))), 
				{}

				printer::blinker::blinker() : blink(false), interval_ms(250), blink_func(regular_blink) {
					reset();
				}

				void printer::blinker::regular_blink(blinker& b, quad& caret) {
					for(int i = 0; i < 4; ++i)
						caret.p[i].col.a = b.caret_visible ? 255 : 0;
				}

				void printer::blinker::update(quad& caret) {
					if(timer.get_miliseconds() > interval_ms) {
						caret_visible = !caret_visible;
						timer.microseconds();
					}
					if(blink_func) blink_func(*this, caret);
				}

				void printer::blinker::reset() {
					timer.microseconds();
					caret_visible = true;
				}

				void printer::draw_quads(std::vector<quad>& v, rect* clipper) {
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

				int printer::get_selection_begin() {
					return s_begin;
				} 

				int printer::get_selection_end() {
					return s_end;
				}

				int printer::get_quads_begin() {
					return q_begin;
				}

				int printer::get_quads_end() {
					return q_end;
				}

				int printer::get_caret_quad() {
					return caret_quad;
				}
			}
		}
	}
}