#pragma once
#include "../rect.h"
#include "printer.h"
#include "draft_interface.h"
namespace db {
	namespace graphics {
		namespace gui {
			namespace text {
				struct text_rect : public rect, public draft_redrawer {
					text::printer print;
					text_rect(const rect&, const fstr& = fstr());
					
					virtual void draw_proc(const draw_info&) override;
				};
			}
		}
	}
}