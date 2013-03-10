#pragma once
#include <algorithm>
#include "text.h"
#include "../../window/window.h"

#undef min
#undef max
namespace db {
	using namespace math;
	namespace graphics {
		using namespace io::input;
		namespace gui {
			bool is_newline(unsigned i) {
				return (i == 0x000A || i == 0x000D);
			}
			
			void scale_virtual_res(rect_wh vres, rect_wh display, vector<quad>& quads) {
				float x_mult = display.w/float(vres.w);
				float y_mult = display.h/float(vres.h);

				for(size_t i = 0; i < quads.size(); ++i) {
					for(int q = 0; q < 4; ++q) {
						quads[i].p[q].x = int(float(quads[i].p[q].x)*x_mult);
						quads[i].p[q].y = int(float(quads[i].p[q].y)*y_mult);
					}
				}
			}
			
			std::wostringstream& operator<<(std::wostringstream& stream, const fstr& f) {
				std::wstring ww;
				ww.reserve(f.length());
				for(size_t i = 0; i < f.length(); ++i)
					ww += f[i].c;

				return stream;
			}

			void formatted_text(const wchar_t* _str, fstr& out, font* f, const pixel_32& p) {
				out.clear();
				formatted_char ch;
				int len = wcslen(_str);
				for(int i = 0; i < len; ++i) {
					ch.set(_str[i], f, p);
					out.append(1, ch);
				}
			}

			fstr formatted_text(const wchar_t* _str, font* f, const pixel_32& p) {
				fstr out;	

				formatted_char ch;
				ch.font_used = f;
				int len = wcslen(_str);

				//out.reserve(len);
				for(int i = 0; i < len; ++i) {
					ch.set(_str[i], f, p);
					out.append(1, ch);
				}

				return out;
			}

			
			int text::line::height() const {
				return asc-desc;
			}

			int text::line::bottom() const {
				return top+height();
			}

			text::line::line() : begin(0) {

			}

			rect_xywh text::line::get_rect() const {
				return rect_ltrb(0, top, right, bottom());
			}

			void text::line::set(int _y, int _asc, int _desc) {
				top = _y;
				asc = _asc;
				desc = _desc;
			}
			
			font* text::getf(unsigned i) {
				return (i < _str.length() && _str[i].font_used) ? _str[i].font_used : global_font;
			}

			font* text::getf(font* f) {
				return f ? f : global_font;
			}
			
			void text::update_text() {
				if(uptodate) return;

				cached.clear();
				cached.reserve(_str.size());

				for(unsigned i = 0; i < _str.size(); ++i)
						cached.push_back(&getf(i)->get_glyph(_str[i].c));

				uptodate = true;
			}

			
			unsigned text::line::anchor(int x, const std::vector<int>& sectors) const {
				unsigned it = hover(x, sectors);

				if(!wrapped) {
					if((it >= end) || (it > begin && sectors[it] - x > x - sectors[it-1])) 
						--it;
				}
				else if(wrapped) {
					if(((it > begin && (it < end ? sectors[it] : right) - x > x - sectors[it-1])))
						--it;
				}

				return it;
			}
					
			unsigned text::line::hover(int x, const std::vector<int>& sectors) const {
				return upper_bound(sectors.begin() + begin, sectors.begin() + end, x) - sectors.begin();
			}

			void  text::selection::_origin(bool s, unsigned l) {
				if(s && !offset) line_origin = l;
			}

			int text::get_line(unsigned i) {
				line l;
				l.begin = i;
				return upper_bound(lines.begin(), lines.end(), l, [](const line& x, const line& y){return x.begin < y.begin;}) - lines.begin() - 1;
			}
			
			void text::_anchor() {
				anchor = caret_pos;
			}

			void text::_clean_selection() {
				str().erase(_str.begin() + get_left_selection(), _str.begin() + get_right_selection());
				caret_pos = get_left_selection();
				select.offset = 0;
			}

