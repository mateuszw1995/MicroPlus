#pragma once
#include "../../Graphics/Mesh.h"

/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/

unsigned long long                 CMesh::options				 = 0;
Registry		<__CMaterial>	   CMesh::g_Materials			 = Registry   <__CMaterial> (); 
BinarySearchTree<Vertex>           CMesh::s_VertBST				 = BinarySearchTree<Vertex>	  ();

CMesh::submesh::submesh() : count(0), startindex(0), mtl_ID(CMaterial()) {}

/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
void CMesh::submesh::Attach_Shader(MeshShader sp){
	ShaderProgram = sp;
}
/***********************************************************************************************************************************************************************/
void CMesh::submesh::Attach_MTL(CMaterial& newmtl){
	mtl_ID = newmtl;
}
/***********************************************************************************************************************************************************************/
int CMesh::submesh::operator>(const submesh& b){
	return ShaderProgram.node > b.ShaderProgram.node;
}

/***********************************************************************************************************************************************************************/
CMesh::CMesh() : 
_2D_projecting(false),
	WindowCoordinatesCache(NULL),
	vertices(_vectorm<Vertex>()),
	tangents(NULL),
	indices(_vectorm<triangle>()),
	mtl_indices(_vectorm<Registry<__CMaterial>::HNode>()),
	q_indices(_vectorm<quad>()),
	q_mtl_indices(_vectorm<Registry<__CMaterial>::HNode>()),
	submeshes(_vectorm<submesh>()), 
	q_submeshes(_vectorm<submesh>()),
	show_normals(0.0),
	visible(true){
}
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/

CMesh::DrawCall::DrawCall() : shadow_caster(true) {}

_vectorm<CMesh::DrawCall*>& CMesh::DrawCalls() {
	return drawCalls;
}

/***********************************************************************************************************************************************************************/
void CMesh::Show_Normals(float scale){
	show_normals = scale;
}
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
void CMesh::submeshes_clear(bool tri, bool quad, bool deletion){
	
	if(tri){
		
	if(deletion){
	_BEGIN_SUBMESH_LOOP(submeshes)
		iter++->mtl_ID.mark_constant();
	_END_LOOP
	}
		submeshes.clear();

	}
	if(quad){
	
	if(deletion){
	_BEGIN_SUBMESH_LOOP(q_submeshes)
		iter++->mtl_ID.mark_constant();
	_END_LOOP
	}
	
		q_submeshes.clear();
	}
}
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
CMesh::~CMesh(){
		submeshes_clear(true, true, true);
		vertices.clear();
		tangents.clear();
		indices.clear();
		mtl_indices.clear();
		q_indices.clear();
		q_mtl_indices.clear();

		if(WindowCoordinatesCache)
			delete WindowCoordinatesCache;
}

/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
void CMesh::Import(ModelType type, _string filename){

	CMesh::~CMesh();
	s_VertBST.ref_sgroups = (CMesh::options & REBUILD_NORMALS);
	switch(type){

	case		OBJ:  importOBJ(filename);		break;
	case		MD5:							break;
	case		DBPO: importPrototype(filename);break;
	case		_3DS: import3DS(filename);		break;

	default: return;
	}
	
	if(CMesh::options & REBUILD_NORMALS)  
		build_normals();

	s_VertBST.clear();
}

/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/

