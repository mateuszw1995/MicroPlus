#pragma once
#include "list.h"
template <class T>
class Registry {
public:
	struct entry {
		T _v;
		int use;
		bool constant;
		inline entry();
		inline entry(const T&);
		inline bool univalent();
	};
	
	static _list<entry> content;
	typedef typename _list<entry>::Node* HNode;
	
	static inline HNode MakeResource(const T&);

	struct Link {
		HNode node;
		inline Link();
		inline Link(const T&);
		inline Link(HNode&);
		inline Link(const Link&);
		inline Link& operator=(const Link&);
		inline T& operator*() const;
		inline ~Link();
		inline void validate();
		
		inline void mark_constant();
		inline void unmark_constant();

	};
};

#include "registry.hpp"