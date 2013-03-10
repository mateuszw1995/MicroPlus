#pragma once
#include "../db/math/math.h"
#include "level.h"
#include "entity.h"
using namespace db::math;

struct game {
	rect_wh virtual_res, target_res;
	point camera_center;

	level* current_map;
	std::vector<entity*> entities;
};


