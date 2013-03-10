#pragma once

template<class T>
_vec<T> _vec<T>::temp = new _vec<T>;


template<class T>
T* _vec<T>::itb = NULL, *_vec<T>::ite = NULL, *_vec<T>::itr = NULL;

#define copy(b, e, r) (itb=b;ite=e;itr=r; while (itb!=ite) *itr++ = *itb++;)
#define poll(b, e) itb=b;ite=e;while (itb!=ite) 


template <class T>
template <class B>
_vec<T> _vec<T>::operator-(const B& b) const {
	temp.resize(count);
	for(int i = 0; i < count; ++i) temp._p[i] = _p[i] - b;
	return temp;
}
template <class T>
template <class B>
_vec<T>& _vec<T>::operator-=(const B&) {
	for(int i = 0; i < count; ++i) _p[i] -= b;
	return *this;
}
template <class T>
template <class B>
_vec<T> _vec<T>::operator+(const B& b) const {
	temp.resize(count);
	for(int i = 0; i < count; ++i) temp._p[i] = _p[i] + b;
	return temp;
}
template <class T>
template <class B>
_vec<T>& _vec<T>::operator+=(const B& b) {
	for(int i = 0; i < count; ++i) _p[i] += b;
	return *this;
}
template <class T>
template <class B>
_vec<T> _vec<T>::operator*(const B& b) const {
	temp.resize(count);
	for(int i = 0; i < count; ++i) temp._p[i] = _p[i] * b;
	return temp;
}
template <class T>
template <class B>
_vec<T>& _vec<T>::operator*=(const B& b) {
	for(int i = 0; i < count; ++i) _p[i] *= b;
	return *this;
}


template <class T>/*****************************************************************************************************************************/
inline bool _vector<T>::fread(const char* fname) {
	FILE* f = fopen(fname, "r");
	if(!f) return false;

	clear();
	
	_int_ s, c;
	::fread(&c, sizeof(_int_), 1, f);
	::fread(&s, sizeof(_int_), 1, f);
	resize(c);
	reserve(s);
	::fread(_p, sizeof(T), c, f);
	count = c;

	fclose(f);
	return true;
}
template <class T>/*****************************************************************************************************************************/
inline bool _vectorm<T>::fsave(const char* fname) {
	FILE* f = fopen(fname, "w");

	if(!f) return false;
	fwrite(&count, sizeof(_int_), 1, f);
	fwrite(&_size, sizeof(_int_), 1, f);
	fwrite(_p, sizeof(T), count, f);

	fclose(f);
	return true;
}

template <class T>/*****************************************************************************************************************************/
inline bool _vectorm<T>::fread(const char* fname) {
	FILE* f = fopen(fname, "r");
	if(!f) return false;

	_int_ s, c;
	::fread(&c, sizeof(_int_), 1, f);
	::fread(&s, sizeof(_int_), 1, f);
	resize(s);
	::fread(_p, sizeof(T), c, f);
	count = c;

	fclose(f);
	return true;
}
template <class T>/*****************************************************************************************************************************/
inline bool _vector<T>::fsave(const char* fname) {
	FILE* f = fopen(fname, "w");

	if(!f) return false;
	fwrite(&count, sizeof(_int_), 1, f);
	fwrite(&_size, sizeof(_int_), 1, f);
	fwrite(_p, sizeof(T), count, f);

	fclose(f);
	return true;
}



template <class T>/*****************************************************************************************************************************/
__forceinline void _vec<T>::_realloc(_int_ memory) {
		_p = (T*)realloc(_p, (_size=memory)*sizeof(T));
}
template <class T>/*****************************************************************************************************************************/
_vector<T>::_vector(_int_ elems) {
	count = _size = 0;
	_p = 0;
	if(elems > 0)
		_p = (T*)malloc(sizeof(T)*(_size = elems));
}
template <class T>/*****************************************************************************************************************************/
_vector<T>::_vector(const _vector<T>& a) {
	count = a.count;
	_size = 0;
	_p = 0;
	if(a._size > 0) {
		_p = (T*)malloc(sizeof(T)*(_size = a._size));

		int i = 0;
		poll(_p, _p+count) new (itb++) T(a._p[i++]);
	}
}

