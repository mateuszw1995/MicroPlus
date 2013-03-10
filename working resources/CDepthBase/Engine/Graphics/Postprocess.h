#pragma once

#include <cmath>
#include "../Defines.h"
#include "../Container/string.h"
#include "../Graphics/Shader/Shader.h"
#include "../Graphics/Shader/PostFXShader.h"
#include "../Math/maths.h"


namespace FX {
	enum FXeffect { 
	NONE = 0,
	GAUSSIAN_BLUR_V,
	GAUSSIAN_BLUR_H,
	MOTION_BLUR,
	ZOOM_BLUR,
	EDGE_DETECTION,
	EMBOSS
	};

	struct Rect {
		static Rect FULLSCREEN;
		GLfloat x,y,w,h;
		Rect(GLfloat,GLfloat,GLfloat,GLfloat);
		Rect(void);
	}; 

};

class CPostProcessor {
	friend struct CScene;
	//static Registry<CPostProcessor> g_PostProcessors;	
public: 
	static void createFbo(GLuint, GLuint, GLuint&, GLuint&, GLuint&, GLuint* = 0);
	__PostFXShader* active_layer;

	GLuint t_width;
	GLuint t_height;
	GLuint textureId;
	GLuint depthTextureId;

	GLuint fboId;
	GLuint rboId;

	GLuint b_textureId;

	GLuint b_fboId;
	GLuint b_rboId;

	bool blendFBO_created;

	CPostProcessor(GLuint _width, GLuint _height);
	~CPostProcessor();
	void Build();

	void NewLayer(__PostFXShader* = 0);
	void CombineLayer(bool notlast = true);
	void Pass(PostFX*);
	void LastPass(PostFX*);

};


/*
typedef Registry<CPostProcessor>::Link CPostProcessor;
Registry<CPostProcessor> CPostProcessor::g_PostProcessors = Registry<CPostProcessor>();


template <>
Registry<CPostProcessor>::Link::Link(const CPostProcessor& val){
			node = Registry<CPostProcessor>::MakeResource(val); 
			++(node->_v.use);
			if(node->_v.univalent()) node->_v._v.Build();
}

*/