			text::text(font* global_font, pixel_32 global_color)
				: global_font(global_font), global_color(global_color), uptodate(false), wrap_width(0),
				q_begin(0), q_end(0), s_begin(0), s_end(0), caret(0), kerning(true), align_caret_height(true), caret_pos(0), caret_width(0), anchor(0),
				caret_mat(material(null_texture, pixel_32(0, 0, 0, 255))), caret_line(0),
				first_line_visible(0),
				last_line_visible(0),
				highlight_current_line(false),
				highlight_during_selection(false), active(false),
				bold_bound(false),
				italics_bound(false),
				forced_bold(false),
				forced_italics(false), max_characters(0), whitelist(0)
			{ is_word_func(0);  }

			void text::set_str(const wstring& s, const formatted_char& new_template) {
				size_t i, size = std::min(s.size(), _str.size());
				formatted_char ch = new_template;
				
				for(i = 0; i < size; ++i)
					_str[i].c = s[i];
				for(; i < s.size(); ++i) {
					ch.c = s[i];
					_str.append(1, ch);
				}
				if(s.size() < _str.size()) 
					_str.erase(i);

				uptodate = false;
			}

			fstr& text::str() {
				uptodate = false;
				return _str;
			}
			
			fstr& text::set_str() {
				edit.actions.clear();
				uptodate = false;
				return _str;
			}

			const fstr& text::get_str() const {
				return _str;
			}
				
			unsigned text::get_caret_pos() const {
				return caret_pos;
			}
			
			unsigned text::get_caret_line() const {
				return caret_line;
			}
			
			bool default_cmpf(wchar_t c, bool a) {
				return (a ? !iswspace(c) : iswspace(c)) && (a ? iswalnum(c) : !iswalnum(c));
			}
			
			text::blinker::blinker() : blink(false), interval_ms(250), blink_func(regular_blink) {
				reset();
			}
			
			void text::blinker::regular_blink(blinker& b, quad& caret) {
				for(int i = 0; i < 4; ++i)
					caret.p[i].col.a = b.caret_visible ? 255 : 0;
			}
				
			void text::blinker::update(quad& caret) {
				if(timer.get_miliseconds() > interval_ms) {
					caret_visible = !caret_visible;
					timer.microseconds();
				}
				if(blink_func) blink_func(*this, caret);
			}

			void text::blinker::reset() {
				timer.microseconds();
				caret_visible = true;
			}

			void text::is_word_func(bool (* cmpf)(wchar_t, bool)) {
				is_word = cmpf;
				if(!cmpf) is_word = default_cmpf;
			}

			void text::set_caret(unsigned pos, bool s) {
				if(!s) select.type = select.LETTERS;
				selection temp = select; 
				if(caret_pos < pos) {
					caret_right(pos - caret_pos, s);
					if(select.type == select.WORDS) {
						// caret_right_word(true);
					}
				}
				else if(caret_pos > pos) {
					caret_left(caret_pos - pos, s);
					if(select.type == select.WORDS) {
						///caret_left_word(true);
					}

				}
				else return;

				//if(!select.offset) {
				//	//	caret_left_word(true);
				////}
				//if(false && select.type == select.WORDS) {
				//	if(!select.offset) {
				//		select = temp;
				//		select.offset = -temp.offset;
				//	}
				//	else if(select.offset > 0){
				//		caret_left_word(true);
				//	}
				//	else if(select.offset < 0)
				//		caret_right_word(true);
				//}

					
			}

			void text::caret_left(bool s) {
				select._origin(s, caret_line);
				if(select.offset && !s) {
					if(select.offset < 0)
						caret_left(-select.offset, false);
					select.offset = 0;
				}
				else if(caret_pos > 0) {
					--caret_pos;
					if(s) ++select.offset;
					
					//if(is_newline(_str[caret_pos].c)) --caret_line;
					caret_line = get_line(caret_pos);
					_anchor();
				}
				_unbind_styles();
			}
			
			void text::caret_right(bool s) {
				select._origin(s, caret_line);
				if(select.offset && !s) {
					if(select.offset > 0)
						caret_right(select.offset, false);
					select.offset = 0;
				}
				else if(caret_pos < _str.length()) {
					++caret_pos;
					if(s) --select.offset;

					//if(is_newline(_str[caret_pos-1].c)) ++caret_line;
					caret_line = get_line(caret_pos);
					_anchor();
				}
				_unbind_styles();
			}

			void text::caret_left(unsigned n, bool s) {
				select._origin(s, caret_line);
				caret_line = get_line(caret_pos -= (n = min(caret_pos, n)));
				_anchor();
				if(s) select.offset += n;
				else select.offset = 0;
				_unbind_styles();
			}

