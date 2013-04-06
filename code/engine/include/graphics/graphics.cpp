#pragma once
#include "graphics.h"
#include <gl/glew.h>
#include <Windows.h>
#include <gdiplus.h>
#include "../error/errmacro.h"

//#include "io/vbo.h"

namespace db {
	namespace graphics {
		db::error errlog(gl_last_error, gl_last_error);
		window::fpstimer* fps = 0;

		Gdiplus::GdiplusStartupInput gdi;
		ULONG_PTR           gdit;

		bool init(window::fpstimer* f) {
			fps = f;
			Gdiplus::GdiplusStartup(&gdit, &gdi, NULL);
			bool r = errs(glewInit() == GLEW_OK, "Create at least 1 window::glwindow before calling graphics::init") != 0;
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			//io::input::vbo::vbo_supported = GLEW_ARB_vertex_buffer_object;
			return r;
		}

		bool deinit() {
			Gdiplus::GdiplusShutdown(gdit);
			return true;
		}
	}
}