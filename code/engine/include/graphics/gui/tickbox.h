#pragma once
#include "rect.h"
#include "text/drafter.h"
#include "text/printer.h"

namespace db {
	namespace graphics {
		namespace gui {
			namespace controls {
				class tickbox : public rect {
					bool set;
				public:
					material active, inactive;

					tickbox(bool& set, const math::rect_xywh& rc, const material& active = material(), const material& inactive = material());
					
					void event_proc(event e);
					void draw_proc(const draw_info&);

					virtual void on_change(bool set) = 0;

					bool get_state() const;
					void set_state(bool);
				};

				
				class label_switcher : public tickbox {
					fstr active_str, inactive_str;
					text::drafter draft; 
					text::printer print;

					using tickbox::set_state;
					bool stretch_rect_to_text;
				public:
					const text::drafter& get_draft();

					label_switcher(const tickbox&, bool stretch_rect_to_text, const fstr& active_str, const fstr& inactive_str);

					virtual void draw_proc(const draw_info&);
					void set_state(bool);
				};
			}
		}
	}
}