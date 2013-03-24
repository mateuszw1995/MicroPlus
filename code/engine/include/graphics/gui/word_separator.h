#pragma once
#include "system.h"

namespace db {
	namespace graphics {
		namespace gui {
			struct word_separator {
				static bool default_is_word(wchar_t, bool);
				static bool default_is_newline(wchar_t);

				bool (*is_word)(wchar_t, bool);
				bool (*is_newline)(wchar_t);

				word_separator(bool (*is_word)(wchar_t, bool) = 0); /* 0 - default comparator */

				void set_default();
				unsigned get_left_word(const fstr&, unsigned max_left, int at) const;
				unsigned get_left_word(const wchar_t*, unsigned max_left, int at) const;
				unsigned get_right_word(const fstr&, unsigned max_right, int at) const;
				unsigned get_right_word(const wchar_t*, unsigned max_right, int at) const;
			};
		}
	}
}