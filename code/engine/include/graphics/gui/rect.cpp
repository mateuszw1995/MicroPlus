#pragma once
#include "rect.h"

namespace db {
	namespace graphics {
		namespace gui {
			rect::solid_stroke::solid_stroke(int width, const material& mat, type _type) : _type(_type) {
				left.width = width; left.mat = mat;
				right = top = bottom = left;
			}

			void rect::solid_stroke::set_width(int w) {
				left.width = top.width = right.width = bottom.width = w; 
			}

			void rect::solid_stroke::set_material(const material& mat) {
				left.mat = top.mat = right.mat = bottom.mat = mat; 
			}

			void rect::solid_stroke::draw(const rect& r, const draw_info& in) const {
				rect_ltrb g = r.get_rect_absolute();

				if(_type == OUTSIDE) {
					g.l -= left.width;
					g.t -= top.width;
					g.r += right.width;
					g.b += bottom.width;
				}

				rect_ltrb lines[4] = { g, g, g, g }; 

				lines[0].r = g.l + left.width;
				lines[1].b = g.t + top.width;
				lines[2].l = g.r - right.width;
				lines[3].t = g.b - bottom.width;

				if(top.width) {
					lines[0].t += top.width;
					lines[2].t += top.width;
				}
				if(bottom.width) {
					lines[0].b -= top.width;
					lines[2].b -= top.width;
				}

				add_quad(left.  mat, lines[0], r.parent, in.v);
				add_quad(top.   mat, lines[1], r.parent, in.v);
				add_quad(right. mat, lines[2], r.parent, in.v);
				add_quad(bottom.mat, lines[3], r.parent, in.v);
			}
			
			rect::info::info(system& owner, unsigned msg) : owner(owner), msg(msg), mouse_fetched(false), scroll_fetched(false) {}
			rect::draw_info::draw_info(system& owner, std::vector<quad>& v) : owner(owner), v(v) {}

			rect::rect(const rect_xywh& rc, const material& mat) 
				: rc(rc), mat(mat), parent(0), 
				draw(true), q(0), was_hovered(false), fetch_wheel(false), clip(true), scrollable(true), bounding_box(rect_xywh()), rc_clipped(rect_xywh()) {
			}
			
			void rect::update_rectangles() {
				/* init later to be processed absolute and clipped with local rc */
				absolute_xy = rc_clipped = rc;

				/* if we've got parent */
				if(parent) {
					/* we have to save our global coordinates in absolute_xy */
					absolute_xy = parent->absolute_xy + point(rc) - parent->pen;
					/* and we have to clip by parent's rc */
 					rc_clipped  = rect_xywh(absolute_xy.x, absolute_xy.y, rc.w(), rc.h());
				}
					
				/* init the bounding box */
				bounding_box = rect_ltrb();
				
				/* enlarge the bounding box by every children */
				for(size_t i = 0; i < children.size(); ++i)
					if(children[i]->draw)
				 		bounding_box.contain(children[i]->rc);

				/* do the same for every children */
				for(size_t i = 0; i < children.size(); ++i) {
					children[i]->parent = this;
					if(children[i]->draw)
					   children[i]->update_rectangles();
				}
			}
			
			void rect::draw_rect(const draw_info& in) {
				q = in.v.size();
				rc_clipped = add_quad(mat, rc_clipped, parent, in.v);
			}

			void rect::draw_children(const draw_info& in) {
				for(size_t i = 0; i < children.size(); ++i) { 
					children[i]->parent = this;
					if(children[i]->draw)
					   children[i]->draw_proc(in);
				}
			}
			
			void rect::update_proc(system& owner) {
				for(size_t i = 0; i < children.size(); ++i) { 
					children[i]->parent = this;
					children[i]->update_proc(owner);
				}
			}

			void rect::draw_proc(const draw_info& in) {
				draw_rect(in);
				draw_children(in);
			}
			
			/* handle focus and passing scroll to parents */
			void rect::event_proc(event e) {
				auto& sys =	 in->owner;
				auto& wnd = *sys.events;
				if(e == event::mdown || e == event::mdoubleclick) {
					if(!bounding_box.inside(rect_wh(rc))) {
						sys.middlescroll.subject = this;
						sys.middlescroll.pos = wnd.mouse.pos;
						sys.set_focus(this);
					} else if(parent) {
						parent->event_proc(e);
					}
				}
				if(e == event::ldown ||
					e == event::ldoubleclick ||
					e == event::ltripleclick ||
					e == event::rdoubleclick ||
					e == event::rdown
					) {
						sys.set_focus(this);
				}
				if(e == event::wheel) {
					if(wnd.keys[db::event::keys::SHIFT]) {
						int temp(int(pen.x));
						if(scrollable) {
							pen.x -= wnd.mouse.scroll;
							align_pen();
						}
						if((!scrollable || temp == pen.x) && parent) {
							parent->event_proc(event::wheel);
						}
					}
					else {
						int temp(int(pen.y));
						if(scrollable) {
							pen.y -= wnd.mouse.scroll;
							align_pen();
						}
						if((!scrollable || temp == pen.y) && parent) {
							parent->event_proc(event::wheel);
						}
					}
				}
			}

