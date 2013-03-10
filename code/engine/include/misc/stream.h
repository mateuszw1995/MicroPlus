#pragma once
#include <string>
#include <sstream>
#include "../math/math.h"

namespace db {
	namespace misc {

		/* number to string conversion */

		template <class T>
		std::string str(T val) {
			std::ostringstream ss;
			ss << val;
			return ss.str();
		}
		
		/* point to string conversion */

		template <> std::string str<db::math::point>(db::math::point val);
		template <> std::string str<db::math::pointf>(db::math::pointf val);
		
		/* number to wide string conversion */

		template <class T>
		std::wstring wstr(T val) {
			std::wostringstream ss;
			ss << val;
			return ss.str();
		}
		
		/* wide string to number conversion */

		template <class T>
		T wnum(std::wstring& s) {
			std::wistringstream ss(s);
			T v;
			ss >> v;
			return v;
		}
		
		/* string to number conversion */

		template <class T>
		T num(std::string& s) {
			std::istringstream ss(s);
			T v;
			ss >> v;
			return v;
		}
	}
}