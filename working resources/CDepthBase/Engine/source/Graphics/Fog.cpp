#pragma once
#include "../../Graphics/Shader/MeshShader.h"
#include "../../Graphics/Color.h"
 
__Shader::Component __MeshShader::Fog::term[] = {
	__Shader::Component(0, 1, "funiform vec4", "fcolor"),
	__Shader::Component(1, 2, "funiform float", "fdensity"),
};

__Shader::Component 
	&__MeshShader::Fog::COLOR		(__MeshShader::Fog::term[0]),
	&__MeshShader::Fog::DENSITY 	(__MeshShader::Fog::term[1]);

void __MeshShader::Fog::set_uni(__MeshShader::Fog::uniform_set& a) {
	COLOR.uni		= (uni_flags & COLOR ?		&a.u_color		: 0 );
	DENSITY.uni		= (uni_flags & DENSITY ?	&a.u_density	: 0 );
}

const int __MeshShader::Fog::comps = sizeof(__MeshShader::Fog::term)/sizeof(__Shader::Component); 

void __MeshShader::Fog::set_names() {
	set_uni(uniform_set());
	*COLOR.act_name		= (COLOR.arr = bool(COLOR.uni))     ?	COLOR.name	 : _string::vec4(color);
	*DENSITY.act_name	= (DENSITY.arr = bool(DENSITY.uni)) ?	DENSITY.name : _string::d2s(density);
}

void __MeshShader::Fog::Update(unsigned __int64 flags) {
	
	static _list<uniform_set>::Node* iter;
	iter = Updaters.first;
	while(iter){
		(*iter->_v.program).Use();

		if(flags & COLOR)
			iter->_v.u_color.set(color[0], color[1], color[2], color[3]); 
		if(flags & DENSITY) 
			iter->_v.u_density.set(density);
		
		iter = iter->next;
	}
}

__MeshShader::Fog::Fog(RGBA_1 col, float dense) {
	memcpy(color, &col, sizeof(RGBA_1));
	density = dense;
}