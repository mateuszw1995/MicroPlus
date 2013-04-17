#pragma once
#include "rect.h"
#include "text/draft_interface.h"
#include "text/printer.h"

namespace db {
	namespace graphics {
		namespace gui {
			namespace controls {
				class check_interface {
				protected:
					bool set;
				public:
					check_interface(bool set);

					virtual void on_change(bool set) = 0;

					bool get_state() const;
					operator bool()  const;

					void set_state(bool);
				};

				struct checkbox : public rect, public check_interface {
					checkbox(bool set, const rect&);
					
					virtual void event_proc(event e) override;
				
				protected:
					using check_interface::set;
				};
				

				class checklabel : public rect, public check_interface {
					void stretch_rc();

				public:
					text::draft_redrawer active_text;
					text::draft_redrawer inactive_text;

					text::printer print;

					checklabel(bool set, const math::rect_xywh& rc, const wstring& label, const text::style& style_active, const text::style& style_inactive);
					checklabel(bool set, const math::rect_xywh& rc, const text::fstr& active_str, const text::fstr& inactive_str);

					virtual void event_proc(event e) override;
					virtual void on_change(bool set) override;
					
					void draw_label(const draw_info&);

					text::draft_redrawer& active_draft();
				protected:
					using check_interface::set;
				};
			}
		}
	}
}