void CMesh::BuildSubmeshes(){
	build_submeshes();
}
void CMesh::BuildShaders(const __MeshShader::ComponentSet& c){
	unsigned __int64 shader_flag = 0;
	__CMaterial* mtlc = 0;
	static __MeshShader::ComponentSet cSet;
	bool lights = c.Lights.size();
	bool tangents = false;

	for(int i = 0; i < 2; ++i) {
	_BEGIN_SUBMESH_LOOP((i == 0 ? submeshes : q_submeshes))
		if(iter->mtl_ID.node){
			cSet = c;
			mtlc = &(*iter->mtl_ID);
			if(mtlc->diffuseMap.node)
				if((*mtlc->diffuseMap).imported)
				shader_flag |= __MeshShader::DIFFUSE_MAPPING;
			if(mtlc->normalMap.node && lights)
				if((*mtlc->normalMap).imported)
				shader_flag |= __MeshShader::NORMAL_MAPPING;
			if(mtlc->heightMap.node) {
				if(!(*mtlc->heightMap).imported)
					cSet.parallax = 0;
			}
			else cSet.parallax = 0;
			if(mtlc->specularMap.node && lights)
				if((*mtlc->specularMap).imported)
				shader_flag |= __MeshShader::SPECULAR_MAPPING;
			
			shader_flag |= __MeshShader::SHADOW_MAPPING;
			cSet._flags &= shader_flag;
			if(!tangents && cSet._flags & __MeshShader::NORMAL_MAPPING) {
				build_tangents();
				tangents = true;
			}
			iter->Attach_Shader(MeshShader(__MeshShader(cSet)));

			shader_flag = 0;
		}
		else iter->Attach_Shader(MeshShader(__MeshShader::ComponentSet()));

		++iter;
	_END_LOOP
	}
}
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
void CMesh::Attach_Shader(MeshShader& s){

	_BEGIN_SUBMESH_LOOP(submeshes)
		iter++->ShaderProgram = s;
	_END_LOOP

	_BEGIN_SUBMESH_LOOP(q_submeshes)
		iter++->ShaderProgram = s;
	_END_LOOP

}
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
void CMesh::Enable(Flags option){
	options |= option;
}
/***********************************************************************************************************************************************************************/
void CMesh::Disable(Flags option){
	options &= ~option;
}
/***********************************************************************************************************************************************************************/
bool CMesh::IsEnabled(Flags option){
	return options & option;
}

/***********************************************************************************************************************************************************************/
void CMesh::Cube(float a){

}
/***********************************************************************************************************************************************************************/
void CMesh::Cuboid(float a, float b, float c){

}
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
void CMesh::Scale(float x_scalar,float y_scalar,float z_scalar, float u_scalar, float v_scalar, float nx_scalar, float ny_scalar, float nz_scalar)
{
	_BEGIN_VERTEX_LOOP(vertices)
	iter->pos.x *= x_scalar;
	iter->pos.y *= y_scalar;
	iter->pos.z *= z_scalar;
	iter->tex.u *= u_scalar;
	iter->tex.v *= v_scalar;
	iter->norm.x *= nx_scalar;
	iter->norm.y *= ny_scalar;
	iter++->norm.z *= nz_scalar;
	_END_LOOP
}
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/

void CMesh::Gen_UV_Sphere()
{
	_BEGIN_VERTEX_LOOP(vertices)
	float x = iter->pos.x;
	float y = iter->pos.y;
	float z = iter->pos.z;
	iter->tex.u = x/(sqrt(x*x + y*y + z*z)); 
	iter++->tex.v = y/(sqrt(x*x + y*y + z*z));
	_END_LOOP
}
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
void CMesh::_2DProjection(bool flag){
	_2D_projecting = flag;
	if(!WindowCoordinatesCache && flag) WindowCoordinatesCache = new _vectorm<GLfloat>;
	else if(WindowCoordinatesCache && !flag) delete WindowCoordinatesCache;
}

/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/

void CMesh::importPrototype(_string filename){

}

