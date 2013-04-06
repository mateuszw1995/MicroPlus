#pragma once
#include "scrollarea.h"

namespace db {
	namespace graphics {
		namespace gui {
			namespace controls {
				scrollarea::scrollarea(const math::rect_xywh& rc, const material& mat, rect* origin, slider* box, orientation flags)
					: rect(rc, mat), origin(origin), box(box), flags(flags), disappear_if_fits(true) {
						children.push_back(box);
				}

				scrollarea::slider::slider(int min_side, const material& mat) : rect(rc, mat), min_side(min_side) {}

				bool scrollarea::is_needed() {
					bool need[2] = {
						origin && origin->bounding_box.r > origin->rc.w(),
						origin && origin->bounding_box.b > origin->rc.h()
					};

					return  (flags == orientation::HORIZONTAL && need[0]) ||
						(flags == orientation::VERTICAL   && need[1]) ||
						(flags == orientation::OMNI       && (need[0]  || need[1]));
				}

				void scrollarea::update_proc(system& inf) {
					bool n = is_needed();
					box->draw = draw = !(disappear_if_fits && !n);
					rect_ltrb& sl = box->rc;

					if(flags & orientation::HORIZONTAL) {
						sl.stick_x(rc);
						if(!n) {
							sl.l = rc.l;
							sl.r = rc.r;
						}
					}

					if(flags & orientation::VERTICAL) {
						sl.stick_y(rc);
						if(!n) {
							sl.t = rc.t;
							sl.b = rc.b;
						}
					}

					if(!n) return;

					if(flags & orientation::HORIZONTAL) {
						int width = std::max(box->min_side, int(rc.w() * origin->rc.w() / origin->bounding_box.r));
						sl.l = int((rc.w() - width) * origin->pen.x / (origin->bounding_box.r - origin->rc.w()));
						sl.t = 0;
						sl.w   (width);
						sl.h   (rc.h());
					}
					
					if(flags & orientation::VERTICAL) {
						int height = std::max(box->min_side, int(rc.h() * origin->rc.h() / origin->bounding_box.b));
						sl.t = int((rc.h() - height) * origin->pen.y / (origin->bounding_box.b - origin->rc.h()));
						sl.l = 0;
						sl.h   (height);
						sl.w   (rc.w());
					}

					rect::update_proc(inf);
				}


				void scrollarea::update_pen_x() {
					origin->pen.x = float(box->rc.l * (origin->bounding_box.r - origin->rc.w()) / (rc.w() - box->rc.w()));
				}

				void scrollarea::update_pen_y() {
					origin->pen.y = float(box->rc.t * (origin->bounding_box.b - origin->rc.h()) / (rc.h() - box->rc.h()));
				}

				void scrollarea::event_proc(event e) {
					if(!is_needed()) return;
					auto& sys = in->owner;
					auto& wnd = *sys.events;
					if(e == event::ldown && box) {
						sys.lholded = box;
						if(flags & orientation::HORIZONTAL) {
							box->rc.center_x(wnd.mouse.pos.x - get_rect_absolute().l);
							update_pen_x();
						}
						if(flags & orientation::VERTICAL) {
							box->rc.center_y(wnd.mouse.pos.y - get_rect_absolute().t);
							update_pen_y();
						}
						box->drag_origin = point(box->rc.l, box->rc.t);
					}
				}

				void scrollarea::slider::event_proc(event e) {
					auto& ms = in->owner.events->mouse;
					scrollarea* pp = (scrollarea*)parent;
					if(!pp->is_needed()) return;
					if(e == event::ldrag) {
						if(pp->flags & orientation::HORIZONTAL) {
							rc.x(drag_origin.x + ms.pos.x - ms.ldrag.x);
							pp->update_pen_x();
						}
						if(pp->flags & orientation::VERTICAL  ) {
							rc.y(drag_origin.y + ms.pos.y - ms.ldrag.y);
							pp->update_pen_y();
						}
					}
				}

				void scrollarea::align() {
					if(flags & orientation::HORIZONTAL) {
						rc.x(origin->rc.l);
						rc.y(origin->rc.b);
						rc.w(origin->rc.w());
					}
					if(flags & orientation::VERTICAL) {
						rc.x(origin->rc.r);
						rc.y(origin->rc.t);
						rc.h(origin->rc.h());
					}
				}
			}
		}
	}
}