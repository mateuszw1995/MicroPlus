#pragma once
#include "system.h"
#include "rect.h"

namespace db {
	namespace graphics {
		namespace gui {
			struct scroll {
				float vel[2], vel_mult;
				scroll();
				void stop();
				void move(pointf&);
				void drag(const point& mouse, const rect_ltrb&);
			};

			namespace controls {
				class scrollarea : public rect {
					void update_pen_x();
					void update_pen_y();
				public:
					bool disappear_if_fits;

					virtual void event_proc(event);
					virtual void update_proc(system&);

					rect* origin;
					void align();
					bool is_needed();

					class slider : public rect {
						pointf val;
						friend class scrollarea;
					public:
						virtual void event_proc(event);

						int min_side;
						slider(int min_side, const material&);

					} *box;

					enum orientation {
						HORIZONTAL = 1,
						VERTICAL = 2,
						OMNI = (HORIZONTAL | VERTICAL)
					} flags;

					scrollarea(const math::rect_xywh& rc, const material&, rect* origin, slider* box, orientation flags);
				};
			}
		}
	}
}