			void text::caret_right(unsigned n, bool s) {
				select._origin(s, caret_line);
				caret_line = get_line(caret_pos += (n = min(n, _str.length() - caret_pos)));
				_anchor();
				if(s) select.offset -= n;
				else select.offset = 0;
				_unbind_styles();
			}

			unsigned text::get_left_word(int left, int at) {
				bool alpha = is_word(_str[at ? at-1 : 0].c, true);

				int offset = 0;
				while(--at >= left) {
					if(!is_newline(_str[at].c) && is_word(_str[at].c, alpha)) ++offset;
					else break;
				}

				if(!offset) offset = 1;
				return offset;
			}
			
			unsigned text::get_right_word(int right, int at) {
				bool alpha = is_word(_str[at].c, true);

				int offset = 0;
				right = min(int(_str.length()), right);

				for(int i = at; i < right; ++i) {
					if(!is_newline(_str[i].c) && is_word(_str[i].c, alpha)) ++offset;
					else break;
				}

				if(!offset) offset = 1;
				return offset;
			}

			void text::caret_left_word (bool s) {
				caret_left(get_left_word(0, caret_pos), s);
			}

			void text::caret_right_word (bool s) {
				caret_right(get_right_word(_str.length(), caret_pos), s);
			}

			
			void text::cut(system& sys) {
				if(select.offset) {
					copy(sys);
					backspace();
				}
				else {
					select_line(caret_pos);
					if(select.offset) {
						copy(sys);
						backspace();
					}
				}
			}
			
			void text::copy(system& sys) {
				if(select.offset)
					sys.copy_clipboard(_str.substr(get_left_selection(), std::abs(select.offset)));
				else {
					unsigned tmp = caret_pos;
					select_line(caret_pos);
					if(select.offset) sys.copy_clipboard(_str.substr(get_left_selection(), std::abs(select.offset)));
				}

			}
			
			void text::paste(system& sys) {
				if(sys.is_clipboard_own())
					insert(sys.clipboard);
				else {
					fstr out;
					paste_clipboard(out, get_current_style());
					insert(out);
				}
			}

			void text::insert(fstr& ss) {
				fstr _s;
				if(whitelist) {
					for(size_t i = 0; i < ss.length(); ++i) 
						if(is_whitelisted(ss[i].c)) _s += ss[i];
				}
				fstr& s = whitelist ? _s : ss;

				if(max_characters > 0) {
					size_t newlen = _str.size() + s.size() - std::abs(select.offset);
					if(newlen > max_characters) {
						s.erase(s.size() - (newlen - max_characters));
					}
				}

				if(select.offset) {
					 edit.action(action(this, get_left_selection(), s, str().substr(get_left_selection(), std::abs(select.offset))));
					_clean_selection();
				}
				else edit.action(action(this, caret_pos, s, action::INSERT));

				str().insert(caret_pos, s);
				caret_pos += s.length();
				edit.front().set_redo();
				_anchor();
			}
			
			void text::character(const wchar_t& cc) {
				formatted_char ch = get_current_style();
				ch.c = cc;
				character(ch);
				_unbind_styles();
			}

			void text::character(const formatted_char& ch) {
				if(!is_whitelisted(ch.c) || (max_characters > 0 && _str.size() + 1 - std::abs(select.offset) > max_characters)) return;

				if(select.offset) {
					 edit.action(action(this, get_left_selection(), ch, str().substr(get_left_selection(), get_right_selection()-get_left_selection())));
					 _clean_selection();
				}
				else edit.action(action(this, caret_pos, ch));
				str().insert(_str.begin() + caret_pos, 1, ch);
				++caret_pos;
				edit.front().set_redo();
				_anchor();
			}

