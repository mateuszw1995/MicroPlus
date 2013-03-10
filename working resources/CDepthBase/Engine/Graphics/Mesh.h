#pragma once
#include "../Defines.h"
#include "../Container/string.h"
#include "../Container/registry.h"
#include "../Container/list.h"
#include "../Container/vector.h"
#include "../Container/bst.h"
#include "../Graphics/Shader/MeshShader.h"
#include "../Graphics/Texture.h"
#include "../Graphics/Light.h"
#include "../Math/maths.h"
#include "../Math/Transformation.h"
#include "../Math/Physics.h"
#include "../Graphics/vertex.h"
#include "../Graphics/Color.h"
#include <cmath>

#define _BEGIN_VERTEX_LOOP(vec)  \
	{Vertex* iter = vec.get_ptr(); \
	Vertex* end_iter = vec.get_ptr()+vec.size(); \
	while(iter != end_iter){ 

#define _BEGIN_SUBMESH_LOOP(vec)  \
	{submesh* iter = vec.get_ptr(); \
	submesh* end_iter = vec.get_ptr()+vec.size(); \
	while(iter != end_iter){ 

#define _BEGIN_INDEX_LOOP(vec)  \
	{GLuint* iter = (GLuint*)vec.get_ptr(); \
	GLuint* end_iter = vec.get_ptr()+vec.size(); \
	while(iter != end_iter){ 

#define _END_LOOP }} 

struct __CMaterial
{
	__CMaterial::__CMaterial(RGBA_1 _ambient = RGBA_1(0.2f, 0.2f, 0.2f, 1.0f), 
							RGBA_1 _diffuse = RGBA_1(0.8f, 0.8f, 0.8f, 1.0f), 
							RGBA_1 _specular = RGBA_1(0.0f, 0.0f, 0.0f, 1.0f), 
							GLfloat _shininess = 0.0f, 
							CTexture* _diffuseMap = 0, 
							CTexture* _normalMap = 0, 
							CTexture* _heightMap = 0, 
							CTexture* _specularMap = 0);

	bool operator==(const __CMaterial& b);

	float ambient[4],
		  diffuse[4],
		  specular[4],
		  shininess;

	CTexture diffuseMap,
			 normalMap,
			 heightMap,
			 specularMap;

	int use_counter;
	void MakeCurrent();
};
typedef Registry<__CMaterial>::Link CMaterial;
extern CMaterial DEFAULT_MATERIAL;

/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
class CMesh{
	static BinarySearchTree<Vertex> s_VertBST;
public:
	
	struct DrawCall {
		Matrix<GLfloat> modelView;
		bool shadow_caster;
		DrawCall();
	};
	_vectorm<DrawCall*> drawCalls;
	_vectorm<DrawCall*>& DrawCalls();

	enum Flags	   {
		REBUILD_NORMALS		  = 1,
		CACHE_FOR_REBUILD     = 2
	};
	enum ModelType {
		OBJ,
		MD5,
		DBPO,
		_3DS
	};
	static unsigned long long			options;
	static void							Enable (Flags option);
	static void							Disable (Flags option);
	static bool							IsEnabled (Flags option);
	static Registry<__CMaterial>		g_Materials;
	
	struct submesh {
		submesh();
		unsigned count;
		unsigned startindex;
		CMaterial mtl_ID;
		int operator>(const submesh&);
		MeshShader ShaderProgram;
		void Attach_Shader(MeshShader sp);
		void Attach_MTL(CMaterial& newmtl);
	};
	struct tangent {
		GLfloat x,y,z,w;
	};
	struct triangle {
		GLuint indices[3];
	};
	struct quad {
		GLuint indices[4];
	};

	bool _2D_projecting;
	bool cast_shadows;
	float show_normals;
	bool visible;


	_vectorm<GLfloat>*	WindowCoordinatesCache;
	_vectorm<tangent>	tangents;
	_vectorm<triangle>	indices;
	_vectorm<Registry<__CMaterial>::HNode> mtl_indices;
	_vectorm<Vertex>	vertices;
	_vectorm<submesh>	submeshes;
	_vectorm<quad>	    q_indices;
	_vectorm<Registry<__CMaterial>::HNode> q_mtl_indices;
	_vectorm<submesh>	q_submeshes;
	
	void AddVertex();
	void AddTriangle();
	void AddQuad();

	void Gen_UV_Sphere();
	void Show_Normals(float scale);
	void _2DProjection(bool);
	void Scale(float, float, float, float u_scalar = 1.0, float v_scalar = 1.0, float nx_scalar = 1.0, float ny_scalar = 1.0, float nz_scalar = 1.0);
	
	CMesh();
	~CMesh();

	void Cube(float);
	void Cuboid(float, float, float);

	void Import(ModelType type, _string filename);
	void Attach_Shader(MeshShader& s);
	
	void BuildSubmeshes();
	void BuildShaders(const __MeshShader::ComponentSet&);
	
	void exportDebug(_string filename, FILE* f = 0);

protected:
	void submeshes_clear(bool, bool, bool);

	void importPrototype(_string filename);		// model
	void importOBJ(_string filename);			// model
	void import3DS(_string filename);			// model
	
	void build_submeshes();
	void build_normals();
	void build_tangents();

	void exportPrototype(_string filename);

};

/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
class VBOMesh : public CMesh {
	char buffered;
	unsigned VBO_buffer; 
	unsigned VBO_indices;
	unsigned tan_offset, qind_offset;
	void clear_buf();
public:
	VBOMesh();
	~VBOMesh();
	void Build();
	void Call();
	void CastShadows();
	void UncastShadows();
};

/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
class DLMesh : public CMesh {
	void exportDLdebug(_string filename);
	bool compiled;
	GLuint DL_id;
	GLuint vDL_id; // used for shadow mapping
public:
	void setup_matrices(Matrix<GLfloat>&);
	~DLMesh();
	DLMesh();
	void Build();
	void Call();
	void CastShadows();
	void UncastShadows();
};