template <class T>/*****************************************************************************************************************************/
_vector<T>& _vector<T>::operator=(const _vector<T>& a){
	if(this == &a) return *this;
	if(a.count > _size) _realloc(a.count);
	
	copy(a._p, a._p+a.count, _p);
	count = count > a.count ? count : a.count;

	return *this;
}

template <class T>/*****************************************************************************************************************************/
void _vector<T>::resize(_int_ memory, const T& t) {
	if(memory > _size) {
		_realloc(memory);
		poll(_p+count, _p+_size) new (itb++) T(t);
	}
	else if (memory == _size) return;
	else {
		poll(_p+memory, _p+_size)(*itb++).~T();
		_realloc(memory);
	}
	count = _size;
}

template <class T>/*****************************************************************************************************************************/
void _vector<T>::resize(_int_ memory) {
	if(memory > _size) {
		_realloc(memory);
		poll(_p+count, _p+_size) new (itb++) T;
	}
	else if (memory == _size) return;
	else {
		poll(_p+memory, _p+_size)(*itb++).~T();
		_realloc(memory);
	}
	count = _size;
}


template <class T>/*****************************************************************************************************************************/
void _vector<T>::reserve(_int_ memory) {
	if(memory > _size)
		_realloc(memory);
}
template <class T>/*****************************************************************************************************************************/
inline void _vector<T>::push_back(const T& _t) {
	if(!memcmp(&count, &_size, sizeof(_int_)))
		_realloc(count ? count*2 : 1);

	new (_p+count++) T(_t);
}

template <class T>/*****************************************************************************************************************************/
inline void _vector<T>::push_back_n(const T& _t) {
	find(_t) ?  0 : push_back(_t);
}

template <class T>/*****************************************************************************************************************************/
inline void _vector<T>::insert(int pos, const T& v) {
	if(pos >= count) return push_back(v);
	
	if(!memcmp(&count, &_size, sizeof(_int_)))
		_realloc(count ? count*2 : 1);

	memmove(_p+pos+1, _p+pos, sizeof(T)*(count++-pos));
	new (_p + pos) T(v);
}
template <class T>/*****************************************************************************************************************************/
inline void _vector<T>::insert(int pos, int n, const T& v) {
	if(pos >= count) pos = count;
	if(_size < count+n)
		_realloc(count ? (count+n)*2 : n);

	memmove(_p+pos+n, _p+pos, sizeof(T)*(count-pos));
	count += n;
	poll(_p+pos, _p+pos+n) new (itb++) T(v);
}
template <class T>/*****************************************************************************************************************************/
inline void _vector<T>::insert_n(int pos, const T& v) {
	find(v) ? 0 : insert(pos, v);
}

template <class T>/*****************************************************************************************************************************/
inline T* _vector<T>::find(const T& _t) {
	poll(_p, _p+count)
		if(*itb++ == _t) 
			return itb-1;
	return 0;
}

template <class T>/*****************************************************************************************************************************/
void _vector<T>::erase(const T* _t){
	erase(_vec<T>::find(_t));
}

template <class T>/*****************************************************************************************************************************/
void _vector<T>::erase(int i){
	if(count <= 0 || i < 0)
		return;

	_p[i].~T();
	memcpy(_p+i, _p+i+1, sizeof(T)*(count-i));
	--count;
}
template <class T>/*****************************************************************************************************************************/
void _vector<T>::erase(int l, int r){
	if(count <= r-l || l < 0 || r < 0)
		return;

	for(int i = l; i <= r; ++i)
		_p[i].~T();

	memcpy(_p+l, _p+r+1, sizeof(T)*(count-r));
	count -= r-l+1;
}
template <class T>/*****************************************************************************************************************************/
void _vector<T>::trim(){
	resize(count);
}

template <class T>/*****************************************************************************************************************************/
void _vector<T>::operator+=(const T& _t){
	push_back(_t);
}

template <class T>/*****************************************************************************************************************************/
void _vector<T>::operator|=(const T& _t){
	push_back_n(_t);
}

