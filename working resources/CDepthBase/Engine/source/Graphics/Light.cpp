#pragma once
/***********************************************************************************************************************/
/***********************************************************************************************************************/

#include "../../Graphics/Scene.h"
#include "../../Graphics/Light.h"
#include "../../Graphics/Shader/uniform_overloads.h"


__Shader::Component CLight::Effect::term[] = {
	__Shader::Component(0, 8, "vuniform vec3", "position"),
	__Shader::Component(1, 16, "vuniform float", "radius"),
	__Shader::Component(2, 32, "funiform vec2", "cos_angles"),
	__Shader::Component(3, 64, "vuniform vec3", "spotDir"),
	__Shader::Component(4, 128, "vuniform mat4", "TextureMatrix")
};

__Shader::Component 
	&CLight::POSITION 		(CLight::Effect::term[0]),
	&CLight::RADIUS 		(CLight::Effect::term[1]),
	&CLight::COS_ANGLES 	(CLight::Effect::term[2]),
	&CLight::SPOTDIR 		(CLight::Effect::term[3]),
	&CLight::TEXTURE_MATRIX (CLight::Effect::term[4]);


__Shader::SwitchComponent CLight::Switch::term[] = {
	__Shader::SwitchComponent(0, 1, "funiform vec4", "l_diffuse"),
	__Shader::SwitchComponent(1, 2, "funiform vec4", "l_ambient"),
	__Shader::SwitchComponent(2, 4, "funiform vec4", "l_specular")
};

__Shader::SwitchComponent 
	&CLight::DIFFUSE		(CLight::Switch::term[0]),
	&CLight::AMBIENT 		(CLight::Switch::term[1]),
	&CLight::SPECULAR 		(CLight::Switch::term[2]);


void CLight::set_uni(uniform_set& a) {
	DIFFUSE.uni		= (uni_flags & use_flags & DIFFUSE ?					&a.diffuse		: 0 );
	AMBIENT.uni		= (uni_flags & use_flags & AMBIENT ?					&a.ambient		: 0 );
	SPECULAR.uni	= (uni_flags & use_flags & SPECULAR ?					&a.specular		: 0 );
	POSITION.uni	= (uni_flags & POSITION ?								&a.position		: 0 );
	RADIUS.uni		= (uni_flags & RADIUS		&& _type != __dir ?			&a.radius		: 0 );
	COS_ANGLES.uni	= (uni_flags & COS_ANGLES	&& _type == __spot ?		&a.cos_angles	: 0 );
	SPOTDIR.uni		= (uni_flags & SPOTDIR		&& _type == __spot ?		&a.spotdir		: 0 );
	
	TEXTURE_MATRIX.uni = (shadows ?											&a.texmat		: 0 );
}				

const int CLight::Effect::comps = sizeof(CLight::Effect::term)/sizeof(__Shader::Component);
const int CLight::Switch::comps = sizeof(CLight::Switch::term)/sizeof(__Shader::SwitchComponent);

void CLight::set_names() {
	set_uni(uniform_set());

	if(use_flags & DIFFUSE)  *DIFFUSE.act_name	  =  DIFFUSE.uni		? DIFFUSE.name		+ dosuffix(DIFFUSE.arr++) : _string::vec4(&diffuse.r) ;
	if(use_flags & AMBIENT)  *AMBIENT.act_name	  =  AMBIENT.uni		? AMBIENT.name		+ dosuffix(AMBIENT.arr++) : _string::vec4(&ambient.r) ;
	if(use_flags & SPECULAR) *SPECULAR.act_name	  =  SPECULAR.uni		? SPECULAR.name		+ dosuffix(SPECULAR.arr++): _string::vec4(&specular.r) ;
	
	*POSITION.act_name	 =  POSITION.uni	? POSITION.name		+ dosuffix(POSITION.arr++) : _string::vec3(&position.x) ;
	*RADIUS.act_name	 =  RADIUS.uni		? RADIUS.name		+ dosuffix(RADIUS.arr++) : _string::d2s(radius) ;
	*COS_ANGLES.act_name =  COS_ANGLES.uni	? COS_ANGLES.name	+ dosuffix(COS_ANGLES.arr++) : _string::vec2(cos_angles) ;
	*SPOTDIR.act_name	 =  SPOTDIR.uni		? SPOTDIR.name		+ dosuffix(SPOTDIR.arr++) : _string::vec3(&lookat.x) ;
	
	*TEXTURE_MATRIX.act_name = TEXTURE_MATRIX.uni ? TEXTURE_MATRIX.name + dosuffix(TEXTURE_MATRIX.arr++)  : "" ;
}				

