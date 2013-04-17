#include "text_rect.h"

namespace db {
	using namespace math;
	namespace graphics {
		using namespace io::input;
		namespace gui {
			namespace text {
				text_rect::text_rect(const rect& r, const fstr& _str) : rect(r), draft_redrawer(_str) {}

				void text_rect::draw_proc(const draw_info& in) {
					guarded_redraw();
					print.draw_text(in.v, get_draft(), get_str(), 0, *this);
				}
			}
		}
	}
}