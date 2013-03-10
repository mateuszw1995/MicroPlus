#pragma once

#include "../../../Graphics/Shader/PostFXShader.h"
#include <cmath>
#include "../../../Math/maths.h"
extern GLint DB::Window::getWidth();
extern GLint DB::Window::getHeight();

__Shader::Component GaussianBlur::term[] = {
	__Shader::Component(0, 1, "funiform float", "kernel"),
	__Shader::Component(1, 2, "funiform float", "step"),
};

__Shader::Component 
	&GaussianBlur::KERNEL	(GaussianBlur::term[0]),
	&GaussianBlur::STEP 	(GaussianBlur::term[1]);


const int GaussianBlur::comps = sizeof(GaussianBlur::term)/sizeof(__Shader::Component);

void GaussianBlur::set_uni() {
	KERNEL.uni	= (uni_flags & KERNEL ?		&uni_kernel	: 0 );
	STEP.uni	= (uni_flags & STEP ?		&uni_step	: 0 );
}				

void GaussianBlur::set_names() {
	static GLfloat* kernel = new GLfloat[delta_width];
	math::kernel::Gaussian1D(radius, kernel, delta_width);
	
	__Shader::clr_names<GaussianBlur>(KERNEL);
	set_uni();
	KERNEL.act_name	= (KERNEL.arr = KERNEL.uni ? delta_width : 0) ?	_string::gen_name_suffixes(KERNEL.name, delta_width)  : _string::fv2sv(kernel, delta_width);
	*STEP.act_name	= (STEP.arr = (bool)STEP.uni) ?	STEP.name : _string::d2s(step);
}				

void GaussianBlur::Update(unsigned __int64 flags) {
	static GLfloat* kernel = new GLfloat[delta_width];
	sp.Use();
	if(flags & KERNEL) {
		math::kernel::Gaussian1D(radius, kernel, delta_width);
		uni_kernel.set(kernel);
	}
	if(flags & STEP) 
		uni_step.set(step);
	
}

GaussianBlur::GaussianBlur(dir orientation, _string* BlendMode, GLfloat max_radius, GLfloat _step) :
radius(max_radius), step(_step), delta_width(GLuint((3*ceil(max_radius))+1)), uni_kernel(delta_width), orient(orientation), PostFX(BlendMode, false) {
}

void GaussianBlur::Build() {
	_string vsrc, fsrc;
	
	__Shader::clr_use<GaussianBlur>();

	set_names();

    vsrc("\n varying vec2 texCoord;")
		("\n attribute vec2 at_Vertex;")
		("\n void main(void)")
		("\n {")
		("\n texCoord  = at_Vertex;")
		("\n gl_Position =  vec4(at_Vertex, 1.0, 1.0) * 2.0 - 1.0;")
		("\n }");

#define vUNI(u,i) (u.act_name[i])

    fsrc("\n uniform sampler2D diffuseMap;")
		("\n varying vec2 texCoord; ");

	__Shader::write_uni<GaussianBlur>(vsrc, fsrc);
	fsrc("\n void main(void)")
		("\n {")
		("\n 	vec4 sum = vec4(0.0);");
		const int dd = delta_width;
		switch(orient) {
		case HORIZONTAL: 
			for(int i = (1-dd); i<dd; ++i) { 
				fsrc("\n sum += ")vUNI(KERNEL, abs(i))(" * texture2D(diffuseMap, texCoord + vec2(")
				(_string::d2s(GLdouble((i))/GLdouble(DB::Window::getWidth())))("*")vUNI(STEP, 0)(", 0.0));");
			}
			break;
		case VERTICAL: 
			for(int i=  (1-dd); i<dd; ++i)
				fsrc("\n sum += ")vUNI(KERNEL, abs(i))(" * texture2D(diffuseMap, texCoord + vec2(0.0, ")
				(_string::d2s(GLdouble((i))/GLdouble(DB::Window::getHeight())))("*")vUNI(STEP, 0)("));");
			break;
		default: break;

		}
		fsrc(*BlendMode)
			("\n}");

#undef vUNI
		
		sp.vsrc = vsrc;
		sp.fsrc = fsrc;
		sp.Build();

		set_uni();
		__Shader::get_uni<GaussianBlur>(sp.p);
}