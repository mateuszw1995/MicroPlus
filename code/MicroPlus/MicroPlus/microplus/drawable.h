#pragma once
#include <string>
#include "../db/graphics/gui/system.h"

struct drawable {
	std::wstring description;
	
	struct animation {

	};

	struct frame {
		db::graphics::gui::material mat;
		float ms;
	};
	std::vector<frame> frames;
};