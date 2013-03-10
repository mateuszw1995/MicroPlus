#pragma once
#include <cstdio> 
#include <new>
typedef int _int_;

template <class T>
struct _vec {
	T* _p;
	static T *itb, *ite, *itr;

	_int_ count, _size;
	
	__forceinline void _realloc(_int_);

	inline _int_ find(const T*), 
				 size() const,
				 capacity() const;
	
	inline T* get_ptr () const;
	inline T& at(_int_) const,
			& operator[](_int_) const,
	        & front() const,
	        & back() const;

	inline bool empty() const;
	inline void fprint_info(FILE*) const,
	            fprint_floatvec(FILE*, short, int, int = 1, int = 0) const,
	            fprint_uintvec (FILE*, short, int) const;

	
	static _vec<T> temp;
	template <class B>
	_vec<T> operator-(const B&) const;
	template <class B>
	_vec<T>& operator-=(const B&);
	template <class B>
	_vec<T> operator+(const B&) const;
	template <class B>
	_vec<T>& operator+=(const B&);
	template <class B>
	_vec<T> operator*(const B&) const;
	template <class B>
	_vec<T>& operator*=(const B&);

};

template <class T>
struct _vector : public _vec<T> {
	_vector(_int_ elems = 0);
	_vector(const _vector&);
	
	inline void operator+=(const T&),
	            operator|=(const T&),
	
				resize(_int_, const T&),
				resize(_int_),
	            reserve(_int_),
	            
	            push_back(const T&),
	            push_back_n(const T&),
	            insert(int, const T&),
	            insert(int, int, const T&),
	            insert_n(int, const T&),
	
				erase(const T&),
	            erase(const T*),
				erase(int),
				erase(int, int),
				trim(),
				clear();

	inline T* find(const T&);
	
	inline _vector& operator=(const _vector&),
				  & operator()(const T& newelem);
	
	inline ~_vector();
	
	inline bool fread(const char*), fsave(const char*);
};


template <class T>
struct _vectorm : public _vec<T> {
	_vectorm(_int_ = 1);
	_vectorm(const _vectorm&);
	
	inline void operator+=(const T&),
	            operator|=(const T&),
	
				resize(_int_, const T&),
				resize(_int_),
	            reserve(_int_),
	            
	            push_back(const T&),
	            push_back_n(const T&),
	            insert(int, const T&),
	            insert(int, int, const T&),
	            insert_n(int, const T&),
	
				erase(const T&),
	            erase(const T*),
				erase(int),
				erase(int, int),
				trim(),
				clear();

	inline T* find(const T&);
	
	inline _vectorm& operator=(const _vectorm&),
				   & operator()(const T& newelem);

	inline ~_vectorm();

	inline bool fread(const char*), fsave(const char*);

};

#include "vector.hpp"