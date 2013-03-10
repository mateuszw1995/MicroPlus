#pragma once
#include "../../Graphics/Mesh.h"
#include "../../Math/Transformation.h"
#define BUFFER_OFFSET(i) ((char*)NULL + (i))

VBOMesh::VBOMesh() : buffered(false) {
}
void VBOMesh::Build(){
	if(!(CMesh::options & CACHE_FOR_REBUILD) && buffered) return;
	clear_buf();

	tan_offset = vertices.size()*sizeof(Vertex);
	qind_offset = indices.size()*sizeof(triangle);

	char* vertbuf = (char*)malloc(tan_offset+tangents.size()*sizeof(tangent));
	char* indbuf  = (char*)malloc(qind_offset+q_indices.size()*sizeof(quad));

	memcpy(vertbuf, vertices.get_ptr(), tan_offset);
	memcpy(vertbuf+tan_offset, tangents.get_ptr(), tangents.size()*sizeof(tangent));
	memcpy(indbuf, indices.get_ptr(), qind_offset);
	memcpy(indbuf+qind_offset, q_indices.get_ptr(), q_indices.size()*sizeof(quad));
	
	glGenBuffers(1, &VBO_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_buffer);
	glBufferData(GL_ARRAY_BUFFER, tan_offset+tangents.size()*sizeof(tangent), vertbuf, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, qind_offset+q_indices.size()*sizeof(quad), indbuf, GL_STATIC_DRAW);
	
	if(indices.size() > 0)
		buffered |= 1;
	if(q_indices.size() > 0)
		buffered |= 2;
	if(tangents.size() > 0)
		buffered |= 4;
	
	if(!(CMesh::options & CACHE_FOR_REBUILD)){
		vertices.clear();
		indices.clear();
		mtl_indices.clear();
		q_indices.clear();
		q_mtl_indices.clear();
		tangents.clear();
	}
	free(vertbuf);
	free(indbuf);
}
VBOMesh::~VBOMesh(){
	clear_buf();
}
void VBOMesh::clear_buf(){
	glDeleteBuffers(1, &VBO_buffer);
	if(buffered & 1 || buffered & 2)
		glDeleteBuffers(1, &VBO_indices);
	buffered = 0;

}
void VBOMesh::Call(){
	using namespace rawMatrix;
	static GLfloat mmat[16];
	static GLfloat smmat[16];
	static GLfloat mpmat[16];
	static DrawCall** dc;
	static _list<CLight*>::Node* iter;
	static _list<CLight::uniform_set>::Node* s_iter;

	if(!buffered || !visible) return;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_indices); 

	glVertexAttribPointer(__MeshShader::ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);  

	if(!__Shader::depth_buffer_mode){
		glVertexAttribPointer(__MeshShader::ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),  BUFFER_OFFSET(sizeof(vec3)));  
		glVertexAttribPointer(__MeshShader::ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),  BUFFER_OFFSET(sizeof(vec3)+ sizeof(vec2)));  

		if(buffered & 4) { 
			glVertexAttribPointer(__MeshShader::ATTRIB_TANGENT, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(tan_offset));   
			glEnableVertexAttribArray(__MeshShader::ATTRIB_TANGENT);  
		}
		else 
			glDisableVertexAttribArray(__MeshShader::ATTRIB_TANGENT);  
	}

	dc = drawCalls.get_ptr();
	for(int i = 0; i < drawCalls.size(); ++i, ++dc) {
		Matrix<GLfloat>& modelView = (*dc)->modelView;
		if(__Shader::depth_buffer_mode){
			if(!cast_shadows) return;

			mult(CScene::CurrentScene->shadow_model.MATRIX, modelView.MATRIX, mmat);
			mult(CScene::CurrentScene->shadow_proj.MATRIX, mmat, mpmat);

			(*CScene::CurrentScene->depthbuf_shader).modelViewProjection.set(mpmat);
		}
		else {
			mult(CScene::CurrentScene->camera->modelView.matrix.MATRIX, modelView.MATRIX, mmat);
			mult(CScene::CurrentScene->camera->projection.MATRIX, mmat, mpmat);
			iter = CScene::CurrentScene->Attachments.Lights.first;
			while(iter){
				mult(iter->_v->tex_mat.MATRIX, modelView.MATRIX, smmat);
				s_iter = iter->_v->Updaters.first;
				while(s_iter) { 
					(*s_iter->_v.program).Use();
					s_iter->_v.texmat.set(smmat);
					s_iter = s_iter->next;
				}
				iter = iter->next;
			}
		}



		submesh* iter = submeshes.get_ptr(); 
		submesh* end_iter = submeshes.get_ptr()+submeshes.size(); 
		if(buffered & 1){
			while(iter != end_iter){ 

				if(!__Shader::depth_buffer_mode) {
					if(iter->ShaderProgram.node && !__Shader::depth_buffer_mode) {
						(*iter->ShaderProgram).Use();
						(*iter->ShaderProgram).modelView.set(mmat);
						(*iter->ShaderProgram).modelViewProjection.set(mpmat);
					}
					else 
						__Shader::UseFixed();

					if(iter->mtl_ID.node)
						(*iter->mtl_ID).MakeCurrent();
					else (*DEFAULT_MATERIAL).MakeCurrent();
				}

				glDrawElements(GL_TRIANGLES, iter->count*3, GL_UNSIGNED_INT, BUFFER_OFFSET(iter->startindex * 3 * sizeof(GLuint)));

				++iter;
			}
		}
		if(buffered & 2){
			iter = q_submeshes.get_ptr(); 
			end_iter = q_submeshes.get_ptr()+q_submeshes.size(); 
			while(iter != end_iter){ 

				if(!__Shader::depth_buffer_mode) {
					if(iter->ShaderProgram.node){
						(*iter->ShaderProgram).Use();
						(*iter->ShaderProgram).modelView.set(mmat);
						(*iter->ShaderProgram).modelViewProjection.set(mpmat);
					}
					else 
						__Shader::UseFixed();

					if(iter->mtl_ID.node)
						(*iter->mtl_ID).MakeCurrent();
					else (*DEFAULT_MATERIAL).MakeCurrent();
				}

				glDrawElements(GL_QUADS, iter->count*4, GL_UNSIGNED_INT, BUFFER_OFFSET(qind_offset + iter->startindex*4*sizeof(GLuint)));
				++iter;
			}
		}
	}
}

void VBOMesh::UncastShadows() {
	cast_shadows = false;
}
void VBOMesh::CastShadows() {
	cast_shadows = true;
}