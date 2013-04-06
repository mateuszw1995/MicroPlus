#pragma once
#include "../error/error.h"

namespace db {
	namespace window {
		class fpstimer;
	}
	namespace graphics {
		extern db::error errlog;
		extern window::fpstimer* fps;
		extern bool init(window::fpstimer*), deinit();

		/* gpu postprocess and separately cpu postprocess */
	}
}