			void rect::on_focus(bool) { 

			}

			void rect::event_proc(const info& inf) {
				using namespace db::event;
				using namespace mouse;
				auto& sys = inf.owner;
				auto& m = sys.events->mouse;
				unsigned msg = inf.msg;
				in = &inf;

				if(draw) {
					for(int i = children.size()-1; i >= 0; --i) {
						children[i]->parent = this;
						children[i]->event_proc(inf);
					}

					if(msg == key::down) event_proc(event::keydown); 
					else if(msg == key::up) event_proc(event::keyup); 
					else if(msg == key::character) event_proc(event::character); 
					else if(msg == key::unichar) event_proc(event::unichar); 
					else {
						bool hover = rc_clipped.hover(m.pos);

						if(hover && !inf.mouse_fetched) {
							inf.mouse_fetched = was_hovered = true;
							event_proc(event::hover);
							if(msg == lup) {
								event_proc(event::lup);	
							}
							if(msg == ldown) {
								sys.lholded = this;
								event_proc(event::ldown);	
							}
							if(msg == mdown) {
								event_proc(event::mdown);	
							}
							if(msg == mdoubleclick) {
								event_proc(event::mdoubleclick);	
							}
							if(msg == ldoubleclick) {
								sys.lholded = this;
								event_proc(event::ldoubleclick);	
							}
							if(msg == ltripleclick) {
								sys.lholded = this;
								event_proc(event::ltripleclick);	
							}
							if(msg == rdown) {
								sys.rholded = this;
								event_proc(event::rdown);
							}
							if(msg == rdoubleclick) {
								sys.rholded = this;
								event_proc(event::rdoubleclick);
							}

							if(msg == wheel) {
								event_proc(event::wheel);
							}

							if(sys.lholded == this && msg == motion && m.state[0] && rc_clipped.hover(m.ldrag)) {
								sys.lholded = this;
								event_proc(event::lpressed);
							}
							if(sys.rholded == this && msg == motion && m.state[1] && rc_clipped.hover(m.rdrag)) {
								sys.rholded = this;
								event_proc(event::rpressed);
							}
						}
						else if(hover) {

						}
						else if(!hover) {
							if(msg == motion) {
								if(was_hovered) {
									event_proc(event::hout);
									was_hovered = false;
								}
								if(sys.lholded == this) {
									event_proc(event::loutdrag);
								}
							}
						}
						if(sys.lholded == this && msg == motion) {
							event_proc(event::ldrag);
						}
					}
					if(sys.lholded != this) drag_origin = point(rc.l, rc.t);
				}
			}
			
			bool rect::is_pen_aligned() {
				return rect_wh(rc).is_sticked(bounding_box, pen);
			}
			
			void rect::align_pen() {
				rect_wh(rc).stick_relative(bounding_box, pen);
			}

			void rect::scroll_to_view() {
				if(parent) {
					rect_ltrb global = get_rect_absolute();
					rect_ltrb parent_global = parent->get_rect_absolute();
					point off1 = point(max(0, global.r + 2 - parent_global.r), max(0, global.b + 2 - parent_global.b));
					point off2 = point(max(0, parent_global.l - global.l + 2 + off1.x), max(0, parent_global.t - global.t + 2 + off1.y));
					parent->pen += off1;
					parent->pen -= off2;
					parent->scroll_to_view();
				}
			}

			rect_ltrb rect::add_quad(const material& mat, const rect_ltrb& origin, const rect* p, std::vector<quad>& v) {
				rect_ltrb rc = origin;
				if(p && p->clip && !rc.clip(p->rc_clipped)) return rc;

				static rect_texture diff;
				static float tw, th;
				static quad q;

				tw = 1.0f / origin.w();
				th = 1.0f / origin.h();

				diff = rect_texture(((q.p[0].x = q.p[3].x = rc.l) - origin.l) * tw,
					((q.p[0].y = q.p[1].y = rc.t) - origin.t) * th,
					((q.p[1].x = q.p[2].x = rc.r) - origin.r) * tw + 1.0f,
					((q.p[2].y = q.p[3].y = rc.b) - origin.b) * th + 1.0f);

				q.p[0].col = q.p[1].col = q.p[2].col = q.p[3].col = mat.color;

				mat.tex->get_uv(diff.u1, diff.v1, q.p[0].u,  q.p[0].v);
				mat.tex->get_uv(diff.u2, diff.v1, q.p[1].u,  q.p[1].v);
				mat.tex->get_uv(diff.u2, diff.v2, q.p[2].u,  q.p[2].v);
				mat.tex->get_uv(diff.u1, diff.v2, q.p[3].u,  q.p[3].v);

				v.push_back(q);
				return rc;
			}
			
			const rect_ltrb& rect::get_clipped_rect() const {
				return rc_clipped;
			}
			
			rect_ltrb rect::get_rect_absolute() const {
				return rect_xywh(absolute_xy.x, absolute_xy.y, rc.w(), rc.h());
			}

			const point& rect::get_absolute_xy() const {
				return absolute_xy;
			}

			int rect::get_quad() const {
				return q;
			}
		}
	}
}
