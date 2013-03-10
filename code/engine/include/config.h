#pragma once
#include <unordered_map>
#include <string>

namespace db {
	struct config {
		struct property {
			double double_val;
			float float_val;
			int int_val;
			std::string string_val;
			operator int();
			operator float();
			operator double();
			operator std::string();
		};

		std::unordered_map<std::string, property> values;
		config(const wchar_t* filename);
		void open(const wchar_t* filename);

		property& operator[](std::string name);
	};
}