#pragma once
#include <algorithm>
#include "textbox.h"
#include "../../window/window.h"

#undef min
#undef max
namespace db {
	using namespace math;
	namespace graphics {
		using namespace io::input;
		namespace gui {
			namespace controls {
				font* textbox::getf(unsigned i) {
					return (i < _str.length() && _str[i].font_used) ? _str[i].font_used : caret.default_style.f;
				}

				void textbox::anchor() {
					anchor_pos = caret.pos;
				}

				void textbox::clean_selection() {
					_str.erase(_str.begin() + get_left_selection(), _str.begin() + get_right_selection());
					caret.pos = get_left_selection();
					caret.selection_offset = 0;
				}

				void textbox::need_redraw() {
					redraw = true;
				}

				void textbox::guarded_redraw() {
					if(redraw) {
						print.draft.draw(get_source_info());
						redraw = false;
					}
				}
					
				text::drafter::source_info textbox::get_source_info() {
					return text::drafter::source_info (_str, &caret, this);
				}

				const fstr& textbox::get_str() const {
					return _str;
				}

				unsigned textbox::get_caret_pos() const {
					return caret.pos;
				}

				void textbox::set_caret(unsigned pos, bool s) {
					//if(!s) select.type = select.LETTERS;
					//selection temp = select; 
					if(caret.pos < pos) {
						caret_right(pos - caret.pos, s);
						//if(select.type == select.WORDS) {
						//	// caret_right_word(true);
						//}
					}
					else if(caret.pos > pos) {
						caret_left(caret.pos - pos, s);
						//if(select.type == select.WORDS) {
						//	///caret_left_word(true);
						//}

					}
					else return;

					//if(!caret.selection_offset) {
					//	//	caret_left_word(true);
					////}
					//if(false && select.type == select.WORDS) {
					//	if(!caret.selection_offset) {
					//		select = temp;
					//		caret.selection_offset = -temp.offset;
					//	}
					//	else if(caret.selection_offset > 0){
					//		caret_left_word(true);
					//	}
					//	else if(caret.selection_offset < 0)
					//		caret_right_word(true);
					//}


				}

				void textbox::caret_left(bool s) {
					if(caret.selection_offset && !s) {
						if(caret.selection_offset < 0)
							caret_left(-caret.selection_offset, false);
						caret.selection_offset = 0;
					}
					else if(caret.pos > 0) {
						--caret.pos;
						if(s) ++caret.selection_offset;

						//if(separator.is_newline(_str[caret.pos].c)) --caret.line;
						anchor();
					}
					unbind_styles();
				}

				void textbox::caret_right(bool s) {
					if(caret.selection_offset && !s) {
						if(caret.selection_offset > 0)
							caret_right(caret.selection_offset, false);
						caret.selection_offset = 0;
					}
					else if(caret.pos < _str.length()) {
						++caret.pos;
						if(s) --caret.selection_offset;

						//if(separator.is_newline(_str[caret.pos-1].c)) ++caret.line;
						anchor();
					}
					unbind_styles();
				}

				void textbox::caret_left(unsigned n, bool s) {
					caret.pos -= (n = min(caret.pos, n));
					anchor();
					if(s) caret.selection_offset += n;
					else caret.selection_offset = 0;
					unbind_styles();
				}

				void textbox::caret_right(unsigned n, bool s) {
					caret.pos += (n = min(n, _str.length() - caret.pos));
					anchor();
					if(s) caret.selection_offset -= n;
					else caret.selection_offset = 0;
					unbind_styles();
				}

				void textbox::caret_left_word (bool s) {
					caret_left(separator.get_left_word(_str, caret.pos), s);
				}

				void textbox::caret_right_word (bool s) {
					caret_right(separator.get_right_word(_str, caret.pos), s);
				}

				void textbox::cut(system& sys) {
					if(caret.selection_offset) {
						copy(sys);
						backspace();
					}
					else {
						select_line(caret.pos);
						if(caret.selection_offset) {
							copy(sys);
							backspace();
						}
					}
				}

				void textbox::copy(system& sys) {
					if(caret.selection_offset)
						sys.copy_clipboard(_str.substr(get_left_selection(), std::abs(caret.selection_offset)));
					else {
						unsigned tmp = caret.pos;
						select_line(caret.pos);
						if(caret.selection_offset) sys.copy_clipboard(_str.substr(get_left_selection(), std::abs(caret.selection_offset)));
					}

				}

