#pragma once
#include <cstdio>
#include <string.h>
namespace db {
	namespace misc {
		namespace container {
			template <typename T>
			class circular_buffer {
				int begin, end, maxlen;
				T* q;
			public:
				circular_buffer(int max_size);
				circular_buffer(const circular_buffer&);
				circular_buffer& operator=(const circular_buffer&);
				
				void push_back(const T&), push_back(T*, int n), pop_front(), pop_back();
				bool empty() const;
				int size() const, capacity() const;
				T& front() const, &back() const;

				void clear();
				T& operator[](int);

				~circular_buffer();
			};
#include "circular_buffer.hpp"
		}
	}
}