			void text::backspace(bool ctrl) {
				if(select.offset) {
					 edit.action(action(this, get_left_selection(), str().substr(get_left_selection(), get_right_selection()-get_left_selection()), action::ERASE));
					 _clean_selection();
					 edit.front().set_redo();
				}
				else if(ctrl) {
					unsigned left_offset = get_left_word(0, caret_pos);
					if(caret_pos > left_offset-1) {
						edit.action(action(this, caret_pos - left_offset, str().substr(caret_pos - left_offset, left_offset), action::ERASE));
						caret_pos -= left_offset;
						str().erase(_str.begin() + caret_pos, _str.begin() + caret_pos + left_offset);
						edit.front().set_redo();
					}
				}
				else if(caret_pos > 0) {
					edit.action(action(this, caret_pos-1, str().substr(caret_pos-1, 1), action::ERASE));
					str().erase(_str.begin() + --caret_pos);
					edit.front().set_redo();
				}
				_anchor();
			}

			void text::del(bool ctrl) {
				if(select.offset) {
					 edit.action(action(this, get_left_selection(), str().substr(get_left_selection(), get_right_selection()-get_left_selection()), action::ERASE));
					_clean_selection();
					edit.front().set_redo();
				//_anchor();
				}
				else if(ctrl) {
					int right_offset = get_right_word(_str.length(), caret_pos);
					if(caret_pos + right_offset - 1 < _str.length()) {
						edit.action(action(this, caret_pos, str().substr(caret_pos, right_offset), action::ERASE));
						str().erase(_str.begin() + caret_pos, _str.begin() + caret_pos + right_offset);
						edit.front().set_redo();
					}
				}
				else if(caret_pos < _str.length()) {
					edit.action(action(this, caret_pos, str().substr(caret_pos, 1), action::ERASE));
					str().erase(_str.begin() + caret_pos);
					edit.front().set_redo();
				}
				_anchor();
			}

			void text::select_all() {
				caret_pos = _str.length();
				select.offset = -int(caret_pos);
				select.line_origin = 0;
				caret_line = lines.size()-1;
				_anchor();
			}
			
			void text::select_word(unsigned at) {
				if(_str.empty()) return;
				if(at && ((is_newline(_str[at].c) && !is_newline(_str[at-1].c)) || at == _str.length())) --at;
				bool alpha = is_word(_str[at].c, true);

				int left = at, right = at;
				for(int i = at; i >= 0; --i)
					if(!is_newline(_str[i].c) && is_word(_str[i].c, alpha)) left = i;
					else break;
				for(unsigned i = at; i < _str.length(); ++i)
					if(!is_newline(_str[i].c) && is_word(_str[i].c, alpha)) right = i;
					else break;

				// always the same line
				caret_pos = min(right+1u, _str.length());
				select.offset = -(right+1 - left);
			//	select.line_origin = caret_line;
				select.line_origin = get_line(caret_pos + select.offset);
				caret_line = get_line(caret_pos);
				select.type = select.WORDS;
				_anchor();
			}
			
			void text::select_line(unsigned at) {
				if(_str.empty()) return;
				int left = at, right = at;
				for(int i = at; i >= 0; --i)
					if(!is_newline(_str[i].c)) left = i;
					else if(i != at) break;
				for(unsigned i = at; i+1 < _str.length(); ++i)
					if(!is_newline(_str[i].c)) right = i+1;
					else break;

				// always the same line
				caret_pos = left;
				select.offset = min(right+1-left, int(_str.length()-caret_pos));
				//select.line_origin = caret_line;
				select.line_origin = get_line(caret_pos + select.offset);
				caret_line = get_line(caret_pos);
				select.type = select.LINES;
				_anchor();
			}

			void text::view_caret(rect& clipper) {	
				if(!sectors.empty() && !lines.empty()) {
					const rect_ltrb& rc = clipper.get_rect_absolute();
					pointf& pen = clipper.pen;
					pointf tp = pen;
					if(caret_line <= unsigned(first_line_visible))
						pen.y = float((lines[caret_line].top - lines[0].top));
					else if(caret_line >= unsigned(last_line_visible))
						pen.y = float((lines[caret_line].top - lines[0].top) + rc.h() - lines[caret_line].height());

					int car = min(sectors.size()-1, caret_pos);
					if(sectors[car] <= rc.l)
						pen.x = float((sectors[car] - sectors[lines[caret_line].begin]));
					if(sectors[car] >= rc.r)
						pen.x = float((sectors[car] - sectors[lines[caret_line].begin]) + rc.w() - 1);

					pen.y = max(pen.y, 0.f);
					pen.x = max(pen.x, 0.f);
				}
			}
			
