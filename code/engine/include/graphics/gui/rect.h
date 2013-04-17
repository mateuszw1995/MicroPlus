#pragma once
#include "system.h"

namespace db {
	namespace graphics {
		namespace gui {
			struct stylesheet;
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

				enum class appearance {
					released = 0,
					hovered = 1,
					pushed = 2,
					unknown
				};

				bool draw, clip, fetch_wheel, scrollable, snap_scroll_to_content, preserve_focus;
				point drag_origin;
				rect_ltrb rc; /* actual rectangle */ 
				rect_wh content_size; /* content's (children's) bounding box */
				pointf scroll; /* scrolls content */

				rect* parent; 

				std::vector<rect*> children;
				
				rect(const math::rect_xywh& rc = math::rect_xywh());
				
				virtual rect_wh get_content_size();
				
				void update_rectangles();
				virtual void event_proc(      event); /* event listener */
				virtual void event_proc(const info&); /* event generator */
				virtual void update_proc(system&);
				virtual void draw_proc(const draw_info&);
				virtual void on_focus(bool);

				/* event_proc default subroutines */
				void handle_scroll(event);
				void handle_middleclick(event);
				void handle_focus(event);

				/* draw_proc default subroutines */
				void draw_rect		(const draw_info& in, const material& = material()), 
					 draw_rect		(const draw_info& in, const stylesheet&),
					 draw_children	(const draw_info& in);

				static rect_ltrb add_quad(const material&, const rect_ltrb& origin, const rect* clipper, std::vector<quad>& v);
				rect_ltrb local_add(const material&, const rect_ltrb& origin, std::vector<quad>& v) const;
				
				/* how should rect look like depending on incoming event */
				static appearance get_appearance(event m); 
				
				/*  does scroll not exceed the content */
				bool is_scroll_aligned();

				/* align scroll to not exceed the content */
				void align_scroll(), 
				/* try to scroll to view whole content */    
					scroll_to_view();  
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