CLight::CLight(
	DIRECTIONAL, RGBA_1 a, RGBA_1 d, RGBA_1 s, Vec3D<> p) : _type(__dir) {
		__base(a,d,s,p);
}

CLight::CLight(
	POINT, RGBA_1 a, RGBA_1 d, RGBA_1 s, Vec3D<> p,
	float r) : radius(1.0f/r), _type(__point) {
		__base(a,d,s,p);
}

CLight::CLight(
	SPOT, RGBA_1 a, RGBA_1 d, RGBA_1 s, Vec3D<> p,Vec3D<> sdir, 
	float r,
	float cos_inner,
	float cos_outer) : radius(1.0f/r), _type(__spot), lookat(sdir) {
		cos_angles[0] = (float)cos(cos_inner * math::var<double>::PI_OVER_180);
		cos_angles[1] = (float)cos(cos_outer * math::var<double>::PI_OVER_180);
		__base(a,d,s,p);
}

CLight::uniform_set::uniform_set() : texmat(1) {
}
void CLight::Update(unsigned __int64 flags) {
	
	static float m_position[3];
	static float m_spotvec[3];
	static _list<uniform_set>::Node* iter;

	if(flags & CLight::POSITION)
		rawMatrix::multiply_point(CScene::CurrentScene->camera->modelView.matrix.MATRIX, &position.x, m_position, (_type == __dir ? 0.0f : 1.0f) );
	if(flags & CLight::SPOTDIR && _type == __spot)
		rawMatrix::multiply_point(CScene::CurrentScene->camera->modelView.matrix.MATRIX, &lookat.x, m_spotvec, 0.0f );

	iter = Updaters.first;

	while(iter){
		(*iter->_v.program).Use();

		if(flags & AMBIENT)
			iter->_v.ambient.set(ambient.r, ambient.g, ambient.b, ambient.alpha); 
		if(flags & DIFFUSE)	
			iter->_v.diffuse.set(diffuse.r, diffuse.g, diffuse.b, diffuse.alpha); 
		if(flags & SPECULAR)
			iter->_v.specular.set(specular.r, specular.g, specular.b, specular.alpha); 
		if(flags & POSITION) 
			iter->_v.position.set(m_position[0], m_position[1], m_position[2]); 
		if(flags & RADIUS && _type != __dir)
			iter->_v.radius.set(radius);
		if(_type == __spot) {
			if(flags & SPOTDIR)
				iter->_v.spotdir.set(m_spotvec[0], m_spotvec[1], m_spotvec[2]);
			if(flags & COS_ANGLES)
				iter->_v.cos_angles.set(cos_angles[0], cos_angles[1]);
		}
		iter = iter->next;
	}


}


void CLight::__base(RGBA_1 a, RGBA_1 d, RGBA_1 s, Vec3D<> p) {
	max_delta = 0;	
	shadows = false;
	PCFmode = CLight::NONE;
	ambient = a;
	diffuse = d;
	specular = s;
	position = p;
}

CLight::~CLight(){
}

void CLight::set_texture_matrix(){
	static float mmat[16];
	static const GLfloat bias[16] = {	
			0.5, 0.0, 0.0, 0.0, 
			0.0, -0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0};

	rawMatrix::mult(bias, shadow_proj.MATRIX, mmat);
	rawMatrix::mult(mmat, CScene::CurrentScene->shadow_model.MATRIX, tex_mat.MATRIX);
}



void CLight::Init_PCF(GLuint maxdelta, GLfloat step, PCFMode pcfmode){
	PCFmode = pcfmode;
	blurStep = step;
	max_delta = maxdelta;
}
void CLight::Init_Shadows(int shadowMapWidth, int shadowMapHeight) {

	shadows = true;
	sMapW = shadowMapWidth;
	sMapH = shadowMapHeight;
	GLenum FBOstatus;
	
	shadow_proj.identity();

	glGenFramebuffersEXT(1, &fboId);													
	glGenTextures(1, &depthTextureId);													
	glGenRenderbuffersEXT(1, &color_rb);	
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);	

	glBindTexture(GL_TEXTURE_2D, depthTextureId);					
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY); 		

	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, sMapW, sMapH, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL );
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, depthTextureId, 0); 
	
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, color_rb);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,GL_RGBA,sMapW, sMapH);	
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,GL_RENDERBUFFER_EXT, color_rb);

	FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT){ 
		DB::Window::MBX("Error", "Could not init FBO."); 
		exit(0); 
	}
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
}


/***********************************************************************************************************************/
/***********************************************************************************************************************/
