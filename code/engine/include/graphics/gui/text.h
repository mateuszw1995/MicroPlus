#pragma once
#include "system.h"
#include "rect.h"
#include "font.h"
#include "scroll.h"
#include "../../misc/undoredo.h"

namespace db {
	namespace graphics {
		namespace gui {
			class text {
				rect* p;
				fstr _str;
				std::vector<font::glyph*> cached;

				bool (*is_word)(wchar_t, bool);
				bool uptodate;
				bool forced_bold, forced_italics, bold_bound, italics_bound;
				int q_begin, q_end, s_begin, s_end, caret;
				int first_line_visible, last_line_visible, max_x;
				unsigned anchor, caret_line, caret_pos;

				void _anchor();
				void _clean_selection();
				void update_text();
				void find_ascdesc(int i, int j, int&, int&);
				void _unbind_styles();
				
				/* Undo/Redo interface implementation to satisfy the template, you shouldn't care */
				struct action {
					text* subject;

					struct memento {
						int caret_pos, sel_offset, line_origin, caret_line;
					} un, re;

					int where, right;
					formatted_char character;
					fstr _str, replaced;

					vector<bool> states;
					bool unapply;

					enum type {
						NONE, CHARACTERS, INSERT, ERASE, REPLACE, REPLACE_CHARACTERS, BOLDEN, ITALICSEN
					} flag;

					action(text* subject, int where, const formatted_char&);
					action(text* subject, int where, const formatted_char&, const fstr& replaced);
					action(text* subject, int where, const fstr&, type flag = INSERT);
					action(text* subject, int where, const fstr&, const fstr& replaced = fstr());
					action(text* subject, int left, int right, bool unapply, vector<bool>&, type flag);
					
					bool include(const action&);
					void execute(bool undo);
					void set_undo();
					void set_redo();
				};

			public:
				struct line {
					unsigned anchor(int x, const std::vector<int>& sectors) const; /* return sector that is the closest x */
					unsigned hover(int x, const std::vector<int>& sectors) const;  /* return sector that x exactly is on  */
					rect_xywh get_rect() const; /* get the actual line coordinates */

				private:
					friend class text;
					int top, right, height() const, bottom() const, /* coordinates */
						asc, desc;
					unsigned begin, end;
					bool wrapped;
					line();
					void set(int y, int asc, int desc);
				};

				struct selection {
					friend class text;
					pixel_32 color;
					material bg_mat;
					material inactive_bg_mat;
					int offset, line_origin;
					enum {
						LETTERS, WORDS, LINES
					} type;
					selection(const pixel_32& = pixel_32(), 
							  const material& = material(null_texture, pixel_32(51, 153, 255, 255)));
				private:
					void _origin(bool, unsigned);
				} select;


				/* USER SETTINGS */
				/* defines how the caret should blink and whether should blink at all */

				struct blinker {
					bool blink, caret_visible;
					int interval_ms;
					
					static void regular_blink(blinker&, quad& caret);
					void (*blink_func)(blinker&, quad&);

					window::timer timer;
					blinker();
					void update(quad& caret);
					void reset();
				} blink;
				
				pointf pos;
				material caret_mat, 
					highlight_mat; /* material for line highlighting */
				unsigned caret_width, wrap_width;
				bool active, kerning, align_caret_height, /* whether caret should be always of line height */
					highlight_current_line, 
					highlight_during_selection;
				
				font* global_font;
				const wchar_t* whitelist;
				unsigned max_characters;
				pixel_32 global_color;
				
				/* optional access */

				std::vector<line> lines;
				std::vector<int> sectors;
				rect_ltrb caret_rect;

				/* editor */

				text(font* global_font, pixel_32 global_color);

				misc::undoredo<action> edit;

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

					 /* needs reference to system to manage application's local, formatted clipboard */

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
					 view_caret(rect& clipper),
					 bold(),
					 italics(),
					 undo(),
					 redo();

				 void set_str(const wstring&, const formatted_char& new_template);
				fstr& set_str();
				fstr& str();

				/* GUI utilities */
				void update_rectangles(rect* clipper);
				void draw_proc(std::vector<quad>& v, rect* clipper);

				
				formatted_char get_neighbor_style(); /* gets formatting template that will be applied to subsequent characters, includes forced bolds/italics */ 
				formatted_char get_current_style(); /* same as get_neighbor_style, yet it takes forced bolds/italics into consideration, actually used function */ 

				/* whether bold/italics is forced (button B or I is pressed) */
				bool get_bold_status();
				bool get_italics_status();

				bool is_whitelisted(wchar_t c);
				
				/* getters */
				const fstr& get_str() const;
				unsigned get_caret_pos() const, get_caret_line() const;
				int get_line(unsigned character_pos);
				unsigned map_mouse(const point&, bool nearest);
				font* getf(unsigned i);
				font* getf(font* f);
				int get_kern(unsigned code1, unsigned code2);
				rect_ltrb get_bbox(); /* same as rect::global */
				unsigned get_first_line_visible(), get_last_line_visible(),
					get_left_word(int left, int at), get_right_word(int right, int at), get_left_selection(), get_right_selection();
				int get_quads_begin(), get_quads_end(), get_selection_begin(), get_selection_end(), get_caret();
				rect* get_parent();
			};

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
					scroll scroller;

					textbox(const math::rect_xywh& rc, const material&, text*);
				};
			}
		}
	}
}