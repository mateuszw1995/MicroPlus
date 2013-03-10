#pragma once
#include <vector>
#include "../db/graphics/gui/system.h"
#include "drawable.h"

struct entity : public drawable {
	int x, y, h;

	void draw(std::vector<db::graphics::gui::quad>&);
};

struct item : public entity {
	bool countable;
	int weight, quantity;
};

struct creature : public entity {
	int hp, speed;
};

struct character : public entity {

};