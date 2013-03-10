#pragma once
#include "../../Graphics/Mesh.h"

void CMesh::import3DS(_string filename){
	struct Chunk {
		unsigned id:2;
		unsigned length:4;
	};
	
	FILE* f3ds = fopen(filename.c_str(), "rb");
	if(!f3ds) return;



}