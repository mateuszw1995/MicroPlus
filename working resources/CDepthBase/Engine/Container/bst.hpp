#pragma once
template<class T>
BinarySearchTree<T>::Node::Node(T& val, Node* l, Node* r) : value(val), parent(NULL) {
	dirs[0] = r;
	dirs[1] = l;
}

template<class T>
BinarySearchTree<T>::BinarySearchTree() : root(NULL)
{}
/**********************************************************************************************************************************************/
template<class T>
typename BinarySearchTree<T>::Node* BinarySearchTree<T>::insert(T& val) {
	static bool cmp = false;
	if(!root) {
		root = new Node(val, 0, 0);	return root;
	}
	else{
		Node* iter = root;
		while(iter){
			cmp = (val < iter->value);
			if(!cmp) if(val == iter->value) return iter;
			if(!iter->dirs[cmp]) { iter->dirs[cmp] = new Node(val, 0, 0); iter->dirs[cmp]->parent = iter; return iter->dirs[cmp]; }
			else iter = iter->dirs[cmp];
		}
	}
	return NULL;
}

/**********************************************************************************************************************************************/
template<class T>
void BinarySearchTree<T>::clear(Node*& p){
	if(p){
		clear(p->dirs[0]);
		clear(p->dirs[1]);
		delete p;
	}
}

/**********************************************************************************************************************************************/
template<class T>
BinarySearchTree<T>::~BinarySearchTree(){
	clear(root);
	root = NULL;
}


/**********************************************************************************************************************************************/
template<class T>
void BinarySearchTree<T>::move_up(Node* node, bool dir){

}
/**********************************************************************************************************************************************/
template<class T>
void BinarySearchTree<T>::remove(Node* node){

	bool nstate[2] = {node->dirs[0],node->dirs[1]}; 

	if(!nstate[0] && !nstate[1]){
		node->parent->dirs[node != node->parent->dirs[0]] = NULL; 
		delete node;
		return;
	}
	else if(nstate[0] && nstate[1]){ 
		Node* iter = node;
		iter = iter->dirs[1]; 
		while(iter->dirs[0]) iter = iter->dirs[0];
		iter->parent->dirs[0] = iter->dirs[1];
	}

}
/**********************************************************************************************************************************************/
/**********************************************************************************************************************************************/
/**********************************************************************************************************************************************/