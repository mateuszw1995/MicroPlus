#pragma once

#include "../../Graphics/Mesh.h"
#include <ctime> 

void CMesh::exportDebug(_string filename, FILE* f){

	FILE* report = 0;
	if(!f) report = fopen(filename.cstr, "w");
	else report = f;

	if(!report) return;
	
time_t czas;
struct tm *czas_strukt;
czas = time(0);
czas_strukt = localtime(&czas);


fprintf(report, 
	  "DepthBase Debug Log. For programmers use.\n------------------------------------------------------------------------------------------------------------------\n---|DATE: %d.%d.%d|\n---|TIME: %d:%s%d:%s%d|\n------------------------------------------------------------------------------------------------------------------\n\n\n\n", czas_strukt->tm_mday, czas_strukt->tm_mon, 1900+czas_strukt->tm_year, czas_strukt->tm_hour, (czas_strukt->tm_min < 10 ? "0" : "" ),  czas_strukt->tm_min,(czas_strukt->tm_sec < 10 ? "0" : "" ), czas_strukt->tm_sec);
	  
	fprintf(report, "bool _2D_projecting = "); (_2D_projecting ? fprintf(report, "true;\n") :  fprintf(report, "false;\n"));
	fprintf(report, "bool visible = "); (visible ? fprintf(report, "true;\n") :  fprintf(report, "false;\n"));

	fprintf(report,"\n------------------------------------------------------------------------------------------------------------------\n_vector<GLfloat>* WindowCoordinatesCache = \n");
	if(WindowCoordinatesCache) (*WindowCoordinatesCache).fprint_floatvec(report, 2, sizeof(GLfloat));
	fprintf(report,"\n------------------------------------------------------------------------------------------------------------------\n_vector<Vertex> vertices = \n");
	vertices.fprint_floatvec(report, 3, sizeof(Vertex), 5);
	fprintf(report,"\n------------------------------------------------------------------------------------------------------------------\n_vector<Vertex> texcoords = \n");
	vertices.fprint_floatvec(report, 2, sizeof(Vertex), 6, 3);
	fprintf(report,"\n------------------------------------------------------------------------------------------------------------------\n_vector<Vertex> normals = \n");
	vertices.fprint_floatvec(report, 3, sizeof(Vertex), 5, 5);
	fprintf(report,"\n------------------------------------------------------------------------------------------------------------------\n_vector<tangent>* tangents = \n");
	tangents.fprint_floatvec(report, 4, sizeof(tangent), 0);
	fprintf(report,"\n------------------------------------------------------------------------------------------------------------------\n_vector<GLuint> indices = \n");
	indices.fprint_uintvec(report, 3, sizeof(triangle));
	
	fprintf(report,"\n------------------------------------------------------------------------------------------------------------------\n_vector<GLuint> mtl_indices;\n");
	
	mtl_indices.fprint_uintvec(report, 1, sizeof(GLuint));

	fprintf(report,"\n------------------------------------------------------------------------------------------------------------------\n_vector<submesh> submeshes = \n");
	submeshes.fprint_info(report);
	for(int i = 0; i < submeshes.size(); ++i){
		fprintf(report, "start: %d\ncount: %d\nMTL addr: %p\nMTL found id: %d\nShader addr: %p\nShader found id: %d\n\n", submeshes[i].startindex,
																					submeshes[i].count, 
																					submeshes[i].mtl_ID,
																					g_Materials.content.ifind(submeshes[i].mtl_ID.node),
																					&submeshes[i].ShaderProgram.node->_v._v,
																					__MeshShader::g_MeshShaders.content.ifind(submeshes[i].ShaderProgram.node)
																					); 
	}


	fprintf(report,"}\n------------------------------------------------------------------------------------------------------------------\n_vector<submesh> q_submeshes = \n");
	q_submeshes.fprint_info(report);
	for(int i = 0; i < q_submeshes.size(); ++i){
		fprintf(report, "start: %d\ncount: %d\nMTL addr: %p\nMTL found id: %d\nShader addr: %p\nShader found id: %d\n\n", q_submeshes[i].startindex, 
																					q_submeshes[i].count, 
																					q_submeshes[i].mtl_ID, 
																					g_Materials.content.ifind(q_submeshes[i].mtl_ID.node),
																					&q_submeshes[i].ShaderProgram.node->_v._v, 
																					__MeshShader::g_MeshShaders.content.ifind(q_submeshes[i].ShaderProgram.node)
																					); 
	}



	fprintf(report,"}\n------------------------------------------------------------------------------------------------------------------\n_vector<GLuint> q_indices = \n");
	q_indices.fprint_uintvec(report, 4, sizeof(quad));
	
	fprintf(report,"\n------------------------------------------------------------------------------------------------------------------\n_vector<GLuint> q_mtl_indices;\n");
	q_mtl_indices.fprint_uintvec(report, 1, sizeof(GLuint));


	fprintf(report, "\n------------------------------------------------------------------------------------------------------------------\n_list<CTexture> CTexture::g_texCache;\n{");
	_list<Registry<Texture>::entry>::Node* iter_begin = Texture::g_texCache.content.first;
	while(iter_begin){

		fprintf(report, "texture: %d\nfilename: %s\nimported: %d\n\n", iter_begin->_v._v.texture, iter_begin->_v._v.filenamez.cstr, iter_begin->_v._v.imported);
		iter_begin = iter_begin->next;

	}
	fprintf(report, "}\n\n");

	fclose(report);

}