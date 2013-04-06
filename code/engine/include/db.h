#pragma once
/* 
					BLACK MESA RESEARCH FACILITY

					Subject: Gordon Freeman


LINKER INPUT:

	glew32.lib
	glu32.lib   // last error
	OpenGL32.lib
	Ws2_32.lib
	SOIL.lib
	dwmapi.lib
	freetype.lib
	Gdiplus.lib

	ps. moga sie roznic dla debug/release
*/


// db
	#include "config.h"
	// misc
//		#include "misc/stream.h"
//		#include "misc/undoredo.h"
		// container
//			#include "misc/container/circular_buffer.h" // cross-platform
		// container
	// misc

	//#include "error/error.h"
	#include "network/network.h" // Ws2_32.lib
	//#include "event.h"
	#include "threads/threads.h"
	#include "window/window.h" // OpenGL32.lib
	//#include "math/math.h"
	#include "math/graph.h"
	#include "graphics/graphics.h"
	// graphics
		// gui
			// text
				//#include "graphics/gui/text/drafter.h"
				//#include "graphics/gui/text/printer.h"
				#include "graphics/gui/text/text_rect.h"
			// text
			//#include "graphics/gui/system.h"
			//#include "graphics/gui/rect.h"
			#include "graphics/gui/scrollarea.h"
			#include "graphics/gui/textbox.h"
			//#include "graphics/gui/font.h"
		// gui
		
		//#include "graphics/pixel.h"
		// io
			// input
				//#include "graphics/io/texture.h"
				#include "graphics/io/vbo.h"
				#include "graphics/io/mesh.h"
		
			//#include "graphics/io/image.h" // Gdiplus.lib
	// graphics
	
	// misc
		#include "misc/value_animator.h"
	// misc
// db
#pragma warning (disable : 4099)