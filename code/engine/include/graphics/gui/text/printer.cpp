#pragma once
#include <algorithm>
#include "ui.h"
#include "drafter.h"
#include "printer.h"
#include "../../../window/window.h"

#undef min
#undef max
namespace db {
	using namespace math;
	namespace graphics {
		using namespace io::input;
		namespace gui {
			namespace text {
				printer::printer(const rect& r) : rect(r), 
					caret_mat(material(null_texture, pixel_32(0, 0, 0, 255))),
					align_caret_height(true), caret_width(0),
					highlight_current_line(false),
					highlight_during_selection(false), active(false), 
				{
					auto& q = quad_indices;	
					q.first_character = q.last_character, 
							selections_first, selections_last,
							highlight,
							caret;
				}

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
				
				void printer::draw_text(const draw_info& in, const ui& u) const {
					draw_text(in, u.draft, u.get_str(), &u.caret);
				}
				
				void printer::draw_text(const draw_info& in, 
						const drafter& d, 
						const fstr& colors, 
						const caret_info* caret) const {
					/* shortcuts */
					auto& lines			= d.lines;
					auto& sectors		= d.sectors;
					auto& v = in.v;
					
					/* validations */
					if(lines.empty() || sectors.empty() || (clip && !get_clipped_rect().good())) return;
					
					/* reserve enough space to avoid reallocation, add one for caret */
					v.reserve(v.size() + d.cached.size() + (caret != nullptr));

					bool selecting = false;
					rect_ltrb selector;
					vector<quad> selects;

					auto visible = d.get_line_visibility(get_local_clipper());
					
					/* shouldn't happen */
					if(visible.first == -1) return;

					if(caret) {
						/* here we manage selections */
						
						unsigned select_left, select_right, select_left_line, select_right_line, caret_line;
						
						caret_line = d.get_line(caret->pos);
						select_left = caret->get_left_selection();
						select_right = caret->get_right_selection();
						select_left_line =   d.get_line(select_left);
						select_right_line =  d.get_line(select_right);

						if(select_right_line >= visible.first || select_left_line <= visible.second) {
							for(int i = visible.first; i <= visible.second; ++i) {
								if(i == select_left_line) {

								}
								else if(i == select_right_line) {

								}
							}
						}
						
						/*
						select_left_line =  max(unsigned(visible.first),  d.get_line(select_left));
						select_right_line = min(unsigned(visible.second), d.get_line(select_right));*/
						
						

						/* here we highlight the line caret is currently on */
						if(active && highlight_current_line)
							local_add(highlight_mat, rect_ltrb(0, lines[caret_line].top, rc.w(), lines[caret_line].bottom()), selects);
					
					
					}

					size_t i = 0, l = d.lines;


					/* since we're */
					if(caret && lines[l].begin > select_left && lines[l].begin < select_right) {
						selector = rect_xywh(sectors[lines[l].begin], lines[l].top, 0, lines[l].height());
						selecting = true;
					}

					/* for every visible line */
					for(;l <= unsigned(d.last_line_visible); ++l) {
						if(caret && selecting)
							selector = rect_xywh(sectors[lines[l].begin], lines[l].top, 0, lines[l].height());

						/* for every character in line */
						for(i = lines[l].begin; i < lines[l].end && i < d.cached.size(); ++i) {

							if(caret) {
								if(i == select_left && caret->selection_offset && !selecting) {
									selector = rect_xywh(sectors[i == select_left ? i : lines[l].begin], lines[l].top, 0, lines[l].height());
									selecting = true;
								}

								else if(i == select_right && selecting) {
									selector.r = sectors[i];
									rect::add_quad(d.active ? selection_bg_mat : selection_inactive_bg_mat, selector, clipper, selects);  
									selecting = false;
								}
							}

							auto& g = *d.cached[i];

							if(g.tex.get_rect().w) {
								rect::add_quad(
									material(&g.tex, 
									selecting ? selected_text_color : style(s.source[i]).color), 
									rect_xywh (sectors[i] + g.info->bear_x, lines[l].top+lines[l].asc - g.info->bear_y, g.info->size.w, g.info->size.h), 
									clipper, v);
							}

						}

						if(caret && selecting) {
							selector.r = sectors[i-(l+1 != lines.size())] + (l+1 != lines.size())*d.cached[i-1]->info->adv;
							rect::add_quad(d.active ? selection_bg_mat : selection_inactive_bg_mat, selector, clipper, selects);  
							if(i == select_right)
								selecting = false;
						}
					}

					if(caret) {

						/* IF LINE IS EMPTY (BEGIN == END) WE HAVE TO SNAP CARET'S HEIGHT TO DEFAULT STYLE!!! */

						if(align_caret_height)
							caret_rect = rect_xywh(sectors[target_caret->pos], lines[caret_line].top, caret_width, lines[caret_line].height());
						else {
							int pos = max(1u, target_caret->pos);
							caret_rect = rect_xywh(sectors[target_caret->pos], lines[caret_line].top + lines[caret_line].asc - getf(source, pos-1)->parent->ascender, 
								caret_width, getf(source, pos-1)->parent->ascender - getf(source, pos-1)->parent->descender);
						}

						v.insert(v.begin() + q_begin, selects.begin() + (d.active && d.highlight_current_line && !d.highlight_during_selection && selects.size() > 1), selects.end());
						s_begin = q_begin;

						q_begin += selects.size();
						q_end   += selects.size();

						s_end   = q_begin;

						if(d.active) {
							caret_quad = q_end;
							if(rect::add_quad(caret_mat, d.caret_rect, clipper, v).good()) blink.update(*v.rbegin());
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
				
				
				rect_xywh quick_print(std::vector<quad>& v,
										const fstr& str, 
										point pos, 
										unsigned wrapping_width,
										rect* clipper) 
				{
					printer p;
					drafter::source_info source(str, nullptr, clipper);
					p.draft.draw(source);
					p.draw_quads(source, v);
					return p.draft.get_bbox();
				}

				rect_xywh quick_print(std::vector<quad>& v,
										std::wstring& wstr,
										gui::style style,
										point pos, 
										unsigned wrapping_width,
										rect* clipper) 
				{
					fstr f = gui::formatted_text(wstr.c_str(), style);
					printer p;
					drafter::source_info source(f, nullptr, clipper);
					p.draft.draw(source);
					p.draw_quads(source, v);
					return p.draft.get_bbox();
				}
			}
			
			text_rect::text_rect(const rect& r) : printer(r), update_str(true) {}

			void text_rect::update_rectangles() {
				rect::update_rectangles();
				bounding_box.contain(draft.get_bbox());
			}

			void text_rect::draw_proc(const draw_info& in) {
				rect::draw_proc(in);
				if(update_str) {
					draft.draw(str);
					update_str = false;
				}
				draw_text(in, draft, str);
			}
		}
	}
}