				void textbox::paste(system& sys) {
					if(sys.is_clipboard_own())
						insert(sys.clipboard);
					else {
						fstr out;
						paste_clipboard(out, get_current_style());
						insert(out);
					}
				}

				void textbox::insert(fstr& ss) {
					fstr _s;
					if(whitelist) {
						for(size_t i = 0; i < ss.length(); ++i) 
							if(is_whitelisted(ss[i].c)) _s += ss[i];
					}
					fstr& s = whitelist ? _s : ss;

					if(max_characters > 0) {
						size_t newlen = _str.size() + s.size() - std::abs(caret.selection_offset);
						if(newlen > max_characters) {
							s.erase(s.size() - (newlen - max_characters));
						}
					}

					if(caret.selection_offset) {
						edit.action(action(*this, get_left_selection(), s, _str.substr(get_left_selection(), std::abs(caret.selection_offset))));
						clean_selection();
					}
					else edit.action(action(*this, caret.pos, s, action::INSERT));

					_str.insert(caret.pos, s);
					caret.pos += s.length();
					edit.front().set_redo();

					anchor();
					need_redraw();
				}

				void textbox::character(const wchar_t& cc) {
					formatted_char ch = get_current_style();
					ch.c = cc;
					character(ch);
					unbind_styles();
					need_redraw();
				}

				void textbox::character(const formatted_char& ch) {
					if(!is_whitelisted(ch.c) || (max_characters > 0 && _str.size() + 1 - std::abs(caret.selection_offset) > max_characters)) return;

					if(caret.selection_offset) {
						edit.action(action(*this, get_left_selection(), ch, _str.substr(get_left_selection(), get_right_selection()-get_left_selection())));
						clean_selection();
					}
					else edit.action(action(*this, caret.pos, ch));
					_str.insert(_str.begin() + caret.pos, 1, ch);
					++caret.pos;
					edit.front().set_redo();
					anchor();
					need_redraw();
				}

				void textbox::backspace(bool ctrl) {
					if(caret.selection_offset) {
						edit.action(action(*this, get_left_selection(), _str.substr(get_left_selection(), get_right_selection()-get_left_selection()), action::ERASE));
						clean_selection();
						edit.front().set_redo();
						need_redraw();
					}
					else if(ctrl) {
						unsigned left_offset = separator.get_left_word(_str, caret.pos);
						if(caret.pos > left_offset-1) {
							edit.action(action(*this, caret.pos - left_offset, _str.substr(caret.pos - left_offset, left_offset), action::ERASE));
							caret.pos -= left_offset;
							_str.erase(_str.begin() + caret.pos, _str.begin() + caret.pos + left_offset);
							edit.front().set_redo();
							need_redraw();
						}
					}
					else if(caret.pos > 0) {
						edit.action(action(*this, caret.pos-1, _str.substr(caret.pos-1, 1), action::ERASE));
						_str.erase(_str.begin() + --caret.pos);
						edit.front().set_redo();
						need_redraw();
					}
					anchor();
				}

				void textbox::del(bool ctrl) {
					if(caret.selection_offset) {
						edit.action(action(*this, get_left_selection(), _str.substr(get_left_selection(), get_right_selection()-get_left_selection()), action::ERASE));
						clean_selection();
						edit.front().set_redo();
						//anchor();
					}
					else if(ctrl) {
						int right_offset = separator.get_right_word(_str, caret.pos);
						if(caret.pos + right_offset - 1 < _str.length()) {
							edit.action(action(*this, caret.pos, _str.substr(caret.pos, right_offset), action::ERASE));
							_str.erase(_str.begin() + caret.pos, _str.begin() + caret.pos + right_offset);
							edit.front().set_redo();
						}
					}
					else if(caret.pos < _str.length()) {
						edit.action(action(*this, caret.pos, _str.substr(caret.pos, 1), action::ERASE));
						_str.erase(_str.begin() + caret.pos);
						edit.front().set_redo();
					}
					anchor();
					need_redraw();
				}

