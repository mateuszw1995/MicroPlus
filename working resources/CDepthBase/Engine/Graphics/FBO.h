#pragma once
#include "../../Defines.h"
#include "../Graphics/Texture.h"

struct Frame {
	static GLuint act_id;
	GLuint id, tex_id[2], rbo_id;

	Frame(int width, int height, bool depth);
	~Frame();
	
	void ColorTexture(Texture*);
	void DepthTexture(Texture*);
	void Bind();
	void Clear();
	void ClearDepth();
	void ClearColor();
};