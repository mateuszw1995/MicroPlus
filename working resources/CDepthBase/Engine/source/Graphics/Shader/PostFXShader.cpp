#pragma once

#include "../../../Graphics/Shader/PostFXShader.h"

template <>
Registry<__PostFXShader>::Link::Link(const __PostFXShader& val) {
			node = Registry<__PostFXShader>::MakeResource(val); 
			++(node->_v.use);
			if(node->_v.univalent()) node->_v._v.Build();
}
Registry<__PostFXShader>	__PostFXShader::g_PostFXShaders = Registry<__PostFXShader>();

_string
	*PostFX::LIGHTEN =	 new _string("\n if(texcol.x > sum.x && texcol.y > sum.y && texcol.z > sum.z) gl_FragColor = texcol; else gl_FragColor = sum;"),
	*PostFX::DARKEN =	 new _string("\n if(texcol.x < sum.x && texcol.y < sum.y && texcol.z < sum.z) gl_FragColor = texcol; else gl_FragColor = sum;"),
	*PostFX::MULTIPLY =	 new _string("\n gl_FragColor = sum*texcol;"),
	*PostFX::DIVIDE =	 new _string("\n gl_FragColor = texcol/sum;"),
	*PostFX::SUBSTRACT = new _string("\n gl_FragColor = sum*sum - sum*texcol * (texcol/sum);"),
	*PostFX::NORMAL =	 new _string("\n gl_FragColor = sum;");

const _string __PostFXShader::FULLSCREEN_VSRC = 
	   "\n varying vec2 texCoord; \
		\n attribute vec2 at_Vertex; \
		\n void main(void) \
		\n { \
		\n texCoord  = at_Vertex; \
		\n gl_Position =  vec4(at_Vertex, 1.0, 1.0) * 2.0 - 1.0; \
		\n }";

PostFX::PostFX(_string* _BlendMode, bool _depth) : BlendMode(_BlendMode), depth(_depth) {
}

int __PostFXShader::operator==(const __PostFXShader& a) {
	return ( vsrc == a.vsrc && fsrc == a.fsrc ) ;
}

bool __PostFXShader::Use() {
	return __Shader::Use() ? __Shader::active_post_sp = this : false;
}
void __PostFXShader::Build() {
	if(imported) __Shader::~__Shader();
	compile();
	
	glBindAttribLocation(p, 0, "at_Vertex");
	link();
	Use();


	diffuseUni.loc = glGetUniformLocation(p, "diffuseMap");
	diffuseUni.set(0);
	blendUni.loc = glGetUniformLocation(p, "blendMap");
	blendUni.set(1);
	depthUni.loc = glGetUniformLocation(p, "depthMap");
	depthUni.set(2);
}

__PostFXShader::__PostFXShader() : __Shader(), BlendMode(0) {}
__PostFXShader::__PostFXShader(_string& vv, _string& ff) : __Shader(vv, ff), BlendMode(0) {}
__PostFXShader::__PostFXShader(_string* blendmode) : __Shader() {
	BlendMode = blendmode;
	vsrc = FULLSCREEN_VSRC;
    fsrc("\n uniform sampler2D diffuseMap;")
		("\n uniform sampler2D blendMap;")
		("\n varying vec2 texCoord; ")
		("\n void main(void)")
		("\n {")
		("\n vec4 texcol = texture2D(diffuseMap, texCoord);")
		("\n vec4 sum =    texture2D(blendMap, texCoord);")
		(*blendmode)//("\n gl_FragColor = sum;")
		("\n }");
}