#pragma once
#include "dragger.h"
#include "rect.h"
#include "text\printer.h"
#include "text\ui.h"
#include "../../misc/undoredo.h"
namespace db {
	namespace graphics {
		namespace gui {
			namespace controls {
				class textbox : public rect {
					point local_mouse();
				public:
					virtual void on_caret_left(bool select);
					virtual void on_caret_right(bool select);
					virtual void on_caret_left_word(bool select);
					virtual void on_caret_right_word(bool select);
					virtual void on_caret_up(bool select);
					virtual void on_caret_down(bool select);
					virtual void on_caret_ctrl_up();
					virtual void on_caret_ctrl_down();
					virtual void on_place_caret(bool select);
					virtual void on_select_word();
					virtual void on_select_line();
					virtual void on_select_all();
					virtual void on_home(bool select);
					virtual void on_end(bool select);
					virtual void on_pagedown();
					virtual void on_pageup();
					virtual void on_character();
					virtual void on_cut();
					virtual void on_bold();
					virtual void on_italics();
					virtual void on_copy();
					virtual void on_paste();
					virtual void on_undo();
					virtual void on_redo();
					virtual void on_backspace(bool);
					virtual void on_del(bool);
					virtual void on_drag();

					virtual rect_wh get_content_size() override;
					virtual void event_proc(rect::event) override;
					virtual void update_proc(system&) override;
					virtual void on_focus(bool) override;

					void draw_text_ui(const draw_info&);
					void handle_interface(event);

					bool view_caret, blink_reset;
					dragger drag;
					
					text::ui editor;
					text::printer print;

					textbox(const rect&, text::style default_style);
				};
			}
		}
	}
}