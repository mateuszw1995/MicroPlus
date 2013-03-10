#pragma once
#include "../../Graphics/Mesh.h"
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
void DLMesh::exportDLdebug(_string filename){
	FILE* report = fopen(filename.cstr, "w");

	fprintf(report, "bool compiled = "); (compiled ? fprintf(report, "true;\n") :  fprintf(report, "false;\n"));
	fprintf(report, "GLuint DL_id = "); fprintf(report, "%d;\n", DL_id); 

	CMesh::exportDebug(filename, report);
}
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
DLMesh::~DLMesh(){
	CMesh::~CMesh();
	if(compiled)
		glDeleteLists(DL_id, 1);
	UncastShadows();
}
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
DLMesh::DLMesh() : CMesh(), DL_id(0), compiled(false) {
}
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
void DLMesh::Build(){
	if(compiled)
		glDeleteLists(DL_id, 1);

	DL_id = glGenLists(1);

	bool use_tangent = !tangents.empty();
	tangent* const t_ptr = tangents.get_ptr();
	triangle*  _i ;  
	triangle*  _i_last ;  
	Vertex*const  _v = vertices.get_ptr();
	float*  vv = 0;
	float*  tt = 0;
	glNewList(DL_id, GL_COMPILE);
	__Shader::SwitchProgram(true);

	_BEGIN_SUBMESH_LOOP(submeshes);

	if(iter->ShaderProgram.node)
		(*iter->ShaderProgram).Use();
	else  
		__Shader::UseFixed();

	if(!(__Shader::active_mesh_sp->compo_set._flags & __MeshShader::NORMAL_MAPPING)){
		use_tangent = false;
	}

	if(iter->mtl_ID.node)
		(*iter->mtl_ID).MakeCurrent();
	else (*DEFAULT_MATERIAL).MakeCurrent();

	_i = indices.get_ptr() + iter->startindex ;
	_i_last = _i + iter->count ;

	glBegin(GL_TRIANGLES);
	while(_i != _i_last){
		for(int j = 0; j < 3; ++j) {
			vv = &((_v+_i->indices[j])->pos.x);
			glVertexAttrib3f(__MeshShader::ATTRIB_NORMAL, vv[5], vv[6], vv[7]);
			glVertexAttrib2f(__MeshShader::ATTRIB_TEXCOORD, vv[3], vv[4]);
			if(use_tangent) {
				tt = &((t_ptr+_i->indices[j])->x);
				glVertexAttrib4f(__MeshShader::ATTRIB_TANGENT, tt[0], tt[1], tt[2], tt[3]);
			}
			glVertexAttrib3f(__MeshShader::ATTRIB_VERTEX, vv[0], vv[1], vv[2]);
		}
		++_i;
	}

	glEnd();
	++iter;
	_END_LOOP

	quad*  q_i;  
	quad*  q_i_last;  


	_BEGIN_SUBMESH_LOOP(q_submeshes)

		if(iter->ShaderProgram.node)
			(*iter->ShaderProgram).Use();
		else  
			__Shader::UseFixed();

	if(!(__Shader::active_mesh_sp->compo_set._flags & __MeshShader::NORMAL_MAPPING)){
		use_tangent = false;
	}


	if(iter->mtl_ID.node)
		(*iter->mtl_ID).MakeCurrent();
	else (*DEFAULT_MATERIAL).MakeCurrent();

	q_i = q_indices.get_ptr() + iter->startindex ;
	q_i_last = q_i + iter->count ;


	glBegin(GL_QUADS);
	while(q_i != q_i_last){
		for(int j = 0; j < 4; ++j) {
			vv = &((_v+q_i->indices[j])->pos.x);
			glVertexAttrib3f(__MeshShader::ATTRIB_NORMAL, vv[5], vv[6], vv[7]);
			glVertexAttrib2f(__MeshShader::ATTRIB_TEXCOORD, vv[3], vv[4]);
			if(use_tangent) {
				tt = &((t_ptr+q_i->indices[j])->x);
				glVertexAttrib4f(__MeshShader::ATTRIB_TANGENT, tt[0], tt[1], tt[2], tt[3]);
			}
			glVertexAttrib3f(__MeshShader::ATTRIB_VERTEX, vv[0], vv[1], vv[2]);
		}
		++q_i;

	}
	glEnd();
	++iter;
	_END_LOOP

		glEndList();




	

	compiled = true;

#ifdef renderlog 
	fclose(flog);
#endif

	if(!(CMesh::options & CACHE_FOR_REBUILD)){
		vertices.clear();
		indices.clear();
		mtl_indices.clear();
		q_indices.clear();
		q_mtl_indices.clear();
	}

}
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
void DLMesh::setup_matrices(Matrix<GLfloat>& modelView){
	 static GLfloat mmat[16];
	 static GLfloat mpmat[16];
	 float mtex[16];
	 using namespace rawMatrix;
	 if(!__Shader::depth_buffer_mode) {
	mult(CScene::CurrentScene->camera->modelView.matrix.MATRIX, modelView.MATRIX, mmat);
	mult(CScene::CurrentScene->camera->projection.MATRIX, mmat, mpmat);
	
		_BEGIN_SUBMESH_LOOP(submeshes) 
			if(iter->ShaderProgram.node) {	
				(*iter->ShaderProgram).Use();
				(*iter->ShaderProgram).modelView.set(mmat);
				(*iter->ShaderProgram).modelViewProjection.set(mpmat);
			}
			++iter;
		_END_LOOP
		_BEGIN_SUBMESH_LOOP(q_submeshes) 
			if(iter->ShaderProgram.node) {	
				(*iter->ShaderProgram).Use();
				(*iter->ShaderProgram).modelView.set(mmat);
				(*iter->ShaderProgram).modelViewProjection.set(mpmat);
			}
			++iter;
			_END_LOOP

			static _list<CLight*>::Node* iter;
			static _list<CLight::uniform_set>::Node* s_iter;
			iter = CScene::CurrentScene->Attachments.Lights.first;
			while(iter){

				mult(iter->_v->tex_mat.MATRIX, modelView.MATRIX, mtex);

				s_iter = iter->_v->Updaters.first;
				while(s_iter) {
					(*s_iter->_v.program).Use();
					s_iter->_v.texmat.set(mtex);
					s_iter = s_iter->next;
				}
				iter = iter->next;
			}
		(*CScene::CurrentScene->depthbuf_shader).Use();

	 }
	 else {
		mult(CScene::CurrentScene->shadow_model.MATRIX, modelView.MATRIX, mmat);
		mult(CScene::CurrentScene->shadow_proj.MATRIX, mmat, mpmat);
		(*CScene::CurrentScene->depthbuf_shader).modelViewProjection.set(mpmat);

	 }
}

