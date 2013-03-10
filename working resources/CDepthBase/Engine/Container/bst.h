#pragma once
#include <cstdio>
#include <new>

template <typename T>
struct BinarySearchTree{
	struct Node{
		T value;
		Node* parent;
		Node* dirs[2]; 
		inline Node(T&, Node*, Node*);
	};

	Node* root;

	inline BinarySearchTree();
	inline ~BinarySearchTree();

	inline void clear(Node*&);
	inline void move_up(Node*, bool);
	inline void remove(Node*);
	
	inline Node* insert(T&);
};

#include "bst.hpp"