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

				unsigned word_separator::get_left_word(const fstr& _str, unsigned max_left, unsigned at) const {
					bool alpha = is_word(_str[at ? at-1 : 0].c, true);

					int offset = 0;
					while(--at >= max_left) {
						if(!is_newline(_str[at].c) && is_word(_str[at].c, alpha)) ++offset;
						else break;
					}

					if(!offset) offset = 1;
					return offset;
				}

				unsigned word_separator::get_left_word(const wchar_t* _str, unsigned max_left, unsigned at) const {
					bool alpha = is_word(_str[at ? at-1 : 0], true);

					int offset = 0;
					while(--at >= max_left) {
						if(!is_newline(_str[at]) && is_word(_str[at], alpha)) ++offset;
						else break;
					}

					if(!offset) offset = 1;
					return offset;
				}

				unsigned word_separator::get_right_word(const fstr& _str, unsigned max_right, unsigned at) const {
					bool alpha = is_word(_str[at].c, true);

					int offset = 0;
					max_right = min(_str.length(), max_right);

					for(int i = at; i < max_right; ++i) {
						if(!is_newline(_str[i].c) && is_word(_str[i].c, alpha)) ++offset;
						else break;
					}

					if(!offset) offset = 1;
					return offset;
				}

				unsigned word_separator::get_right_word(const wchar_t* _str, unsigned max_right, unsigned at) const {
					bool alpha = is_word(_str[at], true);

					int offset = 0;
					for(int i = at; i < max_right; ++i) {
						if(!_str[i]) break;
						if(!is_newline(_str[i]) && is_word(_str[i], alpha)) ++offset;
						else break;
					}

					if(!offset) offset = 1;
					return offset;
				}
		}
	}
}