			void text::bold() {
				if(select.offset) {
					bool bold_all = false;
					int l = get_left_selection(), r = get_right_selection();
					edit.action(action(this, l, r, 0, vector<bool>(), action::BOLDEN));
					for(int i = l; i < r; ++i) {
						if(getf(i)->can_be_bolded() && !getf(i)->is_bolded()) {
							bold_all = true;
							break;
						}
					}
					edit.front().unapply = !bold_all;
					edit.front().set_redo();

					for(int i = l; i < r; ++i) {
						font* f = getf(i);
						edit.front().states.push_back(f->is_bolded());
						_str[i].font_used = f->get_bold(bold_all);
					}
					uptodate = false;
				} else if(forced_bold = !forced_bold)
						bold_bound = !getf(get_neighbor_style().font_used)->is_bolded();
			}

			void text::italics() {
				if(select.offset) {
					bool it_all = false;
					int l = get_left_selection(), r = get_right_selection();
					edit.action(action(this, l, r, 0, vector<bool>(), action::ITALICSEN));
					for(int i = l; i < r; ++i) {
						if(getf(i)->can_be_italicsed() && !getf(i)->is_italicsed()) {
							it_all = true;
							break;
						}
					}
					edit.front().unapply = !it_all;
					edit.front().set_redo();

					for(int i = l; i < r; ++i) {
						font* f = getf(i);
						edit.front().states.push_back(f->is_italicsed());
						_str[i].font_used = f->get_italics(it_all);
					}
					uptodate = false;
				} else if(forced_italics = !forced_italics)
						italics_bound = !getf(get_neighbor_style().font_used)->is_italicsed();
			}

			void text::undo() {
				edit.undo();
			}

			void text::redo() {
				edit.redo();
			}
			
			text::action::action(text* subject, int where, const formatted_char& ch) 
				: subject(subject), where(where), character(ch), flag(CHARACTERS) { set_undo(); }

			text::action::action(text* subject, int where, const formatted_char& ch, const fstr& replaced) 
				: subject(subject), where(where), character(ch), replaced(replaced), flag(REPLACE_CHARACTERS) { set_undo(); }

			text::action::action(text* subject, int where, const fstr& _str, type flag) 
				: subject(subject), where(where), _str(_str), flag(flag)  { set_undo(); }

			text::action::action(text* subject, int where, const fstr& _str, const fstr& replaced) 
				: subject(subject), where(where), _str(_str), replaced(replaced), flag(action::REPLACE) { set_undo(); }
			
			text::action::action(text* subject, int where, int right, bool unapply, vector<bool>& v, type flag) 
				: subject(subject), where(where), right(right), unapply(unapply), states(v), flag(flag) { set_undo(); }

			bool text::action::include(const action& next) {
				if((flag == CHARACTERS || flag == REPLACE_CHARACTERS) && next.flag == CHARACTERS && !is_newline(next.character.c) 
					&& next.where == where + _str.length() + 1 /* we don't want to merge characters at different positions */
					) {
					_str += next.character;
					return true;
				}
				return false;
			}

			void text::action::execute(bool undo) {
				if(flag == action::NONE) return;

				if(flag == (undo ? action::INSERT : action::ERASE)) {
					subject->str().erase(where, _str.length());
				}
				else if(flag == (undo ? action::ERASE : action::INSERT)) {
					subject->str().insert(where, _str);
				}

				if(flag == action::CHARACTERS) {
					if(undo) {
						subject->str().erase(where, _str.length()+1);
					}
					else {
						subject->str().insert(where, 1, character);
						subject->str().insert(where +1, _str);
					}
				}
				
				if(flag == action::REPLACE) {
					subject->str().erase(where, undo ? _str.length() : replaced.length());
					subject->str().insert(where, undo ? replaced : _str);
				}

				if(flag == action::REPLACE_CHARACTERS) {
					subject->str().erase(where, undo ? _str.length()+1 : replaced.length());
					if(undo) 
						subject->str().insert(where, replaced);
					else {
						subject->str().insert(where, 1, character);
						subject->str().insert(where +1, _str);
					}
				}

				subject->caret_pos = undo ? un.caret_pos : re.caret_pos;
				subject->caret_line = undo ? un.caret_line : re.caret_line;
				subject->select.offset = undo ? un.sel_offset : re.sel_offset;
				subject->select.line_origin = undo ? un.line_origin : re.line_origin;
				subject->_anchor();

				if(flag == action::BOLDEN || flag == action::ITALICSEN) {
					for(int i = where; i < right; ++i) {
						font*& f = subject->_str[i].font_used;
						f = flag == action::BOLDEN ?	subject->getf(i)->get_bold   (undo ? (unapply ? true : states[i-where]) : !unapply):
														subject->getf(i)->get_italics(undo ? (unapply ? true : states[i-where]) : !unapply);
					}
					subject->uptodate = false;
				}
			} 
			
