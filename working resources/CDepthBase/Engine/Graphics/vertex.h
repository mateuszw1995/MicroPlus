#pragma once
#include "../Math/Transformation.h"
#include "../Container/bst.h"
#include "../Defines.h"
struct vec4{
	GLfloat x,y,z,w;
};
struct vec3{
	GLfloat x,y,z;
	vec3(GLfloat _x = 0.0, GLfloat _y = 0.0, GLfloat _z = 0.0);
	operator Vec3D<float>();
};
struct vec2{
	GLfloat u,v;
};

struct Vertex{ // vertex class used in meshes
	vec3 pos;
	vec2 tex;
	vec3 norm;
};
struct bstVertex{ // comparision utility
	vec3 pos;
	vec2 tex;
	vec3 norm;
	GLuint index;
	GLint sgroup;
};

template<>
struct BinarySearchTree<Vertex>{
	bool ref_sgroups;
	struct Node{
		Vertex value;
		GLint sgroup;
		GLuint index;
		Node* dirs[2]; 
		inline Node(Vertex&);
	};

	Node* root;
	inline BinarySearchTree();

	inline GLuint insert(Vertex&, GLuint, GLint s_group1 = -1);

	inline void clear(Node*&);
	inline void clear();

	inline ~BinarySearchTree();
};



/***********************************************************************************************************************************************************************/

BinarySearchTree<Vertex>::Node::Node(Vertex& val) {
	value = val;
	dirs[0] = NULL;
	dirs[1] = NULL;
}


/**********************************************************************************************************************************************/
BinarySearchTree<Vertex>::BinarySearchTree() 
	: root(NULL), ref_sgroups(false) {
}

/**********************************************************************************************************************************************/
GLuint BinarySearchTree<Vertex>::insert(Vertex& val, GLuint index, GLint s_group1) {
	if(s_group1 == -1) return index;
	static int cmp = false;
	const int size_of = (ref_sgroups ? sizeof(Vertex) - sizeof(vec3) : sizeof(Vertex) );
	if(!root) {
		root = new Node(val);	
		root->index = index;
		if(ref_sgroups) root->sgroup = s_group1;
		return index;
	}
	else{
		Node* iter = root;
		while(iter){
			cmp = memcmp(&val, &iter->value, size_of);
			if(ref_sgroups && !cmp) 
				cmp = s_group1 > iter->sgroup; 
			if(!cmp)
				return iter->index; 
			if(cmp < 0) cmp = 0;

			if(!iter->dirs[cmp]) { 
				iter->dirs[cmp] = new Node(val); 
				iter->dirs[cmp]->index = index;
				if(ref_sgroups) iter->dirs[cmp]->sgroup = s_group1;
				return index; 
			}
			iter = iter->dirs[cmp];

		}
	}
	return index;
}

/**********************************************************************************************************************************************/
void BinarySearchTree<Vertex>::clear(Node*& p){
	if(p){
		clear(p->dirs[0]);
		clear(p->dirs[1]);
		delete p;
		p=0;
	}
}
/**********************************************************************************************************************************************/
void BinarySearchTree<Vertex>::clear(){
	clear(root);
}

/**********************************************************************************************************************************************/
BinarySearchTree<Vertex>::~BinarySearchTree(){
	clear(root);
	root = NULL;
}