#pragma once
#include "system.h"
#include "rect.h"
#include "../../window/window.h"
#include <GL/GL.h>
#undef max
namespace db {
	using namespace window;
	using namespace math;
	namespace misc {
		std::wstring wstr(const graphics::gui::fstr& f) {
			size_t l = f.size();
			std::wstring ww;
			ww.reserve(l);
			for(size_t i = 0; i < l; ++i)
				ww += f[i].c;

			return ww;
		}
	}

	namespace graphics {
		using namespace io::input;
		namespace gui {
			io::input::texture* null_texture = 0;
			
			void formatted_text(const wchar_t* _str, fstr& out, style s) {
				out.clear();
				formatted_char ch;
				int len = wcslen(_str);
				for(int i = 0; i < len; ++i) {
					ch.set(_str[i], s.f, s.color);
					out.append(1, ch);
				}
			}

			fstr formatted_text(const wchar_t* _str, style s) {
				fstr out;	

				formatted_char ch;
				ch.font_used = s.f;
				int len = wcslen(_str);

				//out.reserve(len);
				for(int i = 0; i < len; ++i) {
					ch.set(_str[i], s.f, s.color);
					out.append(1, ch);
				}

				return out;
			}

			void scale_virtual_res(rect_wh vres, rect_wh display, vector<quad>& quads) {
				float x_mult = display.w/float(vres.w);
				float y_mult = display.h/float(vres.h);

				for(size_t i = 0; i < quads.size(); ++i) {
					for(int q = 0; q < 4; ++q) {
						quads[i].p[q].x = int(float(quads[i].p[q].x)*x_mult);
						quads[i].p[q].y = int(float(quads[i].p[q].y)*y_mult);
					}
				}
			}

			material::material(io::input::texture* tex, const pixel_32& color) : tex(tex), color(color) {}

			quad::quad() {}
			quad::quad(const rect_ltrb& rc, const material& mat, const rect_texture& t) {
				p[0].x = p[3].x = rc.l;
				p[0].y = p[1].y = rc.t;
				p[1].x = p[2].x = rc.r;
				p[2].y = p[3].y = rc.b;

				p[0].col = p[1].col = p[2].col = p[3].col = mat.color;

				mat.tex->get_uv(t.u1, t.v1, p[0].u, p[0].v);
				mat.tex->get_uv(t.u2, t.v1, p[1].u, p[1].v);
				mat.tex->get_uv(t.u2, t.v2, p[2].u, p[2].v);
				mat.tex->get_uv(t.u1, t.v2, p[3].u, p[3].v);
			}
				
			quad quad::clipped(const rect_ltrb& origin, const rect_ltrb& c, const material& mat) {
				rect_ltrb rc = origin;
				rc.clip(c);

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

				return q;
			}
			
			void quad::move(const point& v) {
				for(int i = 0; i < 4; ++i) {
					p[i].x += v.x;
					p[i].y += v.y;
				}
			}

			void quad::set(const rect_ltrb& rc) {
				p[0].x = p[3].x = rc.l;
				p[0].y = p[1].y = rc.t;
				p[1].x = p[2].x = rc.r;
				p[2].y = p[3].y = rc.b;
			}

			void quad::rotate90(int times) {
				for(int i = 0; i < times; ++i) {
					auto tmp = p[3];
					p[3].u = p[2].u;
					p[3].v = p[2].v;
					p[2].u = p[1].u;
					p[2].v = p[1].v;
					p[1].u = p[0].u;
					p[1].v = p[0].v;
					p[0].u = tmp.u;
					p[0].v = tmp.v;
				}
			}
			
			rect_ltrb quad::get_rect() const {
				return rect_ltrb(p[0].x, p[0].y, p[2].x, p[2].y);
			}

			void formatted_char::set(wchar_t ch, font* f, const pixel_32& p) {
				font_used = f;
				c = ch;
				memcpy(&r, &p, sizeof(pixel_32));
			}
			
			void formatted_char::set(font* f, const pixel_32& p) {
				font_used = f;
				memcpy(&r, &p, sizeof(pixel_32));
			}
			
			style::style(font* f, pixel_32 c) : f(f), color(c) {}
			
			style::style(const formatted_char& c) : f(c.font_used), color(pixel_32(c.r, c.g, c.b, c.a)) {}

			style::operator formatted_char() {
				formatted_char c;
				c.set(f, color);
				return c;
			}