			void text::action::set_undo() {
				un.caret_pos = subject->caret_pos;
				un.caret_line = subject->caret_line;
				un.sel_offset = subject->select.offset;
				un.line_origin = subject->select.line_origin;
			}

			void text::action::set_redo() {
				re.caret_pos = subject->caret_pos;
				re.caret_line = subject->caret_line;
				re.sel_offset = subject->select.offset;
				re.line_origin = subject->select.line_origin;
			}
			
			rect* text::get_parent() {
				return p;
			}

			unsigned text::map_mouse(const point& p, bool nearest) {
				if(lines.empty()) return 0;
				line l;
				l.top = p.y;
				l.desc = l.asc = 0;
				auto it = lower_bound(lines.begin() + first_line_visible, lines.begin() + last_line_visible, l, [](const line& x, const line& y){return x.top+x.height() < y.top+y.height();});
				if(it == lines.end()) --it;
				return nearest ? (*it).anchor(p.x, sectors) : (*it).hover(p.x, sectors)-1;
			}

			void text::caret_up(bool s) {
				select._origin(s, caret_line);
				if(!s) {
					if(select.offset < 0) caret_line = select.line_origin;
					caret_pos = get_left_selection();
				}

				if(caret_line > 0) {
					auto c = lines[--caret_line].anchor(sectors[min(sectors.size()-1, anchor)], sectors);
					select.offset += caret_pos - c;
					caret_pos = c;
				}
				
				if(!s) select.offset = 0;
			}

			void text::caret_down(bool s) {
				select._origin(s, caret_line);
				if(!s) {
					if(select.offset > 0) caret_line = select.line_origin;
					caret_pos = get_right_selection();
				}

				if(caret_line < lines.size() - 1) {
					auto c = lines[++caret_line].anchor(sectors[min(sectors.size()-1, anchor)], sectors);
					select.offset -= c - caret_pos;
					caret_pos = c;
				}
				
				if(!s) select.offset = 0;
			}
			
			void text::home(bool s) {
				set_caret(lines[caret_line].begin, s);
			}

			void text::end(bool s) {
				set_caret(lines[caret_line].end-1, s);
			}

			void text::find_ascdesc(int l, int r, int& asc, int& desc) {
				if(l == r) {
					if(l > 0) {
						asc =  getf(l-1)->parent->ascender;
						desc = getf(l-1)->parent->descender;
					}
					else {
						asc =  getf(l)->parent->ascender;
						desc = getf(l)->parent->descender;
					}
				}
				else {
					asc = getf(l)->parent->ascender, desc = getf(l)->parent->descender;

					for(int j = l; j < r; ++j) {
						asc =  max(asc, getf(j)->parent->ascender);
						desc = min(desc, getf(j)->parent->descender);
					}
				}
			}

			int text::get_kern(unsigned i, unsigned l) {
				if(kerning && i > lines[l].begin && getf(i) == getf(i-1)) {
					auto& vk = cached[i]->info->kerning;
					for(unsigned k = 0; k < vk.size(); ++k)
						if(vk[k].first == _str[i-1].c)
							return vk[k].second;
				}
				return 0;
			}
			
			void text::_unbind_styles() {
				forced_bold = forced_italics = false;
			}

