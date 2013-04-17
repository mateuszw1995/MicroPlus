#pragma once
#include "checkbox.h"

namespace db {
	namespace graphics {
		namespace gui {
			namespace controls {
				check_interface::check_interface(bool set) : set(set) {}


				bool check_interface::get_state() const {
					return set;
				}

				void check_interface::set_state(bool flag) {
					if(flag != set) {
						set = flag;
						on_change(set);
					}
				}

				check_interface::operator bool() const {
					return get_state();
				}

				checkbox::checkbox(bool set, const rect& rc)
					: check_interface(set), rect(rc)
				{}

				void checkbox::event_proc(event e) {
					if(e == rect::event::lclick)
						set_state(!get_state());
					rect::event_proc(e);
				}
					
				checklabel::checklabel(bool set, const math::rect_xywh& rc, const wstring& label, const text::style& style_active, const text::style& style_inactive) 
					: check_interface(set), rect(rc), active_text(text::format(label, style_active)), inactive_text(text::format(label, style_inactive)) {
					stretch_rc();
				}

				checklabel::checklabel(bool set, const math::rect_xywh& rc, const text::fstr& active_str, const text::fstr& inactive_str) 
					: check_interface(set), rect(rc), active_text(active_str), inactive_text(inactive_str) {
					stretch_rc();
				}
					
				void checklabel::event_proc(event e) {
					if(e == rect::event::lclick)
						set_state(!get_state());
					rect::event_proc(e);
				}

				void checklabel::draw_label(const draw_info& in) {
					active_draft().guarded_redraw();
					print.draw_text(in.v, active_draft().get_draft(), active_draft().get_str(), 0, *this); 
				}

				void checklabel::on_change(bool set) {
					stretch_rc();
				}
				
				void checklabel::stretch_rc() {
					active_draft().guarded_redraw();
					auto size = active_draft().get_draft().get_bbox();
					rc.w(size.w);
					rc.h(size.h);
				}

				text::draft_redrawer& checklabel::active_draft() {
					return get_state() ? active_text : inactive_text;
				}
			}
		}
	}
}