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
		#include "misc/stream.h"
		#include "misc/undoredo.h"
		// container
			#include "misc/container/circular_buffer.h" // cross-platform
		// container
	// misc

	#include "error/error.h"
	#include "network/network.h" // Ws2_32.lib
	#include "event.h"
	#include "threads/threads.h"
	#include "window/window.h" // OpenGL32.lib
	#include "math/math.h"
	#include "math/graph.h"
	#include "graphics/graphics.h"
	// misc
		#include "misc/value_animator.h"
	// misc
// db

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	using namespace db::window;
	glwindow::hinst = hInstance;
	glwindow::wcl.cbSize = sizeof(glwindow::wcl);
	glwindow::wcl.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	glwindow::wcl.lpfnWndProc = glwindow::wndproc;
	glwindow::wcl.cbClsExtra = 0;
	glwindow::wcl.cbWndExtra = 0;
	glwindow::wcl.hInstance = hInstance;
	glwindow::wcl.hIcon = LoadIcon(0, IDI_APPLICATION);
	glwindow::wcl.hCursor = LoadCursor(0, IDC_ARROW);
	glwindow::wcl.hbrBackground = 0;
	glwindow::wcl.lpszMenuName = 0;
	glwindow::wcl.lpszClassName = "DepthBaseWindow";
	glwindow::wcl.hIconSm = 0;

	if (!RegisterClassEx(&glwindow::wcl))
		return 0;

	return db::main();
}

#pragma warning (disable : 4099)