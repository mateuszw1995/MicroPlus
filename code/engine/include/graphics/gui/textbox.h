#pragma once
#include "dragger.h"
#include "text\printer.h"
#include "text\ui.h"
#include "../../misc/undoredo.h"
namespace db {
	namespace graphics {
		namespace gui {
			namespace controls {
				class textbox : public text::printer, public text::ui {
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

					virtual rect_wh get_content_size();
					virtual void event_proc(rect::event);
					virtual void draw_proc(const draw_info&);
					virtual void update_proc(system&);
					virtual void on_focus(bool);

					bool view_caret, blink_reset;
					dragger drag;

					textbox(const rect&, style default_style);
				};
			}
		}
	}
}