void DLMesh::Call(){
	static DrawCall** dc;
	if(!compiled || !visible) return;
	if(__Shader::depth_buffer_mode){
		if(!cast_shadows) return;

		dc = drawCalls.get_ptr();
		for(int i = 0; i < drawCalls.size(); ++i) {
			setup_matrices((*dc)->modelView);
			glCallList(vDL_id); 
			++dc;
		}
	}
	else { 	
		dc = drawCalls.get_ptr();
		for(int i = 0; i < drawCalls.size(); ++i) {
			setup_matrices((*dc)->modelView);
			glCallList(DL_id);
			++dc;
		}
		__Shader::SwitchProgram(true);
	}

}

/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
void DLMesh::CastShadows() {
	if(cast_shadows) return;
	cast_shadows = true;

	
	triangle*  _i ;  
	triangle*  _i_last ; 
	quad*  q_i ;  
	quad*  q_i_last ;  
	Vertex*const  _v = vertices.get_ptr();
	float*  vv = 0;
	float*  tt = 0;

		vDL_id = glGenLists(1);
		glNewList(vDL_id, GL_COMPILE);
		glBegin(GL_TRIANGLES);
		_BEGIN_SUBMESH_LOOP(submeshes)

		_i = indices.get_ptr() + iter->startindex ;
		_i_last = _i + iter->count ;

		while(_i != _i_last){
			for(int j = 0; j < 3; ++j) {
				vv = &((_v+_i->indices[j])->pos.x);
				glVertexAttrib3f(__MeshShader::ATTRIB_VERTEX, vv[0], vv[1], vv[2]);
			}
			++_i;
		}

		++iter;
		_END_LOOP
		glEnd();

		glBegin(GL_QUADS);
		_BEGIN_SUBMESH_LOOP(q_submeshes)

		q_i = q_indices.get_ptr() + iter->startindex ;
		q_i_last = q_i + iter->count ;

		while(q_i != q_i_last){
			for(int j = 0; j < 4; ++j) {
				vv = &((_v+q_i->indices[j])->pos.x);
				glVertexAttrib3f(__MeshShader::ATTRIB_VERTEX, vv[0], vv[1], vv[2]);
			}
			++q_i;
		}
		++iter;
		_END_LOOP
		glEnd();

		glEndList();



}void DLMesh::UncastShadows() {
	if(!cast_shadows) return;
	cast_shadows = false;

	glDeleteLists(vDL_id, 1);


}