template <class T>/*****************************************************************************************************************************/
_vector<T>& _vector<T>::operator()(const T& newelem){
	push_back(newelem);
	return *this;
}
template <class T>/*****************************************************************************************************************************/
void _vector<T>::clear(void){
	poll(_p, _p+count)
		(*itb++).~T();
	count = 0;
}
template <class T>/*****************************************************************************************************************************/
_vector<T>::~_vector() {
	if(_p) {
		clear();
		free(_p);
		_p = 0;
		_size = 0;
	}
}

template <class T>/*****************************************************************************************************************************/
T& _vec<T>::operator[](_int_ _n) const {
	return _p[_n];
}
template <class T>/*****************************************************************************************************************************/
T& _vec<T>::at(_int_ _n) const {
	return _p[_n];
}

template <class T>/*****************************************************************************************************************************/
_int_ _vec<T>::size() const {
	return count;
}

template <class T>/*****************************************************************************************************************************/
_int_ _vec<T>::capacity() const {
	return _size;
}

template <class T>/*****************************************************************************************************************************/
T* _vec<T>::get_ptr () const{
	return _p;
}
template <class T>/*****************************************************************************************************************************/
T& _vec<T>::front () const{
	return *_p;
}
template <class T>/*****************************************************************************************************************************/
T& _vec<T>::back()const{
	return _p[_size-1];
}

template <class T>/*****************************************************************************************************************************/
inline int _vec<T>::find(const T* _t) {
	int pos = _t - _p;
	return (pos < 0 || pos+1 > count) ? -1 : pos;
}

template <class T>/*****************************************************************************************************************************/
inline bool _vec<T>::empty() const{
	return !count;
}

template <class T>/*****************************************************************************************************************************/
inline void _vec<T>::fprint_info(FILE* str) const{
	fprintf(str, "\nsize: %d\nspace: %d\n{\n", size(), capacity());
}
template <class T>/*****************************************************************************************************************************/
void _vec<T>::fprint_floatvec(FILE* str, short interval, int typesize, int stride = 1, int begin_add = 0) const{
	fprint_info(str);

	const int _s = size()*(typesize/sizeof(GLfloat));
	GLfloat* ptr = (GLfloat*)get_ptr();
	ptr += begin_add;
	for(int i = 0, c = 0; i < _s; i+=stride, ++c){
		for(int j = 0; j < interval; ++j, ++i){
			if(i == _s - 1)
				fprintf(str, "%f ", *(ptr+i));
			else 
				fprintf(str, "%f, ", *(ptr+i));
		}
		fprintf(str, "\n");    
	}
	fprintf(str, "\n}\n");
}

template <class T>/*****************************************************************************************************************************/
void _vec<T>::fprint_uintvec( FILE* str, short interval, int typesize) const{ 
	fprint_info(str);

	const int _s = size()*(typesize/sizeof(GLuint));
	GLuint* ptr = (GLuint*)get_ptr();
	for(int i = 0; i < _s; ++i){
		if(i == _s - 1)
			fprintf(str, "%d ", *(ptr+i));
		else 
			fprintf(str, "%d, ", *(ptr+i));
		if((i % interval) == interval-1) fprintf(str, "\n");  
	}
	fprintf(str, "\n}\n");
}



template <class T>/*****************************************************************************************************************************/
_vectorm<T>::_vectorm(_int_ elems) {
	count = _size = 0;
	_p = 0;
	if(elems > 0)
		_p = (T*)malloc(sizeof(T)*(_size = elems));
}
template <class T>/*****************************************************************************************************************************/
_vectorm<T>::_vectorm(const _vectorm<T>& a) {
	count = a.count;
	_size = 0;
	_p = 0;
	if(a._size > 0) {
		_p = (T*)malloc(sizeof(T)*(_size = a._size));
		memcpy(_p, _p+count, sizeof(T)*count);
	}
}

template <class T>/*****************************************************************************************************************************/
_vectorm<T>& _vectorm<T>::operator=(const _vectorm<T>& a){
	if(this == &a) return *this;
	if(a.count > _size) _realloc(a.count);
	
	memcpy(_p, a._p, sizeof(T)*a.count);
	count = count > a.count ? count : a.count;

	return *this;
}

