#pragma once

/************************************************************************************************************************************************/
template<class T> 
_list<T>::Node::Node(Node* prev, Node* next, const T& _v) : 
prev(prev), 
	next(next),
	_v(_v) 
{}

/************************************************************************************************************************************************/
template<class T> /***********************************************************************************************/
_list<T>::_list() : first(0), last(0), _size(0) {} 
template<class T> /***********************************************************************************************/
_list<T>::_list(int n, const T& v) : first(0), last(0), _size(0) { resize(n, v); } 
template<class T> /***********************************************************************************************/
_list<T>::_list(const _list& b) : first(0), last(0), _size(0)  {
	Node* it = b.first;
	while(it) {
		push_back(it->_v);
		it = it->next;
	}
} 
template<class T> /***********************************************************************************************/
inline _list<T>& _list<T>::operator=(const _list& b){ 
	clear();
	Node* it = b.first;
	while(it) {
		push_back(it->_v);
		it = it->next;
	}
	return *this;
} 

template<class T>
bool _list<T>::operator==(const _list& b) {
	if(size() != b.size()) return false;
	Node* it1 = first, *it2 = b.first;
	while(it1) {
		if(!(it1->_v == it2->_v)) return false;
		it1 = it1->next;
		it2 = it2->next;
	}
	return true;
}

template<class T> /***********************************************************************************************/
inline int _list<T>::get_index(const Node* const n) const{ 
	Node* it = first;
	int i = 0;
	while(it) {
		if(it == n) return i;
		it = it->next;
		++i;
	}
} 
template<class T> /***********************************************************************************************/
typename _list<T>::Node* _list<T>::get_node(_int_ n) const{ 
	Node* it;
	if(n < _size/2) {
		it = first;
		while(n--) it = it->next;
	}
	else {
		n = _size-n;
		it = last;
		while(n--) it = it->prev;
	}
	return it;
} 
template<class T> /***********************************************************************************************/
inline typename _list<T>::Node* _list<T>::find(const T& v) const{
	Node* it = first;
	while(it) {
		if(it->_v == v) return it;
		it = it->next;
	}
	return NULL;
} 
template<class T> /***********************************************************************************************/
inline typename int _list<T>::ifind(Node* v) const{
	int index = 0;
	Node* it = first;
	while(it) {
		if(it == v) return index;
		++index;
		it = it->next;
	}
	return -1;
}
template<class T> /***********************************************************************************************/
inline void _list<T>::resize(unsigned n, const T& v){ 
	if(n > _size) {
		n = n-_size;
		while(n--) push_back(v);
	}
	else if (n == _size) return;
	else {
		n = _size-n;
		while(n--) pop_back();
	}
} 
template<class T> /***********************************************************************************************/
inline typename _list<T>::Node* _list<T>::push_front(const T& v){
	++_size;
	if(first)
		return first->next->prev = (first = new Node(0, first, v));
	else
		return first = last = new Node(0, 0, v);
} 
template<class T> /***********************************************************************************************/
inline typename _list<T>::Node* _list<T>::push_back(const T& v){ 
	++_size;
	if(last)
		return last->prev->next = (last = new Node(last, 0, v));
	else
		return last = first = new Node(0, 0, v);
} 
template<class T> /***********************************************************************************************/
inline typename void _list<T>::pop_front(){ 
	static Node* n;
	if(first) {
		n = first->next;
		if(first == last) last = 0;
		else n->prev = 0;
		delete first;
		first = n;
		--_size;
	}
} 
template<class T> /***********************************************************************************************/
inline typename void _list<T>::pop_back(){ 
	static Node* n;
	if(last) {
		n = last->prev;
		if(first == last) first = 0;
		else n->next = 0;
		delete last;
		last = n;
		--_size;
	}
} 
template<class T> /***********************************************************************************************/
inline typename _list<T>::Node* _list<T>::insert(unsigned n, const T& v){ 
	if(!n) return push_front(v);
	++_size;
	if(!first)
		return first = last = new Node(0, 0, v);

	Node* it = first;
	while(n--)
		it = it->next;

	return it ? (it->prev->next = ((it = new Node(it->prev, it, v))->next->next ? it->next->next->prev = it : it)) : (last->prev->next = (last = new Node(last, 0, v)));	
} 
template<class T> /***********************************************************************************************/
inline _int_ _list<T>::size() const { return _size; } 
template<class T> /***********************************************************************************************/
inline typename _list<T>::Node* _list<T>::push_front_n(const T& v){ return find(v) ? 0 : push_front(v);  } 
template<class T> /***********************************************************************************************/
inline typename _list<T>::Node* _list<T>::push_back_n(const T& v){ return find(v) ? 0 : push_back(v); } 
template<class T> /***********************************************************************************************/
inline typename _list<T>::Node* _list<T>::insert_n(unsigned n, const T& v){ return find(v) ? 0 : insert(n, v); } 
template<class T> /***********************************************************************************************/
inline typename _list<T>::Node* _list<T>::operator+=(const T& v){ return push_back(v); } 
template<class T> /***********************************************************************************************/
inline typename _list<T>::Node* _list<T>::operator|=(const T& v){ return push_back_n(v); } 
template<class T> /***********************************************************************************************/
__forceinline T& _list<T>::operator[](_int_ n) const{ return get_node(n)->_v; } 
template<class T> /***********************************************************************************************/
void _list<T>::erase(Node* n){ 
	if(n == last) pop_back(); 
	else if(n == first) pop_front();
	else if(n) {
		if(n->next) n->next->prev = n->prev;
		if(n->prev) n->prev->next = n->next;
		delete n;
		--_size;
	}
} 
template<class T> /***********************************************************************************************/
void _list<T>::erase(Node* nb, Node* ne){ 
	if(!nb || !ne) return;
	(nb->prev ? nb->prev->next : first) = ne->next;
	(ne->next ? ne->next->prev : last ) = nb->prev;
	Node* tmp;
	while(nb != ne) {
		tmp = nb->next;
		delete nb;
		nb = tmp;
		--_size;
	}
	delete nb;
} 
template<class T> /***********************************************************************************************/
void _list<T>::clear(){ erase(first, last); first = last = 0; } 
template<class T> /***********************************************************************************************/
void _list<T>::on_top(Node* n){
	if(!n || n == last || first == last ) return;
	if(n == first) {
		first = first->next;
		first->prev = 0;
	}
	else 
		(n->prev->next = n->next)->prev = n->prev;

	Node* tmp = last;
	((last = last->next) = n)->next = 0;

	(n->prev = tmp)->next = last;
} 
template<class T> /***********************************************************************************************/
_list<T>::~_list(){clear();} 