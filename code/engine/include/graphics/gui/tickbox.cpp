#pragma once
#include "tickbox.h"

namespace db {
	namespace graphics {
		namespace gui {
			namespace controls {
				tickbox::tickbox(bool set, const math::rect_xywh& rc, const material& active, const material& inactive) :
					active(active), inactive(inactive), set(set), rect(rc, set ? active : inactive) {
				}

				void tickbox::event_proc(event e) {
					if(e == rect::event::lclick)
						set_state(!set);
				}
					
				void tickbox::draw_proc(const draw_info& in) {
					mat = set ? active : inactive;
					rect::draw_proc(in);
				}
					
				bool tickbox::get_state() const {
					return set;
				}
					
				void tickbox::set_state(bool) {
						set = !set;
						this->on_change(set);
				}
				


				const text::drafter& get_draft();

				label_switcher::label_switcher(const tickbox& t, bool stretch_rect_to_text, const fstr& active_str, const fstr& inactive_str) :
					tickbox(t), active_str(active_str), inactive_str(inactive_str), stretch_rect_to_text(stretch_rect_to_text) {
				}

				void label_switcher::draw_proc(const draw_info& in) {
					rect::draw_proc(in);
					draw_text(in.v, draft, _str);
				}
			
				void label_switcher::set_state(bool flag) {
					tickbox::set_state(flag);
					draft.draw(get_state() ? active_str : inactive_str);
					if(stretch_rect_to_text) {
						rect_wh size = draft.get_bbox();
						rc.w = size.w;
						rc.h = size.h;
					}
				}
			}
		}
	}
}