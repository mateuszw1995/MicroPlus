#pragma once
class _string;
#include "Container/string.h"
#include "Container/vector.h"
struct Config
{
	struct segment{
		_string tag;

		enum segtype_tag{
			STRING,INTEGER,FLOAT
		} segtype;

		int intv;
		_string strv;
		float flov;

		segment(_string, segtype_tag stype = STRING, int intval = 0, float fval = 0.0, _string strval = "");
		segment(void);
		int operator>(const segment&);
		int operator<(const segment&);
		int operator==(const segment&);
	};

	static int seg_cmp(const void*, const void*);
	_vector<segment> Values;

	Config(_string);
	segment* operator[](char*);
};
