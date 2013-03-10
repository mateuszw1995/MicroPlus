#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <cmath>

#include <cstdio>
#include <ctime>

#include <winsock2.h>
using namespace std;

// OPENGL LIBRARIES
#include <GL/glew.h>

#include "Window/GLWindow.h"

/*
	LIBRARY DEPENDENCIES:
	
	Win32:

	user32.lib
	gdi32.lib
	ole32.lib
	Ws2_32.lib
	olepro32.lib

	All:

	OpenGL32.lib
	glew32.lib
*/

#include "Container/vector.h"
#include "Container/list.h"
#include "Container/registry.h"
#include "Container/bst.h"
#include "Graphics/vertex.h"

#include "Graphics/Texture.h"
#include "Config.h"
#include "Graphics/Scene.h"
#include "Graphics/GUI/Text.h"
#include "Graphics/GUI/GUI.h"

namespace DB {
	extern bool VBO_supported;
	
	enum ShaderType {
		SHADER_2_0, 
		SHADER_ARB
	};

	extern int OpenGLSet(ShaderType shadertype);


};






