#pragma once
#include "../../../Graphics/Shader/MeshShader.h"

__Shader::Component __MeshShader::ParallaxMap::term[] = {
	__Shader::Component(0, 1, "funiform float", "scale", " = 0.04"),
	__Shader::Component(1, 2, "funiform float", "bias", " = -0.03"),
	__Shader::Component(2, 4, "funiform float", "steps"," = 8.0"),
};

__Shader::Component 
	&__MeshShader::ParallaxMap::SCALE		(__MeshShader::ParallaxMap::term[0]),
	&__MeshShader::ParallaxMap::BIAS	 	(__MeshShader::ParallaxMap::term[1]),
	&__MeshShader::ParallaxMap::STEPS	 	(__MeshShader::ParallaxMap::term[2]);

void __MeshShader::ParallaxMap::set_uni(__MeshShader::ParallaxMap::uniform_set& a) {
	SCALE.uni		= (uni_flags & SCALE			 ?	&a.u_scale	: 0 );
	BIAS.uni		= (!relief && (uni_flags & BIAS) ?	&a.u_bias	: 0 );
	STEPS.uni		= (relief && (uni_flags & STEPS) ?	&a.u_steps	: 0 );
}

const int __MeshShader::ParallaxMap::comps = sizeof(__MeshShader::ParallaxMap::term)/sizeof(__Shader::Component); 

void __MeshShader::ParallaxMap::set_names() {
	set_uni(uniform_set());
	
	*SCALE.act_name		= (SCALE.arr =  (bool)SCALE.uni) ?	SCALE.name	: _string::d2s(scale);
	*BIAS.act_name		= (BIAS.arr  =  (bool)BIAS.uni) ?	BIAS.name	: _string::d2s(bias);
	*STEPS.act_name		= (STEPS.arr =  (bool)STEPS.uni) ?	STEPS.name	: _string::d2s(steps);
}

void __MeshShader::ParallaxMap::Update(unsigned __int64 flags) {
	
	static _list<uniform_set>::Node* iter;
	iter = Updaters.first;
	while(iter){
		(*iter->_v.program).Use();

		if(flags & SCALE)
			iter->_v.u_scale.set(scale);
		if(flags & BIAS) 
			iter->_v.u_bias.set(bias);
		if(flags & STEPS) 
			iter->_v.u_steps.set(steps);
		
		iter = iter->next;
	}
}

__MeshShader::ParallaxMap::ParallaxMap(float _scale, float _bias) : relief(false), steps(0.0), scale(_scale), bias(_bias) {}

void __MeshShader::ParallaxMap::Relief(bool turn, float _steps) {
	steps = _steps;
	relief = turn;
}