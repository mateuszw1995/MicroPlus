#include "stream.h"

namespace db {
	namespace misc {
		template <>
		std::string str<db::math::point>(db::math::point val) {
			std::ostringstream ss;
			ss << '[';
			ss << val.x;
			ss << ',';
			ss << val.y;
			ss << ']';
			return ss.str();
		}

		template <>
		std::string str<db::math::pointf>(db::math::pointf val) {
			std::ostringstream ss;
			ss << '[';
			ss << val.x;
			ss << ',';
			ss << val.y;
			ss << ']';
			return ss.str();
		}
	}
}