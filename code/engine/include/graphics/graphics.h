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

// gui
	#include "gui/system.h"
	#include "gui/rect.h"
	#include "gui/scroll.h"
	#include "gui/text.h"
	#include "gui/font.h"
// gui

#include "pixel.h"
// io
	// input
		#include "io/texture.h"
		#include "io/vbo.h"
		#include "io/mesh.h"

	#include "io/image.h" // Gdiplus.lib