/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
void CMesh::build_normals(){

	unsigned Size = vertices.size();
	Vertex* vptr = vertices.get_ptr();
	Vertex* endvptr = vertices.get_ptr()+Size;

	GLfloat* vert1 = 0;
	GLfloat* vert2 = 0;
	GLfloat* vert3 = 0;
	GLfloat v1[3],v2[3], out[3];
	Vertex* fiter = 0;

	_BEGIN_VERTEX_LOOP(vertices)
		memset(&iter->norm, 0, 3*sizeof(float));
	++iter;
	_END_LOOP
		triangle* iter = indices.get_ptr(); 
	triangle* end_iter = indices.get_ptr()+indices.size(); 
	
#define accumulate  fiter->norm.x += out[0]; fiter->norm.y += out[1]; fiter->norm.z += out[2]; 


	while(iter != end_iter){ 
		vert1 = &(vptr + iter->indices[0])->pos.x;
		vert2 = &(vptr + iter->indices[1])->pos.x;
		vert3 = &(vptr + iter->indices[2])->pos.x;
		v1[0] = vert1[0] - vert2[0];					
		v1[1] = vert1[1] - vert2[1];					
		v1[2] = vert1[2] - vert2[2];		

		v2[0] = vert2[0] - vert3[0];					
		v2[1] = vert2[1] - vert3[1];					
		v2[2] = vert2[2] - vert3[2];			
		out[0] = v1[1]*v2[2] - v1[2]*v2[1];	
		out[1] = v1[2]*v2[0] - v1[0]*v2[2];	
		out[2] = v1[0]*v2[1] - v1[1]*v2[0];

		
		fiter = (vptr + iter->indices[0]); accumulate;
		fiter = (vptr + iter->indices[1]); accumulate;           
		fiter = (vptr + iter->indices[2]); accumulate;

		++iter;
	}

	quad* q_iter = q_indices.get_ptr(); 
	quad* q_end_iter = q_indices.get_ptr()+q_indices.size(); 
	while(q_iter != q_end_iter){ 
		vert1 = &(vptr + q_iter->indices[0])->pos.x;
		vert2 = &(vptr + q_iter->indices[1])->pos.x;
		vert3 = &(vptr + q_iter->indices[2])->pos.x;
		v1[0] = vert1[0] - vert2[0];					
		v1[1] = vert1[1] - vert2[1];					
		v1[2] = vert1[2] - vert2[2];		

		v2[0] = vert2[0] - vert3[0];					
		v2[1] = vert2[1] - vert3[1];					
		v2[2] = vert2[2] - vert3[2];			
		out[0] = v1[1]*v2[2] - v1[2]*v2[1];	
		out[1] = v1[2]*v2[0] - v1[0]*v2[2];	
		out[2] = v1[0]*v2[1] - v1[1]*v2[0];

		
		fiter = (vptr + q_iter->indices[0]); accumulate;
		fiter = (vptr + q_iter->indices[1]); accumulate;           
		fiter = (vptr + q_iter->indices[2]); accumulate;        
		fiter = (vptr + q_iter->indices[3]); accumulate;

		++q_iter;
	}
#undef accumulate

	while(vptr != endvptr)
		rawVector::normalize3D(&vptr++->norm.x);
}
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
void CMesh::build_tangents(){
	const GLuint VSIZE = vertices.size();

	tangents.resize(VSIZE);
	
    float edge1[3] = {0.0f, 0.0f, 0.0f};
    float edge2[3] = {0.0f, 0.0f, 0.0f};
    float texEdge1[2] = {0.0f, 0.0f};
    float texEdge2[2] = {0.0f, 0.0f};
    float tangent[4] = {0.0f, 0.0f, 0.0f};
    float bitangent[3] = {0.0f, 0.0f, 0.0f};
    float det = 0.0f;
    float nDotT = 0.0f;
    float bDotB = 0.0f;
    float length = 0.0f;
	
    Vertex *pVertex0 = 0;
    Vertex *pVertex1 = 0;
    Vertex *pVertex2 = 0;

	vec3* bitangents = new vec3[VSIZE]; 
	memset(bitangents, 0, sizeof(vec3)*VSIZE );
	memset(tangents.get_ptr(), 0, sizeof(tangent)*tangents.capacity() );
	
	triangle* iter = indices.get_ptr(); 
	triangle* end_iter = indices.get_ptr()+indices.size(); 
	
	Vertex* v_ptr = vertices.get_ptr();
	Vertex* endvptr = v_ptr+VSIZE;
	vec3* bt_ptr = bitangents;
	vec3* bt_iter = bitangents;
	CMesh::tangent* t_ptr = tangents.get_ptr();
	CMesh::tangent* t_iter = t_ptr;
	
#define accumulate t_iter->x += tangent[0]; t_iter->y += tangent[1]; t_iter->z += tangent[2]; t_iter->w += tangent[3]; bt_iter->x += bitangent[0]; bt_iter->y += bitangent[1]; bt_iter->z += bitangent[2]; 

	while(iter != end_iter){ 
		
        pVertex0 = v_ptr + iter->indices[0];
        pVertex1 = v_ptr + iter->indices[1];
        pVertex2 = v_ptr + iter->indices[2];

        edge1[0] = pVertex1->pos.x - pVertex0->pos.x;
        edge1[1] = pVertex1->pos.y - pVertex0->pos.y;
        edge1[2] = pVertex1->pos.z - pVertex0->pos.z;

        edge2[0] = pVertex2->pos.x - pVertex0->pos.x;
        edge2[1] = pVertex2->pos.y - pVertex0->pos.y;
        edge2[2] = pVertex2->pos.z - pVertex0->pos.z;

        texEdge1[0] = pVertex1->tex.u - pVertex0->tex.u;
        texEdge1[1] = pVertex1->tex.v - pVertex0->tex.v;

        texEdge2[0] = pVertex2->tex.u - pVertex0->tex.u;
        texEdge2[1] = pVertex2->tex.v - pVertex0->tex.v;

        det = texEdge1[0] * texEdge2[1] - texEdge2[0] * texEdge1[1];

		 if (fabs(det) < 1e-6f){
            tangent[0] = 1.0f;
            tangent[1] = 0.0f;
            tangent[2] = 0.0f;

            bitangent[0] = 0.0f;
            bitangent[1] = 1.0f;
            bitangent[2] = 0.0f;
        }
        else{
            det = 1.0f / det;

            tangent[0] = (texEdge2[1] * edge1[0] - texEdge1[1] * edge2[0]) * det;
            tangent[1] = (texEdge2[1] * edge1[1] - texEdge1[1] * edge2[1]) * det;
            tangent[2] = (texEdge2[1] * edge1[2] - texEdge1[1] * edge2[2]) * det;

            bitangent[0] = (-texEdge2[0] * edge1[0] + texEdge1[0] * edge2[0]) * det;
            bitangent[1] = (-texEdge2[0] * edge1[1] + texEdge1[0] * edge2[1]) * det;
            bitangent[2] = (-texEdge2[0] * edge1[2] + texEdge1[0] * edge2[2]) * det;
		}
		
		 t_iter = t_ptr + iter->indices[0];
		 bt_iter = bitangents + iter->indices[0];
		 accumulate;
		 t_iter = t_ptr + iter->indices[1];
		 bt_iter = bitangents + iter->indices[1];
		 accumulate;
		 t_iter = t_ptr + iter->indices[2];
		 bt_iter = bitangents + iter->indices[2];
		 accumulate;

		++iter;
	}

	quad* q_iter = q_indices.get_ptr(); 
	quad* q_end_iter = q_indices.get_ptr()+q_indices.size(); 
	
	while(q_iter != q_end_iter){ 
        pVertex0 = v_ptr + q_iter->indices[0];
        pVertex1 = v_ptr + q_iter->indices[1];
        pVertex2 = v_ptr + q_iter->indices[2];

        edge1[0] = pVertex1->pos.x - pVertex0->pos.x;
        edge1[1] = pVertex1->pos.y - pVertex0->pos.y;
        edge1[2] = pVertex1->pos.z - pVertex0->pos.z;

        edge2[0] = pVertex2->pos.x - pVertex0->pos.x;
        edge2[1] = pVertex2->pos.y - pVertex0->pos.y;
        edge2[2] = pVertex2->pos.z - pVertex0->pos.z;

        texEdge1[0] = pVertex1->tex.u - pVertex0->tex.u;
        texEdge1[1] = pVertex1->tex.v - pVertex0->tex.v;

        texEdge2[0] = pVertex2->tex.u - pVertex0->tex.u;
        texEdge2[1] = pVertex2->tex.v - pVertex0->tex.v;

        det = texEdge1[0] * texEdge2[1] - texEdge2[0] * texEdge1[1];
		
		 if (fabs(det) < 1e-6f){
            tangent[0] = 1.0f;
            tangent[1] = 0.0f;
            tangent[2] = 0.0f;

            bitangent[0] = 0.0f;
            bitangent[1] = 1.0f;
            bitangent[2] = 0.0f;
        }
        else{
            det = 1.0f / det;

            tangent[0] = (texEdge2[1] * edge1[0] - texEdge1[1] * edge2[0]) * det;
            tangent[1] = (texEdge2[1] * edge1[1] - texEdge1[1] * edge2[1]) * det;
            tangent[2] = (texEdge2[1] * edge1[2] - texEdge1[1] * edge2[2]) * det;

            bitangent[0] = (-texEdge2[0] * edge1[0] + texEdge1[0] * edge2[0]) * det;
            bitangent[1] = (-texEdge2[0] * edge1[1] + texEdge1[0] * edge2[1]) * det;
            bitangent[2] = (-texEdge2[0] * edge1[2] + texEdge1[0] * edge2[2]) * det;
        }
		
		 t_iter = t_ptr + q_iter->indices[0];
		 bt_iter = bitangents + q_iter->indices[0];
		 accumulate;
		 t_iter = t_ptr + q_iter->indices[1];
		 bt_iter = bitangents + q_iter->indices[1];
		 accumulate;
		 t_iter = t_ptr + q_iter->indices[2];
		 bt_iter = bitangents + q_iter->indices[2];
		 accumulate;
		 t_iter = t_ptr + q_iter->indices[3];
		 bt_iter = bitangents + q_iter->indices[3];
		 accumulate;


		++q_iter;
	}

	
#undef accumulate

	while(v_ptr != endvptr)
    {

		nDotT = v_ptr->norm.x * t_ptr->x +
                v_ptr->norm.y * t_ptr->y +
                v_ptr->norm.z * t_ptr->z;

        t_ptr->x -= v_ptr->norm.x * nDotT;
        t_ptr->y -= v_ptr->norm.y * nDotT;
        t_ptr->z -= v_ptr->norm.z * nDotT;


        length = 1.0f / sqrtf(t_ptr->x * t_ptr->x +
                              t_ptr->y * t_ptr->y +
                              t_ptr->z * t_ptr->z);

        t_ptr->x *= length;
        t_ptr->y *= length;
        t_ptr->z *= length;
		
        bitangent[0] = (v_ptr->norm.y * t_ptr->z) - 
                       (v_ptr->norm.z * t_ptr->y);
        bitangent[1] = (v_ptr->norm.z * t_ptr->x) -
                       (v_ptr->norm.x * t_ptr->z);
        bitangent[2] = (v_ptr->norm.x * t_ptr->y) - 
                       (v_ptr->norm.y * t_ptr->x);

        bDotB = bitangent[0] * bt_ptr->x + 
                bitangent[1] * bt_ptr->y + 
                bitangent[2] * bt_ptr->z;

        t_ptr->w = (bDotB < 0.0f) ? 1.0 : -1.0;

        bt_ptr->x = bitangent[0];
        bt_ptr->y = bitangent[1];
        bt_ptr->z = bitangent[2];
		++v_ptr; 
		++t_ptr; 
		++bt_ptr;
	}
	


	tangents.count = VSIZE;
	delete [] bitangents;
}