template <class T>/*****************************************************************************************************************************/
void _vectorm<T>::resize(_int_ memory, const T& t) {
	if(memory > _size) {
		_realloc(memory);
		poll(_p+count, _p+_size) memcpy(itb++, &t, sizeof(T));
	}
	else if (memory == _size) return;
	else _realloc(memory);
	count = _size;
}
template <class T>/*****************************************************************************************************************************/
void _vectorm<T>::resize(_int_ memory) {
	if(memory > _size) 
		_realloc(memory);
	else if (memory == _size) return;
	else _realloc(memory);
	count = _size;
}

template <class T>/*****************************************************************************************************************************/
void _vectorm<T>::reserve(_int_ memory) {
	if(memory > _size)
		_realloc(memory);
}
template <class T>/*****************************************************************************************************************************/
inline void _vectorm<T>::push_back(const T& _t) {
	if(!memcmp(&count, &_size, sizeof(_int_)))
		_realloc(count ? count*2 : 1);

	memcpy(_p+count++, &_t, sizeof(T));
}

template <class T>/*****************************************************************************************************************************/
inline void _vectorm<T>::push_back_n(const T& _t) {
	find(_t) ?  0 : push_back(_t);
}

template <class T>/*****************************************************************************************************************************/
inline void _vectorm<T>::insert(int pos, const T& v) {
	if(pos >= count) return push_back(v);
	
	if(!memcmp(&count, &_size, sizeof(_int_)))
		_realloc(count ? count*2 : 1);

	memmove(_p+pos+1, _p+pos, sizeof(T)*(count++-pos));
	new (_p + pos) T(v);
}
template <class T>/*****************************************************************************************************************************/
inline void _vectorm<T>::insert(int pos, int n, const T& v) {
	if(pos >= count) pos = count;
	if(_size < count+n)
		_realloc(count ? (count+n)*2 : n);

	memmove(_p+pos+n, _p+pos, sizeof(T)*(count-pos));
	count += n;
	poll(_p+pos, _p+pos+n) memcpy(itb++, &v, sizeof(T));
}
template <class T>/*****************************************************************************************************************************/
inline void _vectorm<T>::insert_n(int pos, const T& v) {
	find(v) ? 0 : insert(pos, v);
}

template <class T>/*****************************************************************************************************************************/
inline T* _vectorm<T>::find(const T& _t) {
	poll(_p, _p+count)
		if(!memcmp(itb++,_t,sizeof(T))) 
			return itb-1;
	return 0;
}

template <class T>/*****************************************************************************************************************************/
void _vectorm<T>::erase(const T* _t){
	erase(_vec<T>::find(_t));
}
template <class T>/*****************************************************************************************************************************/
void _vectorm<T>::erase(int i){
	if(count <= 0 || i < 0)
		return;

	memcpy(_p+i, _p+i+1, sizeof(T)*(count-i));
	--count;
}

template <class T>/*****************************************************************************************************************************/
void _vectorm<T>::erase(int l, int r){
	if(count <= r-l || l < 0 || r < 0)
		return;

	memcpy(_p+l, _p+r+1, sizeof(T)*(count-r));
	count -= r-l+1;
}
template <class T>/*****************************************************************************************************************************/
void _vectorm<T>::trim(){
	resize(count);
}

template <class T>/*****************************************************************************************************************************/
void _vectorm<T>::operator+=(const T& _t){
	push_back(_t);
}

template <class T>/*****************************************************************************************************************************/
void _vectorm<T>::operator|=(const T& _t){
	push_back_n(_t);
}

template <class T>/*****************************************************************************************************************************/
_vectorm<T>& _vectorm<T>::operator()(const T& newelem){
	push_back(newelem);
	return *this;
}
template <class T>/*****************************************************************************************************************************/
void _vectorm<T>::clear(void){
	count = 0;
}
template <class T>/*****************************************************************************************************************************/
_vectorm<T>::~_vectorm() {
	if(_p) {
		clear();
		free(_p);
		_p = 0;
		_size = 0;
	}
}

#undef copy
#undef poll