			system::system(event::state* events) : events(events), own_copy(false), own_clip(false), fetch_clipboard(true), focus(0), lholded(0), rholded(0) {
				middlescroll.subject = 0;
				middlescroll.speed_mult = 1.f;
				middlescroll.size = rect_wh(25, 25);
			}

			bool system::is_clipboard_own() {
				return own_clip;
			}
			
			void system::set_focus(rect* f) {
				if(focus) focus->on_focus(false);
				focus = f;
				f->on_focus(true);
			}
			
			rect* system::get_focus() const {
				return focus;
			}

			void system::bind() {
				glVertexPointer(2, GL_INT, sizeof(vertex), rect_array.data());
				glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (char*)(rect_array.data()) + sizeof(int)*2);
				glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(vertex), (char*)(rect_array.data()) + sizeof(int)*2 + sizeof(float)*2);
			}
			
			void system::default_update() {
				update_rectangles();
				call_updaters();
				update_rectangles();
				update_array();
			}

			void system::update_array() {
				rect_array.clear();
				rect::draw_info in(*this, rect_array);

				for(size_t i = 0; i < windows.size(); ++i) {
					windows[i]->parent = 0;
					windows[i]->draw_proc(in);
				}
				
				if(middlescroll.subject) {
					rect_ltrb scroller = rect_wh(middlescroll.size);
					scroller.center(middlescroll.pos);
					rect::add_quad(middlescroll.mat, scroller, 0, rect_array); 
				}
			}
			
			void system::update_rectangles() {
				for(size_t i = 0; i < windows.size(); ++i) {
					windows[i]->parent = 0;
					windows[i]->update_rectangles();
				}
			}

			void system::call_updaters() {
				for(size_t i = 0; i < windows.size(); ++i) {
					windows[i]->parent = 0;
					windows[i]->update_proc(*this);
				}

				if(middlescroll.subject) {
					point tempp = middlescroll.subject->pen; 
					middlescroll.subject->pen += (events->mouse.pos - middlescroll.pos) * float(middlescroll.speed_mult*fps->frame_speed());
				}
			}

			void paste_clipboard(fstr& out, formatted_char f) {
				std::wstring w;
				window::paste_clipboard(w);
				size_t len = w.length();
				out.clear();
				out.reserve(len);
				for(size_t i = 0; i < len; ++i) {
					f.c = w[i];
					out += f;
				}
			}

			void system::copy_clipboard(fstr& s) {
				clipboard = s;
				own_copy = true;
				own_clip = true;
				window::copy_clipboard(misc::wstr(s));
			}

			void system::poll_events() {
				event::state& gl = *events;
				using namespace event::key;
				rect::info in(*this, gl.msg);
				bool pass = true;
				
				if((gl.msg == event::mouse::mdown || gl.msg == event::mouse::mdoubleclick) && middlescroll.subject) {
					pass = false;
					middlescroll.subject = 0;
				}

				if(gl.msg == event::mouse::lup) {
					if(lholded) {
						lholded->in = &in;
						if(lholded->get_clipped_rect().hover(gl.mouse.pos)) {
							lholded->event_proc(rect::event::lup);
							lholded->event_proc(rect::event::lclick);
							pass = false;
						}
						else
							lholded->event_proc(rect::event::loutup);
						lholded = 0;
					}
				} 
				
				if(gl.msg == event::mouse::rup) {
					if(rholded) {
						rholded->in = &in;
						if(rholded->get_clipped_rect().hover(gl.mouse.pos)) {
							rholded->event_proc(rect::event::rup);
							rholded->event_proc(rect::event::rclick);
							pass = false;
						}
						else
							rholded->event_proc(rect::event::routup);
						rholded = 0;
					}
				} 

				if(focus && (gl.msg == down || 
				   gl.msg == up   || 
				   gl.msg == character || 
				   gl.msg == unichar || 
				   (focus->fetch_wheel && gl.msg == event::mouse::wheel))) {
						focus->event_proc(in);
				}
				else if(gl.msg == event::clipboard_change) {
					if(!own_copy && fetch_clipboard) {
						formatted_char ch;
						ch.set(0, 0, pixel_32(0, 0, 0, 255));
						paste_clipboard(clipboard, ch);
						own_clip = false;
					}

					own_copy = false;
				}
				else if(pass) {
					for(int i = windows.size() - 1; i >= 0; --i) {
						windows[i]->parent = 0;
						windows[i]->event_proc(in);
					}
				}
			}
		}
	}
}