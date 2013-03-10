#pragma once
#include "config.h"
#include <fstream>

namespace db {
	config::property::operator int() {
		return int_val;
	}

	config::property::operator float() {
		return float_val;
	}

	config::property::operator double() {
		return double_val;
	}

	config::property::operator std::string() {
		return string_val;
	}

	config::config(const wchar_t* filename) {
		open(filename);
	}

	void config::open(const wchar_t* filename) {
		std::ifstream in(filename);
		std::string name;
		while(in.good()) {
			property prop;
			in >> name;
			     if(name == "string")	{ in >> name; in >> prop.string_val; }
			else if(name == "int")		{ in >> name; in >> prop.int_val;	 }
			else if(name == "float")	{ in >> name; in >> prop.float_val;	 }
			else if(name == "double")	{ in >> name; in >> prop.double_val; }

			values[name] = prop;
		}
		in.close();
	}

	config::property& config::operator[](std::string name) {
		return values[name];
	}

}
