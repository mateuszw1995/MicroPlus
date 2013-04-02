#pragma once
#include "caret.h"
#include "drafter.h"
#include "../../../misc/undoredo.h"
#include "../font.h"

namespace db {
	namespace graphics {
		namespace gui {
			namespace text {/* Undo/Redo interface implementation to satisfy the template, you shouldn't care */
				class ui {	
					struct action {
						ui* subject;

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

						action(ui& subject, int where, const formatted_char&);
						action(ui& subject, int where, const formatted_char&, const fstr& replaced);
						action(ui& subject, int where, const fstr&, type flag = INSERT);
						action(ui& subject, int where, const fstr&, const fstr& replaced = fstr());
						action(ui& subject, int left, int right, bool unapply, vector<bool>&, type flag);

						bool include(const action&);
						void execute(bool undo);
						void set_undo();
						void set_redo();
					};

					fstr _str;
					unsigned anchor_pos;
					bool redraw, forced_bold, forced_italics, bold_bound, italics_bound;

					friend class printer;
					caret_info caret;

					void unbind_styles();
					void anchor();
					void clean_selection();

					void need_redraw();
				public:

					drafter draft;
					word_separator separator;
					/* nullptr - no whitelisting */
					const wchar_t* whitelist; 
					/* 0 - unlimited */
					unsigned max_characters; 
					pixel_32 global_color;
					misc::undoredo<action> edit;
					/* editor */
					ui(style default_style);
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

					void guarded_redraw();
					virtual const fstr& get_str() const;

					/* gets formatting template that will be applied to subsequent characters, includes forced bolds/italics */ 
					style get_neighbor_style() const;
					/* same as get_neighbor_style, yet it takes forced bolds/italics into consideration, actually used function */
					style get_current_style() const;  

					/* whether bold/italics is forced (button B or I is pressed) */
					bool get_bold_status() const;
					bool get_italics_status() const;

					bool is_whitelisted(wchar_t c) const;

					/* font getting helper, shortens _str[i].font_used */
					font* getf(unsigned i) const; 

					unsigned get_caret_pos() const, 
						get_caret_line() const,
						get_left_selection() const,
						get_right_selection() const;

				};
			}
		}
	}
}