			void text::update_rectangles(rect* clipper) {
				point g_pen(clipper ? clipper->get_absolute_xy() - clipper->pen + pos : pos);

				int asc, desc;
				bool crlf = false;
				unsigned line_x = g_pen.x, l = 0, i;

				update_text();
				
				lines.clear();
				sectors.clear();
				
				first_line_visible = -1;
				 last_line_visible = -1;
				
				lines.push_back(line());
				max_x = line_x;
				for(i = 0; i < cached.size(); ++i) {
					crlf = is_newline(_str[i].c);
					 
					auto& g = *cached[i];

					lines[l].right = g_pen.x;
					g_pen.x += get_kern(i, l) + g.info->adv;
					bool wrap = (clipper && wrap_width > 0 && g_pen.x >= clipper->get_rect_absolute().l + wrap_width - clipper->pen.x);
					
					if(crlf || wrap) {
						lines.push_back(line());
						lines[l].wrapped = wrap;
						lines[l].end = lines[l+1].begin = i+1-wrap;
						
						g_pen.x = line_x;
						if(wrap) {
							int left = get_left_word(lines[l].begin, i);
							if((lines[l].end - left) > lines[l].begin) {
								lines[l].end -= left;
								lines[l+1].begin -= left;
								
								for(unsigned k = lines[l+1].begin; k <= lines[l+1].begin+left; ++k)
									g_pen.x += cached[k]->info->adv + get_kern(k, l+1);
							}
							else g_pen.x += g.info->adv + get_kern(i, l);
						}
						
						++l;
					} else max_x = max(max_x, g_pen.x);
				}
				
				lines[l].wrapped = false;
				lines[l].end = i+1;

				for(l = 0; l < lines.size(); ++l) {
					g_pen.x = line_x;
					find_ascdesc(lines[l].begin, lines[l].end-1, asc, desc);
					lines[l].set(g_pen.y, asc, desc);
					if(clipper && lines[l].top + lines[l].height() >= clipper->get_clipped_rect().t && first_line_visible == -1) first_line_visible = l;
					if(clipper && lines[l].top + lines[l].height() >= clipper->get_clipped_rect().t && lines[l].top <= clipper->get_clipped_rect().b)  last_line_visible = l;

					g_pen.y += asc;

					for(i = lines[l].begin; i < lines[l].end && i < cached.size(); ++i) {
						g_pen.x += get_kern(i, l);
						sectors.push_back(g_pen.x);

						g_pen.x += cached[i]->info->adv;
					}
					g_pen.y -= desc;
				}

				sectors.push_back(g_pen.x);

				caret_line = get_line(caret_pos);

				if(align_caret_height)
					caret_rect = rect_xywh(sectors[caret_pos], lines[caret_line].top, caret_width, lines[caret_line].height());
				else {
					int pos = max(1u, caret_pos);
					caret_rect = rect_xywh(sectors[caret_pos], lines[caret_line].top + lines[caret_line].asc - getf(pos-1)->parent->ascender, 
						caret_width, getf(pos-1)->parent->ascender - getf(pos-1)->parent->descender);
				}
				
				if(last_line_visible == -1)  last_line_visible = l;
				last_line_visible = min((unsigned)last_line_visible, lines.size()-1);
				if(first_line_visible == -1)  first_line_visible = 0;

				if(clipper) clipper->bounding_box.contain(get_bbox() - (clipper ? clipper->get_absolute_xy() - clipper->pen : point(0, 0)));
			}

