#pragma once
template<typename T>
circular_buffer<T>::circular_buffer(int max_size) : maxlen(max_size+1), begin(0), end(0) {
	q = (T*)malloc(sizeof(T)*maxlen);
}

template<typename T>
circular_buffer<T>::circular_buffer(const circular_buffer<T>& b) {
	q = (T*)malloc(sizeof(T)*(maxlen = b.maxlen));
	(*this) = b;
}

template<typename T>
circular_buffer<T>& circular_buffer<T>::operator=(const circular_buffer<T>& b) {
	begin = 0;
	end = b.size();

	if(maxlen < end)
		q = (T*)realloc(q, sizeof(T)*end);

	if(end > 0) {
		if(b.begin < b.end)
			memcpy(q, b.q+begin, sizeof(T)*end);
		else {
			memcpy(q, b.q+b.begin, sizeof(T)*(b.maxlen-b.begin));
			memcpy(q+(b.maxlen-b.begin), b.q, sizeof(T)*(b.end));
		}
	}
	return *this;
}

template<typename T>
void circular_buffer<T>::push(const T& v){
	memcpy(q+end, &v, sizeof(T));
	end = (end == maxlen-1) ? 0 : (end + 1);
}

template<typename T>
void circular_buffer<T>::push(T* v, int n){
	if(end+n-1 <= maxlen-1) {
		memcpy(q+end, v, sizeof(T)*n);
		end += n; 
	}
	else {
		memcpy(q+end, v, sizeof(T)*(maxlen-end));
		memcpy(q, v+(maxlen-end), sizeof(T)*(n-(maxlen-end)));
		end += n - maxlen;
	}
}

template<typename T>
void circular_buffer<T>::pop() {
	begin = (begin == maxlen-1) ? 0 : (begin + 1);
}

template<typename T>
void circular_buffer<T>::pop_back() {
	end = (end == 0) ? maxlen-1 : (end - 1);
}

template<typename T>
bool circular_buffer<T>::empty() const {
	return (maxlen == 0) ? true : (begin == end);
}

template<typename T>
int circular_buffer<T>::size() const {
	return begin < end ? (end - begin) : (end + maxlen-begin);	
}

template<typename T>
int circular_buffer<T>::capacity() const {
	return maxlen;
}

template<typename T>
T& circular_buffer<T>::front() const {
	return q[begin];	
}

template<typename T>
T& circular_buffer<T>::back() const {
	return q[end];
}

template<typename T>
void circular_buffer<T>::clear() {
	begin = end = 0;
}

template<typename T>
T* circular_buffer<T>::get_ptr() {
	return q+begin;
}

template<typename T>
T& circular_buffer<T>::operator[](int n) {
	return q[begin+n];
}

template<typename T>
circular_buffer<T>::~circular_buffer() {
	delete [] q; 
}