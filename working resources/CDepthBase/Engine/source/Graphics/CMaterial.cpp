#pragma once

#include "../../Graphics/Mesh.h"
CMaterial DEFAULT_MATERIAL = CMaterial(__CMaterial());
#define MTLS_EQUAL(mtl1, mtl2) ((!memcmp(&mtl1, &mtl2, sizeof(GLfloat)*13)) && \
	(*mtl1.diffuseMap).texture == (*mtl2.diffuseMap).texture &&\
	(*mtl1.normalMap).texture == (*mtl2.normalMap).texture &&\
	(*mtl1.heightMap).texture == (*mtl2.heightMap).texture &&\
	(*mtl1.specularMap).texture == (*mtl2.specularMap).texture ) 
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
__CMaterial::__CMaterial(	RGBA_1 _ambient, 
							RGBA_1 _diffuse, 
							RGBA_1 _specular, 
							GLfloat _shininess, 
							CTexture* _diffuseMap, 
							CTexture* _normalMap, 
							CTexture* _heightMap, 
							CTexture* _specularMap){
		memcpy(ambient, &_ambient, sizeof(RGBA_1));
		memcpy(diffuse, &_diffuse, sizeof(RGBA_1));
		memcpy(specular, &_specular, sizeof(RGBA_1));
		shininess = _shininess;
	    diffuseMap	= (_diffuseMap ?  (*_diffuseMap) : CTexture(EMPTY_TEXTURE));
	    normalMap	= (_normalMap ?   (*_normalMap) : CTexture(EMPTY_TEXTURE));
	    heightMap	= (_heightMap ?   (*_heightMap) : CTexture(EMPTY_TEXTURE));
	    specularMap = (_specularMap ? (*_specularMap) : CTexture(EMPTY_TEXTURE));
}
 
int operator<(const CMaterial& a, const CMaterial& b){
	return a.node < b.node;
}

/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
void __CMaterial::MakeCurrent(){
	
	static GLfloat whiteee[4] = { 1.0, 1.0, 1.0, 1.0 };
	static GLfloat blaaack[4] = { 0.0, 0.0, 0.0, 0.0 };
	static unsigned __int64 flags;
	if(__Shader::active_sp){
		
	static _list<__MeshShader::DepthMap>::Node* iter;
	iter = __Shader::active_mesh_sp->ShadowMaps.first;
	while(iter) {
		glActiveTexture(GL_TEXTURE0 + iter->_v.texUnit);
		
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, whiteee);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, iter->_v.depthTexId);
				iter = iter->next;
	}
	__MeshShader::Material* m = __Shader::active_mesh_sp->compo_set.material;
	if(m) {
		m->u_diffuse.set(diffuse[0], diffuse[1], diffuse[2], diffuse[3]);
		m->u_ambient.set(ambient[0], ambient[1], ambient[2], ambient[3]);
		m->u_specular.set(specular[0], specular[1], specular[2], specular[3]);
		m->u_shininess.set(shininess);
	}

	if(diffuse[3] < 1.0 || ambient[3] < 1.0 || specular[3] < 1.0)
		glEnable(GL_BLEND);
	else 
		glDisable(GL_BLEND);

	}
	flags = __MeshShader::active_mesh_sp->compo_set._flags;
	if(diffuseMap.node && (*diffuseMap).imported && flags & __MeshShader::DIFFUSE_MAPPING){
			glActiveTexture(GL_TEXTURE0);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glBindTexture(GL_TEXTURE_2D, (*diffuseMap).texture); 
			
	}
	
	if(normalMap.node && (*normalMap).imported && flags & __MeshShader::NORMAL_MAPPING ){
			glActiveTexture(GL_TEXTURE1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glBindTexture(GL_TEXTURE_2D, (*normalMap).texture); 
	}

	if(heightMap.node && (*heightMap).imported &&  __Shader::active_mesh_sp->compo_set.parallax ){
			glActiveTexture(GL_TEXTURE2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glBindTexture(GL_TEXTURE_2D, (*heightMap).texture); 
	}

	if(specularMap.node && (*specularMap).imported && flags & __MeshShader::SPECULAR_MAPPING ){
			glActiveTexture(GL_TEXTURE3);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glBindTexture(GL_TEXTURE_2D, (*specularMap).texture); 
	}
}

bool __CMaterial::operator==(const __CMaterial& mtl2){
	return ((!memcmp(this, &mtl2, sizeof(GLfloat)*13)) && 
	diffuseMap.node == mtl2.diffuseMap.node &&
	normalMap.node == mtl2.normalMap.node &&
	heightMap.node == mtl2.heightMap.node &&
	specularMap.node == mtl2.specularMap.node ) ;
}