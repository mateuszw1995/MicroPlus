#pragma once

#include "../../Graphics/Texture.h"

Registry<Texture> Texture::g_texCache = Registry<Texture> ();


/****************************************************************************************************/

Texture::~Texture(){
		if(imported){
			glDeleteTextures(1, &texture);
			imported = false;
		}
	}

/****************************************************************************************************/
_string Texture::Get_Filename(){
	return filenamez;
}
/****************************************************************************************************/

bool Texture::operator==(const Texture& c){
	return !strcmp(filenamez.cstr, c.filenamez.cstr);
}

/****************************************************************************************************/
Texture::Texture(void) : imported(false), filenamez("") {}
/****************************************************************************************************/
Texture::Texture(const char* filename)  : imported(false) {
	Import_Texture(filename);
}

/****************************************************************************************************/
void Texture::Import_Texture(const char* filename){
	filenamez = filename;
}
/****************************************************************************************************/
void Texture::Build(){	
	if(imported || filenamez.length() < 1) return;

	Bitmap bitmap;
	if (bitmap.loadPicture(filenamez.c_str()))
    {
        bitmap.flipVertical();

        glPixelStorei(GL_UNPACK_ALIGNMENT,4);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, bitmap.width, bitmap.height,
            0, GL_BGRA, GL_UNSIGNED_BYTE, bitmap.getPixels());

		
	}
	else {
		DB::Window::MBX(filenamez.c_str(), "Could not load texture file!") ;
		exit(1);
	}   
	imported = true;
}

/****************************************************************************************************/
