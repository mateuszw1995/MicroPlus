#pragma once
#include "../Math/Transformation.h"
#include "../Defines.h"
#include "../Graphics/Shader/MeshShader.h"
#include "../Graphics/Postprocess.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/Color.h"
#include "../Math/Camera.h"

#include "../Graphics/Shader/MeshShader.h"

class CLight;
class DLMesh;
class VBOMesh;
class Printer;

class CScene {
	friend class DLMesh;
	friend class VBOMesh;
	friend class CPostProcessor;
	friend class CLight;

	static GLuint fs_mesh;
	static bool dl_fs_mesh;
	
	Matrix<> shadow_proj, shadow_model;
	CPostProcessor* postprocessor;
	__MeshShader::ComponentSet Attachments;

	void CallLists();
	void CallVBOs();
	void switchUniforms();

public:
	static MeshShader depthbuf_shader;
	static CScene* CurrentScene;
	static FPPCamera default_camera;
	static void SetCurrentScene(CScene*);
	static void SetBackgroundColor(RGBA_1);

	Camera* camera;
	_list<DLMesh*> DLDrawings;
	_list<VBOMesh*> INDrawings;
	_list<Printer*> Texts;

	void Add(DLMesh* newobj); 
	void Add(VBOMesh* newobj); 
	void Add(CLight* newlight); 
	void Add(Printer* newtext); 

	template<class T>
	CScene& operator+=(T* object) {
		Add(object);
		return *this;
	}
	
	void Set_PostProcessor(CPostProcessor* ptr); 
	void MapShadows();
	void Draw();

	CScene();
};
