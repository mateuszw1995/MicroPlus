#pragma once
#include <vector>
#include "../db/math/math.h"
#include "tile.h"
#include "entity.h"

class level {
	struct header {
		unsigned version;
		rect_wh size;
		unsigned tiletypes;
	};

	struct tilechunk {

	};

public:

	db::math::rect_wh size;
	std::vector<tiletype> tiletypes;

	void open(const wchar_t* filename);
	void save(const wchar_t* filename);
};