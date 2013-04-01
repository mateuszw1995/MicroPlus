#pragma once
#include "scroll.h"
#include "text\printer.h"
#include "../../misc/undoredo.h"
// should change text positioning to local by clipper
namespace db {
	namespace graphics {
		namespace gui {
			namespace controls {
				/* processes events and passes them to text* print, implementing the actual GUI for text class */
				class textbox : public rect {
					/* Undo/Redo interface implementation to satisfy the template, you shouldn't care */
					struct action {
						textbox* subject;

						struct memento {
							int caret_pos, sel_offset;
						} un, re;

						int where, right;
						formatted_char character;
						fstr _str, replaced;

						vector<bool> states;
						bool unapply;

						enum type {
							NONE, CHARACTERS, INSERT, ERASE, REPLACE, REPLACE_CHARACTERS, BOLDEN, ITALICSEN
						} flag;

						action(textbox& subject, int where, const formatted_char&);
						action(textbox& subject, int where, const formatted_char&, const fstr& replaced);
						action(textbox& subject, int where, const fstr&, type flag = INSERT);
						action(textbox& subject, int where, const fstr&, const fstr& replaced = fstr());
						action(textbox& subject, int left, int right, bool unapply, vector<bool>&, type flag);

						bool include(const action&);
						void execute(bool undo);
						void set_undo();
						void set_redo();
					};

					fstr _str;
					unsigned anchor_pos;
					bool redraw, forced_bold, forced_italics, bold_bound, italics_bound;
					text::drafter::caret_info caret;

					void unbind_styles();
					void anchor();
					void clean_selection();

					void need_redraw();
					void guarded_redraw();

					text::drafter::source_info get_source_info();
				public:

					word_separator separator;
					/* nullptr - no whitelisting */
					const wchar_t* whitelist; 
					/* 0 - unlimited */
					unsigned max_characters; 
					pixel_32 global_color;
					misc::undoredo<action> edit;
					/* editor */

					/* define word function for CTRL+arrow traversal, 0 sets default */
					void is_word_func(bool (*)(wchar_t, bool) = 0),
						set_caret(unsigned pos, bool select = false),

						caret_left (bool select = false),
						caret_right(bool select = false),
						caret_left_word (bool select = false),
						caret_right_word (bool select = false),
						caret_left (unsigned n, bool select = false),
						caret_right(unsigned n, bool select = false),

						caret_up(bool select = false),
						caret_down(bool select = false),
						home(bool select = false),
						end(bool select = false),

						/* needs reference to system to manage application's local, formatted clipboard
						*/

						cut(system&),
						copy(system&),
						paste(system&),

						insert(fstr&),
						character(const wchar_t&),
						character(const formatted_char&),
						backspace(bool ctrl = false), 
						del(bool ctrl = false),

						select_all(),
						select_word(unsigned at),
						select_line(unsigned at),
						bold(),
						italics();
					bool undo(),
						redo();
					
					const fstr& get_str() const;

					/* gets formatting template that will be applied to subsequent characters, includes forced bolds/italics */ 
					style get_neighbor_style();
					/* same as get_neighbor_style, yet it takes forced bolds/italics into consideration, actually used function */
					style get_current_style();  

					/* whether bold/italics is forced (button B or I is pressed) */
					bool get_bold_status();
					bool get_italics_status();

					bool is_whitelisted(wchar_t c);

					/* font getting helper, shortens _str[i].font_used */
					font* getf(unsigned i); 

					unsigned get_caret_pos() const, 
						get_caret_line() const,
						get_left_selection() const,
						get_right_selection() const;
					
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

					virtual void update_rectangles();
					virtual void event_proc(rect::event);
					virtual void draw_proc(const draw_info&);
					virtual void update_proc(system&);
					virtual void on_focus(bool);

					bool view_caret, blink_reset;
					text::printer print;

					scroll scroller;

					textbox(const math::rect_xywh& rc, const material&, style default_style);
				};
			}
		}
	}
}