#pragma once
#include "../../../Graphics/Shader/MeshShader.h"

__Shader::Component __MeshShader::Material::Effect::term[] = {
	__Shader::Component(0, 1, "funiform float", "m_shininess")
};

__Shader::Component 
	&__MeshShader::Material::SHININESS ( __MeshShader::Material::Effect::term[0] );


__Shader::SwitchComponent __MeshShader::Material::Switch::term[] = {
	__Shader::SwitchComponent(0, 2, "funiform vec4", "m_diffuse", " = vec4(0.8, 0.8, 0.8, 1.0)"),
	__Shader::SwitchComponent(1, 4, "funiform vec4", "m_ambient", " = vec4(0.0, 0.0, 0.0, 1.0)"),
	__Shader::SwitchComponent(2, 8, "funiform vec4", "m_specular", " = vec4(0.0, 0.0, 0.0, 1.0)")
};

__Shader::SwitchComponent 
	&__MeshShader::Material::DIFFUSE ( __MeshShader::Material::Switch::term[0] ),
	&__MeshShader::Material::AMBIENT ( __MeshShader::Material::Switch::term[1] ),
	&__MeshShader::Material::SPECULAR( __MeshShader::Material::Switch::term[2] );

const int __MeshShader::Material::Effect::comps = sizeof(__MeshShader::Material::Effect::term)/sizeof(__Shader::Component);
const int __MeshShader::Material::Switch::comps = sizeof(__MeshShader::Material::Switch::term)/sizeof(__Shader::SwitchComponent);

__MeshShader::Material::Material(RGBA_1 diff, RGBA_1 amb, RGBA_1 spec, float shin) : shininess(shin) {
	memcpy(diffuse, &diff, sizeof(RGBA_1));
	memcpy(ambient, &amb, sizeof(RGBA_1));
	memcpy(specular, &spec, sizeof(RGBA_1));
}

void __MeshShader::Material::set_uni() {
	DIFFUSE.uni		= (uni_flags & use_flags & DIFFUSE ?	&u_diffuse	: 0 );
	AMBIENT.uni		= (uni_flags & use_flags & AMBIENT ?	&u_ambient	: 0 );
	SPECULAR.uni	= (uni_flags & use_flags & SPECULAR ?	&u_specular	: 0 );
	SHININESS.uni	= (uni_flags & SHININESS ?				&u_shininess: 0 );
}


void __MeshShader::Material::set_names() {
	set_uni();
	if(use_flags & DIFFUSE) *DIFFUSE.act_name	= (DIFFUSE.arr = (bool)DIFFUSE.uni) ?		DIFFUSE.name	: _string::vec4(diffuse)  ;
	if(use_flags & AMBIENT) *AMBIENT.act_name	= (AMBIENT.arr = (bool)AMBIENT.uni) ?		AMBIENT.name	: _string::vec4(ambient)  ;
	if(use_flags & SPECULAR)*SPECULAR.act_name	= (SPECULAR.arr = (bool)SPECULAR.uni) ?		SPECULAR.name	: _string::vec4(specular) ;
							*SHININESS.act_name	= (SHININESS.arr = (bool)SHININESS.uni) ?	SHININESS.name	: _string::d2s (shininess);
}
