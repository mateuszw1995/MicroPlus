#pragma once
#include "scroll.h"

namespace db {
	namespace graphics {
		namespace gui {
			class text_interface;
			namespace controls {
				/* processes events and passes them to text* print, implementing the actual GUI for text class */
				struct textbox : public rect {
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
					
					virtual void event_proc(rect::event);
					virtual void update_proc(system&);
					virtual void on_focus(bool);

					bool view_caret, blink_reset;
					text_interface* editor;
					scroll scroller;

					textbox(const math::rect_xywh& rc, const material&, text_interface*);
				};
			}
		}
	}
}