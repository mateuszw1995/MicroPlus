#pragma once
#include "../system.h"
#include "../../../misc/undoredo.h"
#include "word_separator.h"
#include "printer.h"

/* if any bugs, recheck the caret_line/get_line policy */

namespace db {
	namespace graphics {
		namespace gui {
			namespace text {
				class ui { /* user interface */
					fstr _str;
					unsigned anchor, caret_line, caret_pos;
					bool forced_bold, forced_italics, bold_bound, italics_bound;

					void _unbind_styles();
					void _anchor();
					void _clean_selection();

					/* Undo/Redo interface implementation to satisfy the template, you shouldn't care */
					struct action {
						ui* subject;

						struct memento {
							int caret_pos, sel_offset, caret_line;
						} un, re;

						int where, right;
						formatted_char character;
						fstr _str, replaced;

						vector<bool> states;
						bool unapply;

						enum type {
							NONE, CHARACTERS, INSERT, ERASE, REPLACE, REPLACE_CHARACTERS, BOLDEN, ITALICSEN
						} flag;

						action(ui* subject, int where, const formatted_char&);
						action(ui* subject, int where, const formatted_char&, const fstr& replaced);
						action(ui* subject, int where, const fstr&, type flag = INSERT);
						action(ui* subject, int where, const fstr&, const fstr& replaced = fstr());
						action(ui* subject, int left, int right, bool unapply, vector<bool>&, type flag);

						bool include(const action&);
						void execute(bool undo);
						void set_undo();
						void set_redo();
					};

				public:
					drafter* draft;
					drafter::caret_info caret;

					word_separator separator;
					const wchar_t* whitelist; /* nullptr - no whitelisting */
					unsigned max_characters; /* 0 - unlimited */
					pixel_32 global_color;
					misc::undoredo<action> edit;

					ui(style default_style);

					/* editor */

					/* define word function for CTRL+arrow traversal, 0 sets default */
					/* USER INTERFACE FUNCTIONS */
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

					style get_neighbor_style(); /* gets formatting template that will be applied to subsequent characters, includes forced bolds/italics */ 
					style get_current_style(); /* same as get_neighbor_style, yet it takes forced bolds/italics into consideration, actually used function */ 

					/* whether bold/italics is forced (button B or I is pressed) */
					bool get_bold_status();
					bool get_italics_status();

					bool is_whitelisted(wchar_t c);

					font* getf(unsigned i); /* font getting helper, shortens _str[i].font_used */

					unsigned get_caret_pos() const, 
						get_caret_line() const,
						get_left_selection() const,
						get_right_selection() const;
				};
			}
		}
	}
}