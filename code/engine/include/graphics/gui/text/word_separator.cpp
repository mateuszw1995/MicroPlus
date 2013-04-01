#pragma once
#include "word_separator.h"

namespace db {
	namespace graphics {
		namespace gui {
				bool word_separator::default_is_newline(wchar_t i) {
					return (i == 0x000A || i == 0x000D);
				}
				
				bool word_separator::default_is_word(wchar_t c, bool a) {
					return (a ? !iswspace(c) : iswspace(c)) && (a ? iswalnum(c) : !iswalnum(c));
				}

				void word_separator::set_default() {
					is_word =    default_is_word;
					is_newline = default_is_newline;
				}

				word_separator::word_separator(bool (*is_word)(wchar_t, bool)) : is_word(is_word) {
					if(is_word == 0) set_default();
				}

				unsigned word_separator::get_left_word(const fstr& _str, int at, int max_left) const {
					if(max_left == -1) max_left = 0;
					if(_str.empty() || at <= int(max_left)) return 0;

					bool alpha = is_word(_str[at ? at-1 : 0].c, true);
					--at;

					unsigned offset = 1;
					while(at > max_left) {
						if(!is_newline(_str[at ? at - 1 : 0].c) && is_word(_str[at ? at - 1 : 0].c, alpha)) {
							++offset;
							--at;
						}
						else break;
					}

					return offset;
				}

				unsigned word_separator::get_right_word(const fstr& _str, int at, int max_right) const {
					if(max_right == -1) max_right = _str.length();
					if(_str.empty() || at >= int(max_right)) return 0;

					bool alpha = is_word(_str[at].c, true);
					++at;

					unsigned offset = 1;
					while(at < max_right) {
						if(!is_newline(_str[at].c) && is_word(_str[at].c, alpha)) {
							++offset;
							++at;
						}
						else break;
					}

					return offset;
				}
		}
	}
}