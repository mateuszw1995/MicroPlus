#pragma once

#include "Shader.h"


class __PostFXShader  : public __Shader { 
public:
	static Registry<__PostFXShader> g_PostFXShaders;
	int operator==(const __PostFXShader&);
	
	void Build();
	bool Use();

	Uniform1i diffuseUni,
			  blendUni,
			  depthUni;

	__PostFXShader();
	__PostFXShader(_string&, _string&);
	__PostFXShader(_string*);

	_string* BlendMode;

	static const _string FULLSCREEN_VSRC;
};

struct PostFX {
	static _string 
		*LIGHTEN, 
		*DARKEN,
		*MULTIPLY,
		*DIVIDE,
		*SUBSTRACT, 
		*NORMAL;

	_string* BlendMode;

	__PostFXShader sp;
	bool depth;

	PostFX(_string*, bool);

	virtual void Build() = 0;
};

struct GaussianBlur : public __Shader::Effect, public PostFX {
	static __Shader::Component term[];
	static const int comps;
	const GLuint delta_width;
	GLfloat radius, step;

	Uniform1fv uni_kernel;
	Uniform1f uni_step;

	enum dir {
		HORIZONTAL,
		VERTICAL
	} orient;

	static void Kernel(GLdouble, GLfloat*, GLint);

	static __Shader::Component& KERNEL, &STEP;

	void set_uni();
	void set_names();

	void Update(unsigned __int64);
	GaussianBlur(dir orientation, _string* BlendMode, GLfloat max_radius, GLfloat _step);

	void Build(); 
};

struct MedianFilter : public __Shader::Effect, public PostFX {
	static __Shader::Component term[];
	static const int comps;
	const GLuint delta_width;
	GLfloat step;

	Uniform1f uni_step;

	static __Shader::Component &STEP;
	
	void set_uni();
	void set_names();

	void Update(unsigned __int64);
	MedianFilter(_string* BlendMode, GLint radius, GLfloat step);

	void Build(); 
};

struct DepthOfField : public __Shader::Effect, public PostFX {

	DepthOfField();
};

struct SSAmbientOcclusion : public __Shader::Effect, public PostFX {

	SSAmbientOcclusion();
};


typedef Registry<__PostFXShader>::Link PostFXShader;

template <>
Registry<__PostFXShader>::Link::Link(const __PostFXShader& val);