				void textbox::select_all() {
					caret.pos = _str.length();
					caret.selection_offset = -int(caret.pos);
					anchor();
				}

				void textbox::select_word(unsigned at) {
					guarded_redraw();
					if(_str.empty()) return;
					if(at && ((separator.is_newline(_str[at].c) && !separator.is_newline(_str[at-1].c)) || at == _str.length())) --at;
					bool alpha = separator.is_word(_str[at].c, true);

					auto& line = print.draft.lines[print.draft.get_line(caret.pos)];
					int left = separator.get_left_word(_str, at, line.begin),
						right = separator.get_right_word(_str, at, line.end);

					caret.pos = min(caret.pos+right, _str.length());
					caret.selection_offset = -int(right - left);
					anchor();
				}

				void textbox::select_line(unsigned at) {
					guarded_redraw();
					if(_str.empty()) return;
					auto& line = print.draft.lines[print.draft.get_line(caret.pos)];
					caret.pos = line.begin;
					caret.selection_offset = -int(line.end - line.begin);
					anchor();
				}

				void textbox::bold() {
					if(caret.selection_offset) {
						bool bold_all = false;
						int l = get_left_selection(), r = get_right_selection();
						edit.action(action(*this, l, r, 0, vector<bool>(), action::BOLDEN));
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
					} else if(forced_bold = !forced_bold)
						bold_bound = !get_neighbor_style().f->is_bolded();
					need_redraw();
				}

				void textbox::italics() {
					if(caret.selection_offset) {
						bool it_all = false;
						int l = get_left_selection(), r = get_right_selection();
						edit.action(action(*this, l, r, 0, vector<bool>(), action::ITALICSEN));
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
					} else if(forced_italics = !forced_italics)
						italics_bound = !get_neighbor_style().f->is_italicsed();
					need_redraw();
				}

				bool textbox::undo() {
					need_redraw();
					return edit.undo();
				}

				bool textbox::redo() {
					need_redraw();
					return edit.redo();
				}

				void textbox::caret_up(bool s) {
					guarded_redraw();
					if(!s) {
						caret.pos = get_left_selection();
					}

					int line = print.draft.get_line(caret.pos);
					if(line > 0) {
						auto c = print.draft.lines[line-1].anchor(print.draft.sectors[min(print.draft.sectors.size()-1, anchor_pos)], print.draft.sectors);
						caret.selection_offset += caret.pos - c;
						caret.pos = c;
					}

					if(!s) caret.selection_offset = 0;
				}

				void textbox::caret_down(bool s) {
					guarded_redraw();
					if(!s) {
						caret.pos = get_right_selection();
					}

					unsigned line = print.draft.get_line(caret.pos);
					if(line < print.draft.lines.size() - 1) {
						auto c = print.draft.lines[line+1].anchor(print.draft.sectors[min(print.draft.sectors.size()-1, anchor_pos)], print.draft.sectors);
						caret.selection_offset -= c - caret.pos;
						caret.pos = c;
					}

					if(!s) caret.selection_offset = 0;
				}

				void textbox::home(bool s) {
					guarded_redraw();
					set_caret(print.draft.lines[print.draft.get_line(caret.pos)].begin, s);
				}

				void textbox::end(bool s) {
					guarded_redraw();
					set_caret(print.draft.lines[print.draft.get_line(caret.pos)].end-1, s);
				}

				textbox::action::action(textbox& subject, int where, const formatted_char& ch) 
					: subject(&subject), where(where), character(ch), flag(CHARACTERS) { set_undo(); }

				textbox::action::action(textbox& subject, int where, const formatted_char& ch, const fstr& replaced) 
					: subject(&subject), where(where), character(ch), replaced(replaced), flag(REPLACE_CHARACTERS) { set_undo(); }

				textbox::action::action(textbox& subject, int where, const fstr& _str, type flag) 
					: subject(&subject), where(where), _str(_str), flag(flag)  { set_undo(); }

				textbox::action::action(textbox& subject, int where, const fstr& _str, const fstr& replaced) 
					: subject(&subject), where(where), _str(_str), replaced(replaced), flag(action::REPLACE) { set_undo(); }

				textbox::action::action(textbox& subject, int where, int right, bool unapply, vector<bool>& v, type flag) 
					: subject(&subject), where(where), right(right), unapply(unapply), states(v), flag(flag) { set_undo(); }

