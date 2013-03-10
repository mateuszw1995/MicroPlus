#pragma once
#include "../../Graphics/FBO.h"

GLuint Frame::act_id = 0;

Frame::Frame(int width, int height, bool depth) {
	if(depth) {
		glGenRenderbuffersEXT(1, &rbo_id);
	}
	glGenFramebuffersEXT(1, &id);

}
Frame::~Frame() {

}

void Frame::Bind() {
	if(act_id != id) {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, id);
		id = act_id;
	}
}
void Frame::Clear() {
	Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Frame::ClearDepth() {
	Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
}
void Frame::ClearColor() {	
	Bind();
	glClear(GL_COLOR_BUFFER_BIT);
}

void Frame::ColorTexture(Texture* t) {

}
void Frame::DepthTexture(Texture* t) {

}