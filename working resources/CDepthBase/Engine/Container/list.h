#pragma once
#include <cstdio>
#include <new>

typedef int _int_;
template <typename T>
struct _list {

	struct Node {
		T _v;
		Node* next, *prev;
		Node(Node* = 0, Node* = 0, const T& = T());
	} *first, *last;
	unsigned _size;

	_list();
	_list(int, const T& = T());
	_list(const _list&);
	
	inline _list& operator=(const _list& b);
	
	inline Node *insert(unsigned, const T&),
			    *insert_n(unsigned, const T&),
			    *push_front(const T&),
			    *push_back(const T&),
			    *push_front_n(const T&),
			    *push_back_n(const T&),
			    *operator+=(const T&),
			    *operator|=(const T&),
			     
			    *get_node(_int_) const,
			    *find(const T&) const;
	int ifind(Node*) const;

	inline _int_ size() const,
	             get_index(const Node* const) const;

	__forceinline T& operator[](_int_) const;
	bool operator==(const _list&);
	
	inline void resize(unsigned, const T& = T()),
	            pop_front(),
	            pop_back(),
				on_top(Node*),
				erase(Node*),
				erase(Node*, Node*),
				clear();
	~_list();
};

#include "list.hpp"