#pragma once
#include "../../Graphics/Postprocess.h"
#include "../../Graphics/Scene.h"

FX::Rect::Rect(GLfloat _x,GLfloat _y,GLfloat _w,GLfloat _h) : x(_x),y(_y),w(_w),h(_h) {}
FX::Rect::Rect() : x(0),y(0),w(0),h(0) {}
FX::Rect FX::Rect::FULLSCREEN = FX::Rect(0, 0, 0, 0);


void CPostProcessor::createFbo(GLuint t_width, GLuint t_height, GLuint& textureId, GLuint& fboId, GLuint& rboId, GLuint* depthTextureId){
	glGenTextures(1, &textureId);
	glGenRenderbuffersEXT(1, &rboId);
	glGenFramebuffersEXT(1, &fboId);

	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, t_width, t_height, 0,GL_BGRA, GL_UNSIGNED_BYTE, 0);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

	if(depthTextureId) { 
		glGenTextures(1, depthTextureId);
		glBindTexture(GL_TEXTURE_2D, *depthTextureId);			
		glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, t_width, t_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL );
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, *depthTextureId, 0); // attach it to the framebuffer
	}
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, textureId, 0);
	
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rboId);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, t_width, t_height);
	//glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rboId);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, rboId);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT){

	}
}
void CPostProcessor::Build(){
	createFbo(t_width, t_height, textureId, fboId, rboId, &depthTextureId);
}

CPostProcessor::CPostProcessor(GLuint _width, GLuint _height) 
	: t_width(_width), t_height(_height), blendFBO_created(false), active_layer(0)  {}

CPostProcessor::~CPostProcessor(){
	glDeleteFramebuffersEXT(1, &fboId);
	glDeleteRenderbuffersEXT(1, &rboId);
	glDeleteTextures(1, &textureId);
}

/****************************************************************************************************************************************/

void CPostProcessor::LastPass(PostFX* pass) {
	if(!active_layer || active_layer->BlendMode == PostFX::NORMAL)
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	Pass(pass);
}

void CPostProcessor::Pass(PostFX* pass) {
	pass->sp.Use();
	if(pass->depth) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, depthTextureId);
	}

	glCallList(CScene::fs_mesh);

	if(active_layer && active_layer->BlendMode != PostFX::NORMAL) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, b_textureId);
	}
}

void CPostProcessor::NewLayer(__PostFXShader* mixer) {
	active_layer = mixer;
	if(active_layer && active_layer->BlendMode != PostFX::NORMAL) {
		if(!blendFBO_created) {
			createFbo(t_width, t_height, b_textureId, b_fboId, b_rboId);
			blendFBO_created = true;
		}
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, b_fboId);
	}
}

void CPostProcessor::CombineLayer(bool notlast) {
	if(active_layer && active_layer->BlendMode != PostFX::NORMAL) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, b_textureId);

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, notlast ? fboId : 0);

		active_layer->Use();

		glCallList(CScene::fs_mesh);
	}
	if(!notlast) 
		glEnable(GL_DEPTH_TEST);
}