			void text::draw_proc(std::vector<quad>& v, rect* clipper) {
				if(lines.empty() || sectors.empty()) return;
				v.reserve(v.size() + _str.size() + 1); // one for caret
				bool selecting = false;
				unsigned select_left = caret_pos, select_right = caret_pos;
				
				(select.offset < 0 ? select_left : select_right) += select.offset;
				rect_ltrb selector;
				vector<quad> selects;

				if(active && highlight_current_line)
					rect::add_quad(highlight_mat, rect_ltrb(sectors[0], lines[caret_line].top, numeric_limits<int>::max(), lines[caret_line].bottom()), clipper, selects);

				q_begin = v.size();
				
				size_t i = 0, l = first_line_visible;

				if(lines[l].begin > select_left && lines[l].begin < select_right) {
					selector = rect_xywh(sectors[lines[l].begin], lines[l].top, 0, lines[l].height());
					selecting = true;
				}

				for(;l <= unsigned(last_line_visible); ++l) {
					if(selecting)
							selector = rect_xywh(sectors[lines[l].begin], lines[l].top, 0, lines[l].height());

					for(i = lines[l].begin; i < lines[l].end && i < cached.size(); ++i) {

						if(i == select_left && select.offset && !selecting) {
							selector = rect_xywh(sectors[i == select_left ? i : lines[l].begin], lines[l].top, 0, lines[l].height());
							selecting = true;
						}
						
						else if(i == select_right && selecting) {
							selector.r = sectors[i];
							rect::add_quad(active ? select.bg_mat : select.inactive_bg_mat, selector, clipper, selects);  
							selecting = false;
						}

						auto& g = *cached[i];

						if(g.tex.get_rect().w) {
							rect::add_quad(
								material(&g.tex, 
								selecting ? select.color : pixel_32(_str[i].r, _str[i].g, _str[i].b, _str[i].a)), 
								rect_xywh (sectors[i] + g.info->bear_x, lines[l].top+lines[l].asc - g.info->bear_y, g.info->width, g.info->height), 
								clipper, v);
						}

					}

					if(selecting) {
						selector.r = sectors[i-(l+1 != lines.size())] + (l+1 != lines.size())*cached[i-1]->info->adv;
						rect::add_quad(active ? select.bg_mat : select.inactive_bg_mat, selector, clipper, selects);  
						if(i == select_right)
							selecting = false;
					}
				}
				
				q_end = v.size();
				v.insert(v.begin() + q_begin, selects.begin() + (active && highlight_current_line && !highlight_during_selection && selects.size() > 1), selects.end());
				s_begin = q_begin;

				q_begin += selects.size();
				q_end   += selects.size();
				
				s_end   = q_begin;
				if(active) {
					caret = q_end;
					if(rect::add_quad(caret_mat, caret_rect, clipper, v).good()) blink.update(*v.rbegin());
				} else caret = -1;
			}
			
			
			formatted_char text::get_current_style() {
				formatted_char ch = get_neighbor_style();
				if(forced_bold)    ch.font_used = ch.font_used->get_bold(bold_bound);
				if(forced_italics) ch.font_used = ch.font_used->get_italics(italics_bound);
				return ch;

			}

			formatted_char text::get_neighbor_style() {
				formatted_char ch;
				if(!_str.empty()) ch = caret_pos ? _str[caret_pos-1] : _str[0];
				else ch.set(0, global_font, global_color);
				return ch;
			}
			
			bool text::get_bold_status() {
				if(select.offset) {
					int l = get_left_selection(), r = get_right_selection();
					for(int i = l; i < r; ++i)
						if(!getf(i)->is_bolded())
							return false;
					return true;
				} 
				else return forced_bold ? bold_bound : getf(get_neighbor_style().font_used)->is_bolded(); 
			}

			bool text::get_italics_status() {
				if(select.offset) {
					int l = get_left_selection(), r = get_right_selection();
					for(int i = l; i < r; ++i)
						if(!getf(i)->is_italicsed())
							return false;
					return true;
				} 
				else return forced_italics ? italics_bound : getf(get_neighbor_style().font_used)->is_italicsed(); 
			}
			
			bool text::is_whitelisted(wchar_t c) {
				if(!whitelist) return true;
				const wchar_t* it = whitelist;
				while(*it) if(*it++ == c) return true;
				return false;
			}

			rect_ltrb text::get_bbox() {
				if(sectors.empty() || lines.empty()) return rect_ltrb(0,0,0,0);
				return rect_ltrb(sectors[0], lines[0].top, max_x, (*lines.rbegin()).top + (*lines.rbegin()).height());
			}

			text::selection::selection(const pixel_32& color, const material& bg_mat) : color(color), bg_mat(bg_mat), offset(0), line_origin(0), type(LETTERS) {}

			int text::get_selection_begin() {
				return s_begin;
			} 

			int text::get_selection_end() {
				return s_end;
			}

			int text::get_quads_begin() {
				return q_begin;
			}
			
			int text::get_quads_end() {
				return q_end;
			}

			int text::get_caret() {
				return caret;
			}

			unsigned text::get_left_selection() {
				return select.offset < 0 ? (caret_pos + select.offset) : caret_pos;
			}
			
			unsigned text::get_right_selection() {
				return select.offset < 0 ? caret_pos : (caret_pos + select.offset);
			}

			unsigned text::get_first_line_visible() {
				return first_line_visible;
			}

			unsigned text::get_last_line_visible() {
				return last_line_visible;
			}
		}
	}
}
