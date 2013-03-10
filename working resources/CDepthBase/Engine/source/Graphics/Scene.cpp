#pragma once
#include "../../Graphics/Scene.h"
#include "../../Math/maths.h"
#include "../../Graphics/GUI/Text.h"
extern GLuint FPS;
extern GLuint MAXFPS;
extern bool capturefps;
extern GLuint FPSlive;
extern GLuint fpstimer;

CScene*   CScene::CurrentScene = 0;
GLuint	  CScene::fs_mesh = 0;
bool	  CScene::dl_fs_mesh = false;
FPPCamera CScene::default_camera;

MeshShader CScene::depthbuf_shader;
/***********************************************************************************************************************************************************************/
void CScene::SetCurrentScene(CScene* scene) {
	CurrentScene = scene;
}
void CScene::SetBackgroundColor(RGBA_1 col) {
	glClearColor(col.r, col.g, col.b, col.alpha);
}
/***********************************************************************************************************************************************************************/
void CScene::Add(DLMesh* newobj)  { DLDrawings.push_back(newobj);			}
void CScene::Add(VBOMesh* newobj) { INDrawings.push_back(newobj);			}
void CScene::Add(CLight* newlight){ Attachments.Lights.push_back(newlight); }
void CScene::Add(Printer* newtext){ Texts.push_back(newtext);				}
/******************************************************************************************************************************/
CScene::CScene() : postprocessor(0), camera(&default_camera), DLDrawings(_list<DLMesh*>()), INDrawings(_list<VBOMesh*>()) {
}
/******************************************************************************************************************************/
void CScene::Set_PostProcessor(CPostProcessor* ptr){
	postprocessor = ptr;
	if(!ptr) 
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	if(!dl_fs_mesh){
		fs_mesh = glGenLists(1);

		glNewList(fs_mesh, GL_COMPILE);
		glBegin(GL_QUADS);
		glVertexAttrib2s(__MeshShader::ATTRIB_VERTEX, -1, -1);
		glVertexAttrib2s(__MeshShader::ATTRIB_VERTEX, -1, 1);
		glVertexAttrib2s(__MeshShader::ATTRIB_VERTEX, 1, 1);
		glVertexAttrib2s(__MeshShader::ATTRIB_VERTEX, 1, -1);
		glEnd();
		glEndList();
		dl_fs_mesh = true;
	}
}

/******************************************************************************************************************************/
/******************************************************************************************************************************/
void CScene::CallLists(){
	static _list<DLMesh*>::Node* dldriterator;
	dldriterator = DLDrawings.first;

	while(dldriterator){
		dldriterator->_v->Call(); 
		dldriterator = dldriterator->next;
	}  
}
/******************************************************************************************************************************/
/******************************************************************************************************************************/
void CScene::CallVBOs(){
	static _list<VBOMesh*>::Node* indriterator;

	indriterator = INDrawings.first;
	while(indriterator){
		indriterator->_v->Call(); 
		indriterator = indriterator->next; 
	}
}

/******************************************************************************************************************************/
/******************************************************************************************************************************/
void CScene::switchUniforms(){

	static _list<Registry<__MeshShader>::entry>::Node* uniiterator;
	uniiterator = Registry<__MeshShader>::content.first;

	while(uniiterator){
		static __MeshShader::Material* m;
		m = uniiterator->_v._v.compo_set.material;
		if(m) {
			m->u_ambient.switch_next = true;
			m->u_diffuse.switch_next = true;
			m->u_specular.switch_next = true;
			m->u_shininess.switch_next = true;
		}
		uniiterator = uniiterator->next;
	};
}

/******************************************************************************************************************************/
/******************************************************************************************************************************/

void CScene::Draw(){
	CurrentScene = this;
	
	if(!camera) camera = &default_camera;

	static bool shadows_called;
	static _list<CLight*>::Node* iter;

	shadows_called = false;
	iter = Attachments.Lights.first;

	/***********************************************************************************************************************/
	/**********************************          SHADOWS       *************************************************************/
	/***********************************************************************************************************************/
	while(iter){
		if(iter->_v->shadows)
			shadows_called = true;
		iter = iter->next;
	}

	if(shadows_called) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); 
		glDisableVertexAttribArray(__MeshShader::ATTRIB_NORMAL);
		glDisableVertexAttribArray(__MeshShader::ATTRIB_TEXCOORD);
		glDisableVertexAttribArray(__MeshShader::ATTRIB_TANGENT); 
		__Shader::depth_buffer_mode = true;
		(*depthbuf_shader).Use();
	}
	iter = Attachments.Lights.first;
	while(iter){
		if(iter->_v->shadows) { 
			glViewport(0,0,iter->_v->sMapW,iter->_v->sMapH);

			shadow_proj = iter->_v->shadow_proj;
			shadow_model.identity();
			shadow_model.lookat(
				iter->_v->position,
				iter->_v->lookat,
				Vec3D<>(0.f,-1.f,0.f));

			iter->_v->set_texture_matrix();
			(*depthbuf_shader).Use();

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,iter->_v->fboId);
			glClear(GL_DEPTH_BUFFER_BIT);
			CallLists();
			CallVBOs();
		}	
		iter = iter->next;
	}

	if(shadows_called) {
		glDisable(GL_CULL_FACE);
		__Shader::depth_buffer_mode = false;
		glCullFace(GL_BACK);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); 	
		glViewport(0,0,DB::Window::getWidth(),DB::Window::getHeight());

		glEnableVertexAttribArray(__MeshShader::ATTRIB_NORMAL);
		glEnableVertexAttribArray(__MeshShader::ATTRIB_TEXCOORD);
	}

	if(postprocessor)
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, postprocessor->fboId);
	else if(shadows_called)
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	CallLists();
	switchUniforms();
	CallVBOs();
	
	static _list<Printer*>::Node* textit; // Text drawings iterator
	textit = Texts.first;
	while(textit) {
		textit->_v->Print();
		textit = textit->next;
	}

	/***********************************************************************************************************************/
	/*****************************                POSTPROCESSING        ****************************************************/
	/***********************************************************************************************************************/

	if(postprocessor){
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, postprocessor->textureId);
	}
}
