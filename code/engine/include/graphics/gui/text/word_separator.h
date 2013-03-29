#pragma once
#include "system.h"

namespace db {
	namespace graphics {
		namespace gui {
			struct word_separator {
				static bool default_is_word(wchar_t, bool);
				static bool default_is_newline(wchar_t);

				bool (*is_word)(wchar_t character, bool alpha);
				bool (*is_newline)(wchar_t);

				word_separator(bool (*is_word)(wchar_t, bool) = 0); /* 0 - default comparator */

				void set_default();

				/* max_left/right are introduced for word wrappers where no newlines are present inside strings
					-1 means no boundaries
				*/
				unsigned get_left_word(const fstr&, int at, int max_left = -1) const;
				unsigned get_right_word(const fstr&, int at, int max_right = -1) const;
			};
		}
	}
}