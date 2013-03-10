#pragma once
#include "../../Graphics/Mesh.h"

#define MAKE_SGROUP s_groups(act_sgroup)(act_sgroup)(act_sgroup)
#define _ADD_VERTEX(vert, in, s) s_VertBST.insert(vert, in, s); 

void CMesh::importOBJ(_string filename){

	struct __MTLkey{
		CMaterial addr;
		_string name;
	};
	static _list<__MTLkey> local_MTLs;
	GLuint* _indices = new GLuint[9000000];
	GLuint* _q_indices = new GLuint[9000000];
	Vertex* _vertices = new Vertex[4000000];
	Registry<__CMaterial>::HNode* _mtl_indices= new Registry<__CMaterial>::HNode[4000000];
	Registry<__CMaterial>::HNode* _q_mtl_indices= new Registry<__CMaterial>::HNode[4000000];
	vec3* _v= new vec3[4000000];
	vec2* _t= new vec2[4000000];
	vec3* _n= new vec3[4000000];
	memset(_vertices, 0, sizeof(Vertex)*4000000);
	Vertex* pVIter = _vertices;
	GLuint* indIter = _indices;
	GLuint* q_indIter = _q_indices;
	Registry<__CMaterial>::HNode* mtlIter = _mtl_indices;
	Registry<__CMaterial>::HNode* q_mtlIter = _q_mtl_indices;
	*indIter = 0;
	*q_indIter = 0;
	*mtlIter = 0;
	*q_mtlIter = 0;
	FILE* fobj = fopen(filename.c_str(), "r");
	if(!fobj) return;

	int _iv=0, _it=0, _in=0;

	char line_buf[2000];
	char f;
	char s;
	int ix[9];
	fpos_t position;
	Registry<__CMaterial>::HNode actual_Material = DEFAULT_MATERIAL.node;
	int act_sgroup = -1;
	GLuint first_v = 0;
	GLuint last_v = 0;
	GLuint _index = 0;
	while(true){ 
		f = fgetc(fobj);
		if(f == EOF) break;
		else if(f == 'v'){
			s = fgetc(fobj);
			if(s == 't') { fscanf(fobj, " %f %f", &_t[_it].u,&_t[_it].v); ++_it; }
			else if(!(CMesh::options & REBUILD_NORMALS) && s == 'n'){ fscanf(fobj, " %f %f %f", &_n[_in].x, &_n[_in].y,&_n[_in].z); ++_in; }
			else if (s == ' ') { fscanf(fobj, " %f %f %f", &_v[_iv].x, &_v[_iv].y, &_v[_iv].z); ++_iv; }
			else if (s != '\n') fgets(line_buf, sizeof(line_buf), fobj);
		}
		else if (f != '\n') fgets(line_buf, sizeof(line_buf), fobj);

	}
	rewind(fobj);
	bool quad_pass = false;
	while(true){
		f = fgetc(fobj);
		if(f == EOF) break;
		else if(f == 'f'){
			fgetpos(fobj, &position);
			if(fscanf(fobj, "%d/%d/%d %d/%d/%d %d/%d/%d", &ix[0], &ix[1], &ix[2], &ix[3], &ix[4], &ix[5], &ix[6], &ix[7], &ix[8]  ) == 9){
				quad_pass = true;
				pVIter->pos = _v[ix[0]-1]; pVIter->tex = _t[ix[1]-1]; if(!(CMesh::options & REBUILD_NORMALS))  pVIter->norm = _n[ix[2]-1];
				*indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup); 
				if(*indIter++ == last_v){ ++pVIter; ++last_v; }	
				first_v = *(indIter-1);

				pVIter->pos = _v[ix[3]-1]; pVIter->tex = _t[ix[4]-1]; if(!(CMesh::options & REBUILD_NORMALS))  pVIter->norm = _n[ix[5]-1];
				*indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup); 
				if(*indIter++ == last_v){ ++pVIter; ++last_v; }	

				pVIter->pos = _v[ix[6]-1]; pVIter->tex = _t[ix[7]-1]; if(!(CMesh::options & REBUILD_NORMALS))  pVIter->norm = _n[ix[8]-1];
				*indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup); 
				if(*indIter++ == last_v){ ++pVIter; ++last_v; }	

				*mtlIter++ = actual_Material;

				while(fscanf(fobj, " %d/%d/%d", &ix[0], &ix[1], &ix[2]) == 3){
					pVIter->pos = _v[ix[0]-1]; pVIter->tex = _t[ix[1]-1];  if(!(CMesh::options & REBUILD_NORMALS))  pVIter->norm = _n[ix[2]-1];
					if(quad_pass){
						*q_indIter++ = *(indIter-3);
						*q_indIter++ = *(indIter-2);
						*q_indIter++ = *(indIter-1);
						*q_indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup);
						if(*q_indIter == last_v) {  ++pVIter; ++last_v; }	
						++q_indIter;
						indIter -= 3;
						mtlIter -= 1;
						*q_mtlIter++ = actual_Material;
					}
					else{
						*indIter++ = first_v;  
						*indIter = *(q_indIter-1); ++indIter;
						*indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup);
						if(*indIter == last_v) {  ++pVIter; ++last_v; }	
						++indIter;
						*mtlIter++ = actual_Material;
					}
					quad_pass = !quad_pass;

				}

				continue;
			}
			fsetpos(fobj, &position);
			if(fscanf(fobj, "%d//%d %d//%d %d//%d", &ix[0], &ix[1], &ix[2], &ix[3], &ix[4], &ix[5]) == 6){
				quad_pass = true;
				pVIter->pos = _v[ix[0]-1]; if(!(CMesh::options & REBUILD_NORMALS))  pVIter->norm = _n[ix[1]-1];
				*indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup); 
				if(*indIter++ == last_v){ ++pVIter; ++last_v; }	 first_v = *(indIter-1);

				pVIter->pos = _v[ix[2]-1]; if(!(CMesh::options & REBUILD_NORMALS))  pVIter->norm = _n[ix[3]-1];
				*indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup); 
				if(*indIter++ == last_v){ ++pVIter; ++last_v; }	

				pVIter->pos = _v[ix[4]-1]; if(!(CMesh::options & REBUILD_NORMALS))  pVIter->norm = _n[ix[5]-1];
				*indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup); 
				if(*indIter++ == last_v){ ++pVIter; ++last_v; }	

				*mtlIter++ = actual_Material;
				while(fscanf(fobj, " %d//%d", &ix[0], &ix[1]) == 2){
					pVIter->pos = _v[ix[0]-1]; if(!(CMesh::options & REBUILD_NORMALS))  pVIter->norm = _n[ix[1]-1];
					if(quad_pass){
						*q_indIter++ = *(indIter-3);
						*q_indIter++ = *(indIter-2);
						*q_indIter++ = *(indIter-1);
						*q_indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup);
						if(*q_indIter == last_v) {  ++pVIter; ++last_v; }	
						++q_indIter;
						indIter -= 3;
						mtlIter -= 1;
						*q_mtlIter++ = actual_Material;
					}
					else{
						*indIter++ = first_v;  
						*indIter = *(q_indIter-1); ++indIter;
						*indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup);
						if(*indIter == last_v) {  ++pVIter; ++last_v; }	
						++indIter;
						*mtlIter++ = actual_Material;
					}
					quad_pass = !quad_pass;
				}
				continue;
			}
			fsetpos(fobj, &position);
			if(fscanf(fobj, "%d/%d %d/%d %d/%d", &ix[0], &ix[1], &ix[2], &ix[3], &ix[4], &ix[5]) == 6){
				quad_pass = true;
				pVIter->pos = _v[ix[0]-1]; pVIter->tex = _t[ix[1]-1]; 
				*indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup); 
				if(*indIter++ == last_v){ ++pVIter; ++last_v; }	 first_v = *(indIter-1);

				pVIter->pos = _v[ix[2]-1]; pVIter->tex = _t[ix[3]-1];
				*indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup); 
				if(*indIter++ == last_v){ ++pVIter; ++last_v; }	

				pVIter->pos = _v[ix[4]-1]; pVIter->tex = _t[ix[5]-1]; 
				*indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup); 
				if(*indIter++ == last_v){ ++pVIter; ++last_v; }	

				*mtlIter++ = actual_Material;
				while(fscanf(fobj, " %d/%d", &ix[0], &ix[1]) == 2){
					pVIter->pos = _v[ix[0]-1]; pVIter->tex = _t[ix[1]-1];  
					if(quad_pass){
						*q_indIter++ = *(indIter-3);
						*q_indIter++ = *(indIter-2);
						*q_indIter++ = *(indIter-1);
						*q_indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup);
						if(*q_indIter == last_v) {  ++pVIter; ++last_v; }	
						++q_indIter;
						indIter -= 3;
						mtlIter -= 1;
						*q_mtlIter++ = actual_Material;
					}
					else{
						*indIter++ = first_v;  
						*indIter = *(q_indIter-1); ++indIter;
						*indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup);
						if(*indIter == last_v) {  ++pVIter; ++last_v; }	
						++indIter;
						*mtlIter++ = actual_Material;
					}
					quad_pass = !quad_pass;
				}
				continue;
			}
			fsetpos(fobj, &position);
			if(fscanf(fobj, "%d %d %d", &ix[0], &ix[1], &ix[2]) == 3){
				quad_pass = true;
				pVIter->pos = _v[ix[0]-1]; 
				*indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup); 
				if(*indIter++ == last_v){ ++pVIter; ++last_v; }	
				first_v = *(indIter-1);

				pVIter->pos = _v[ix[1]-1]; 
				*indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup); 
			
				if(*indIter++ == last_v){ ++pVIter; ++last_v; }	

				pVIter->pos = _v[ix[2]-1]; 
				*indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup); 
				if(*indIter++ == last_v){ ++pVIter; ++last_v; }	
				
				*mtlIter++ = actual_Material;
				while(fscanf(fobj, " %d", &ix[0]) == 1){
					pVIter->pos = _v[ix[0]-1];
					if(quad_pass){
						*q_indIter++ = *(indIter-3);
						*q_indIter++ = *(indIter-2); 
						*q_indIter++ = *(indIter-1);
						*q_indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup);
						if(*q_indIter == last_v) {  ++pVIter; ++last_v; }	
						++q_indIter;
						indIter -= 3;
						mtlIter -= 1;
						*q_mtlIter++ = actual_Material;
					}
					else{
						*indIter++ = first_v;  
						*indIter = *(q_indIter-1); ++indIter;
						*indIter = s_VertBST.insert(*pVIter, last_v, act_sgroup);
						if(*indIter == last_v) {  ++pVIter; ++last_v; }	
						++indIter;
						*mtlIter++ = actual_Material;
					}
					quad_pass = !quad_pass;
				}
				continue;
			}
		}
		else if((CMesh::options & REBUILD_NORMALS) && f == 's'){
			if(fscanf(fobj, " %d", &act_sgroup) != 1) act_sgroup = -1;
		}
		else if(f == 'm'){
			char tag_buf[200];
			fscanf(fobj, "%s %s", tag_buf, line_buf);
			if(!strcmp(tag_buf, "tllib")){

				FILE* mtllib = fopen(line_buf, "r");
				if(mtllib){

					bool it_is_first = true;
					__CMaterial act_mtl;
					act_mtl = *DEFAULT_MATERIAL;
					__MTLkey act_key;
					char mtlname[256];

					while(f != EOF){ 

						f = fgetc(mtllib);
						if(f == 'n' || f == EOF){
							if(it_is_first && f == 'n'){ 
								fscanf(mtllib, "%*s %s", mtlname);
								act_key.name = _string(mtlname);
								it_is_first = false;
							}
							else if(!it_is_first) {

								act_key.addr = CMaterial(act_mtl);
								local_MTLs.push_back(act_key);
								act_mtl = *DEFAULT_MATERIAL;

								if(f == 'n'){
									fscanf(mtllib, "%*s %s", mtlname);
									act_key.name = _string(mtlname);
								}
							}

						}
						else { 
							float dd = 1.0;
							switch(f){
							case 'N': if(fgetc(mtllib) == 's'){ fscanf(mtllib, "%f", &act_mtl.shininess);
									  }
									  break;
									  
							case 'd': 
								fscanf(mtllib, "%f", &dd);
								act_mtl.ambient[3] = dd;
								act_mtl.diffuse[3] = dd;
								act_mtl.specular[3] = dd;
								break;
							case 'i': 
								fscanf(mtllib, "%*s %*d");
								break;
							case 'K': 

								switch(fgetc(mtllib)){
								case 'a': fscanf(mtllib, "%f %f %f",
											  &act_mtl.ambient[0],
											  &act_mtl.ambient[1],
											  &act_mtl.ambient[2]);
									break;

								case 'd': fscanf(mtllib, "%f %f %f",
											  &act_mtl.diffuse[0],
											  &act_mtl.diffuse[1],
											  &act_mtl.diffuse[2]);
									break;

								case 's': fscanf(mtllib, "%f %f %f",
											  &act_mtl.specular[0],
											  &act_mtl.specular[1],
											  &act_mtl.specular[2]);
									break;
								default: 
									fgets(line_buf, sizeof(line_buf), mtllib); break;
								}
								break;

							case 'm':

								if(fscanf(mtllib, "%s %s", line_buf, mtlname) == 2){
									if(!strcmp(line_buf, "ap_Kd") || !strcmp(line_buf, "ap_kd")){
										act_mtl.diffuseMap = CTexture(mtlname);

									}
									else if(!strcmp(line_buf, "ap_bump") || !strcmp(line_buf, "ap_Bump")){
										
										act_mtl.normalMap = CTexture(mtlname);

									}
									else if(!strcmp(line_buf, "ap_height") || !strcmp(line_buf, "ap_Height")){
										
										act_mtl.heightMap = CTexture(mtlname);

									}
									else if(!strcmp(line_buf, "ap_spec") || !strcmp(line_buf, "ap_Spec")){
										
										act_mtl.specularMap = CTexture(mtlname);

									}
									else fgets(line_buf, sizeof(line_buf), mtllib);
								}
								else fgets(line_buf, sizeof(line_buf), mtllib);

								break;
							default: break;
							};
						}  
					}
				}
				fclose(mtllib);
			}
		}
		else if(f == 'u' ) { fscanf(fobj, "%*s %s", line_buf); 
		_list<__MTLkey>::Node* _iter_begin = local_MTLs.first;
		bool found = false;
		while(_iter_begin){
			if(!strcmp(_iter_begin->_v.name.cstr, line_buf)){ actual_Material = _iter_begin->_v.addr.node;
			actual_Material->_v.constant = true;
			found = true;
			break;
			}
			_iter_begin = _iter_begin->next;
		}
		if(!found) actual_Material = DEFAULT_MATERIAL.node;
		}
		else if (f != '\n') fgets(line_buf, sizeof(line_buf), fobj);
	}
		 vertices.reserve(	pVIter		- _vertices		);
		  indices.reserve(  (indIter	- _indices)	  /3);
	  mtl_indices.reserve(	mtlIter		- _mtl_indices  );
		q_indices.reserve(  (q_indIter	- _q_indices) /4);
	q_mtl_indices.reserve(	q_mtlIter	- _q_mtl_indices);

	memcpy(		 vertices.get_ptr(),     _vertices,		 vertices.capacity()*sizeof(Vertex));
	memcpy(       indices.get_ptr(),      _indices,		  indices.capacity()*sizeof(triangle));
	memcpy(	  mtl_indices.get_ptr(),  _mtl_indices,	  mtl_indices.capacity()*sizeof(CMaterial));
	memcpy(	    q_indices.get_ptr(),    _q_indices,		q_indices.capacity()*sizeof(quad));
	memcpy(	q_mtl_indices.get_ptr(),_q_mtl_indices, q_mtl_indices.capacity()*sizeof(CMaterial));

		vertices.count =	   vertices._size;
		 indices.count =		indices._size;
	 mtl_indices.count =	mtl_indices._size;
	   q_indices.count =	  q_indices._size;
   q_mtl_indices.count =  q_mtl_indices._size;

	fclose(fobj);

	local_MTLs.clear();

	  delete [] _indices ;
	  delete [] _q_indices ;
	  delete []  _vertices ;
	  delete []  _mtl_indices;
	  delete []  _q_mtl_indices;
	  delete []  _v;
	  delete []  _t;
	  delete []  _n;
	
}

/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/