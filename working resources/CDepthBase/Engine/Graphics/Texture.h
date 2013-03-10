#pragma once
#include "../Defines.h"
#include "../Container/registry.h"
#include "../Container/string.h"
#include "../../Dependencies/bitmap/bitmap.h"

struct Texture{

	static Registry<Texture> g_texCache;

	GLuint texture;
	_string filenamez;

	bool imported;

	Texture(const char* filename);
	Texture(void);

	bool operator==(const Texture&);
	_string Get_Filename();

	void Import_Texture(const char*);
	void Build();

	~Texture();

} const EMPTY_TEXTURE;

typedef Registry<Texture>::Link CTexture;
template <>
Registry<Texture>::Link::Link(const Texture& val){
			node = Registry<Texture>::MakeResource(val); 
			++(node->_v.use);
			if(node->_v.univalent()) node->_v._v.Build();
}
