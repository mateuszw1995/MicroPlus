#pragma once
#include "system.h"

namespace db {
	namespace graphics {
		namespace gui {
			struct rect {

				/* 
				index_info structure tells the index of a particular rectangle in the quad vector.

				If any child class overrides draw_proc to draw additional content, 
				it should derive index_info and quad_indices at the same names and add its own members.
				
				-1 means "not drawn" and "not in the vector"
				*/
				struct index_info {
					int background;
				} quad_indices;

				struct draw_info {
					system& owner;
					std::vector<quad>& v;

					draw_info(system&, std::vector<quad>&); 
				};

				struct info {
					system& owner;
					unsigned msg;

					mutable bool mouse_fetched;
					mutable bool scroll_fetched;
					info(system&, unsigned); 
				} const *in;

				struct solid_stroke {
					struct border {
						int width;
						material mat;
					} left, top, right, bottom;

					enum type {
						INSIDE, OUTSIDE
					} _type;

					solid_stroke(int width = 1, const material& = material(), type = OUTSIDE);

					void set_width(int);
					void set_material(const material&);
					void draw(const rect&, const draw_info&) const;
				};

				enum class event {
					keydown,
					keyup,
					character,
					unichar,
					wheel,

					lpressed,
					rpressed,
					ldown,
					mdown,
					rdown,
					lclick,
					rclick,
					ldoubleclick,
					ltripleclick,
					mdoubleclick,
					rdoubleclick,
					lup,
					mup,
					rup,
					loutup,
					routup,
					hover,
					hout,
					ldrag,
					loutdrag
				};

				bool draw, clip, fetch_wheel, scrollable, snap_scroll_to_bbox;
				point drag_origin;
				rect_ltrb rc, /* actual rectangle */ 
					bounding_box; /* content's (children) bounding box */
				pointf pen; /* scrolls content */

				material mat;
				rect* parent; 

				std::vector<rect*> children;
				
				rect(const math::rect_xywh& rc = math::rect_xywh(), const material& = material());
				
				virtual rect_ltrb get_bounding_box() const;
				virtual void update_rectangles();
				virtual void event_proc(      event); /* event listener */
				virtual void event_proc(const info&); /* event generator */
				virtual void update_proc(system&);
				virtual void draw_proc(const draw_info&);
				virtual void on_focus(bool);
				void draw_rect(const draw_info& in), 
					 draw_children(const draw_info& in);

				static rect_ltrb add_quad(const material&, const rect_ltrb& origin, const rect* clipper, std::vector<quad>& v);
				rect_ltrb local_add(const material&, const rect_ltrb& origin, std::vector<quad>& v) const;

				bool is_pen_aligned(); /*   is pen valid */
				void align_pen(),      /* make pen valid */
					scroll_to_view();  /* scroll to view whole content */
				const rect_ltrb& get_clipped_rect() const;
				rect_ltrb get_rect_absolute() const;
				const point& get_absolute_xy() const;
				rect_ltrb get_local_clipper() const;

			private:
				rect_ltrb rc_clipped;
				point absolute_xy;

				bool was_hovered;
			};
		}
	}
}