/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
void CMesh::build_submeshes(){
	using namespace math;
	using namespace math::sort;
	if(!indices.empty()){
	submeshes_clear(true, false, false);
	
	dual_heap(
		mtl_indices.get_ptr(), 
		indices.get_ptr(),
		indices.size());


	Registry<__CMaterial>::HNode act_inmtl = mtl_indices[0];
	
	submeshes.push_back(submesh());
	submesh* act_sub = &submeshes.back();
	
	act_sub->startindex = 0;
	act_sub->count = 0;
	act_sub->mtl_ID = CMaterial(act_inmtl);
	
	Registry<__CMaterial>::HNode* iter = mtl_indices.get_ptr(); 
	Registry<__CMaterial>::HNode* end_iter = (mtl_indices.get_ptr()+mtl_indices.size()); 
	while(iter != end_iter){ 
		if(*iter == act_inmtl){
			++(act_sub->count);
		    ++iter;
		}
		else {
			int sind  = act_sub->startindex, cnt = act_sub->count; 
			submeshes.push_back(submesh());
			act_sub = &submeshes.back();
			act_sub->startindex = sind+cnt;
			act_sub->count = 0;
			act_sub->mtl_ID = CMaterial(*iter);
			act_sub->mtl_ID.unmark_constant();
			act_inmtl = *iter;
		}
	}

	heap(submeshes.get_ptr(), submeshes.size());
	}

	
	/* QUADS **********************************************************/
	/* QUADS **********************************************************/

	if(!q_indices.empty()){
		
	submeshes_clear(false, true, false);

	dual_heap(
		q_mtl_indices.get_ptr(), 
		q_indices.get_ptr(),
		q_indices.size());

	Registry<__CMaterial>::HNode q_act_inmtl = q_mtl_indices[0];
	
	q_submeshes.push_back(submesh());
	submesh* q_act_sub = &q_submeshes.back();
	
	q_act_sub->startindex = 0;
	q_act_sub->count = 0;
	q_act_sub->mtl_ID = CMaterial(q_act_inmtl);
	
	Registry<__CMaterial>::HNode* q_iter = q_mtl_indices.get_ptr(); 
	Registry<__CMaterial>::HNode* q_end_iter = (q_mtl_indices.get_ptr()+q_mtl_indices.size()); 
	while(q_iter != q_end_iter){ 
		if(*q_iter == q_act_inmtl){
			++(q_act_sub->count);
		    ++q_iter;
		}
		else {
			int sind  = q_act_sub->startindex, cnt = q_act_sub->count; 
			q_submeshes.push_back(submesh());
			q_act_sub = &q_submeshes.back();
			q_act_sub->startindex = sind+cnt;
			q_act_sub->count = 0;
			q_act_sub->mtl_ID = CMaterial(*q_iter);
			q_act_sub->mtl_ID.unmark_constant();
			q_act_inmtl = *q_iter;
		}
	}
	
	heap(q_submeshes.get_ptr(), q_submeshes.size());
	}
}

/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