				bool textbox::action::include(const action& next) {
					if((flag == CHARACTERS || flag == REPLACE_CHARACTERS) && next.flag == CHARACTERS && !subject->separator.is_newline(next.character.c) 
						&& next.where == where + _str.length() + 1 /* we don't want to merge characters at different positions */
						) {
							_str += next.character;
							return true;
					}
					return false;
				}

				void textbox::action::execute(bool undo) {
					if(flag == action::NONE) return;

					if(flag == (undo ? action::INSERT : action::ERASE)) {
						subject->_str.erase(where, _str.length());
					}
					else if(flag == (undo ? action::ERASE : action::INSERT)) {
						subject->_str.insert(where, _str);
					}

					if(flag == action::CHARACTERS) {
						if(undo) {
							subject->_str.erase(where, _str.length()+1);
						}
						else {
							subject->_str.insert(where, 1, character);
							subject->_str.insert(where +1, _str);
						}
					}

					if(flag == action::REPLACE) {
						subject->_str.erase(where, undo ? _str.length() : replaced.length());
						subject->_str.insert(where, undo ? replaced : _str);
					}

					if(flag == action::REPLACE_CHARACTERS) {
						subject->_str.erase(where, undo ? _str.length()+1 : replaced.length());
						if(undo) 
							subject->_str.insert(where, replaced);
						else {
							subject->_str.insert(where, 1, character);
							subject->_str.insert(where +1, _str);
						}
					}

					subject->caret.pos = undo ? un.caret_pos : re.caret_pos;
					subject->caret.selection_offset = undo ? un.sel_offset : re.sel_offset;
					subject->anchor();

					if(flag == action::BOLDEN || flag == action::ITALICSEN) {
						for(int i = where; i < right; ++i) {
							font*& f = subject->_str[i].font_used;
							f = flag == action::BOLDEN ?	subject->getf(i)->get_bold   (undo ? (unapply ? true : states[i-where]) : !unapply):
								subject->getf(i)->get_italics(undo ? (unapply ? true : states[i-where]) : !unapply);
						}
					}
				} 

				void textbox::action::set_undo() {
					un.caret_pos = subject->caret.pos;
					un.sel_offset = subject->caret.selection_offset;
				}

				void textbox::action::set_redo() {
					re.caret_pos = subject->caret.pos;
					re.sel_offset = subject->caret.selection_offset;
				}

				unsigned textbox::get_left_selection() const {
					return caret.selection_offset < 0 ? (caret.pos + caret.selection_offset) : caret.pos;
				}

				unsigned textbox::get_right_selection() const {
					return caret.selection_offset < 0 ? caret.pos : (caret.pos + caret.selection_offset);
				}

				void textbox::unbind_styles() {
					forced_bold = forced_italics = false;
				}

				style textbox::get_current_style() {
					style ch = get_neighbor_style();
					if(forced_bold)    ch.f = ch.f->get_bold(bold_bound);
					if(forced_italics) ch.f = ch.f->get_italics(italics_bound);
					return ch;

				}

				style textbox::get_neighbor_style() {
					if(!_str.empty()) return (caret.pos ? _str[caret.pos-1] : _str[0]);
					else return caret.default_style;
				}

				bool textbox::get_bold_status() {
					if(caret.selection_offset) {
						int l = get_left_selection(), r = get_right_selection();
						for(int i = l; i < r; ++i)
							if(!getf(i)->is_bolded())
								return false;
						return true;
					} 
					else return forced_bold ? bold_bound : get_neighbor_style().f->is_bolded(); 
				}

				bool textbox::get_italics_status() {
					if(caret.selection_offset) {
						int l = get_left_selection(), r = get_right_selection();
						for(int i = l; i < r; ++i)
							if(!getf(i)->is_italicsed())
								return false;
						return true;
					} 
					else return forced_italics ? italics_bound : get_neighbor_style().f->is_italicsed(); 
				}

				bool textbox::is_whitelisted(wchar_t c) {
					if(!whitelist) return true;
					const wchar_t* it = whitelist;
					while(*it) if(*it++ == c) return true;
					return false;
				}
			}
		}
	}
}