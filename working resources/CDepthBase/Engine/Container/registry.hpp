#pragma once



/********************************************************************************************************************************/
template <class T>
Registry<T>::entry::entry() : use(0) {}
/********************************************************************************************************************************/
template <class T>
Registry<T>::entry::entry(const T& v) : use(0), _v(v), constant(false) {}
/********************************************************************************************************************************/
template <class T>
bool Registry<T>::entry::univalent() { return !(use > 1); } 

/********************************************************************************************************************************/
template <class T>
typename Registry<T>::HNode Registry<T>::MakeResource(const T& val) {
	static _list<entry>::Node* iter;
	iter = content.first;
	while(iter){
		if(iter->_v._v == val) return iter; 
		iter = iter->next;
	}
	return content.push_back(entry(val));
}

/********************************************************************************************************************************/
template <class T>
Registry<T>::Link::Link(void) : node(0) {}
/********************************************************************************************************************************/
template <class T>
Registry<T>::Link::Link(typename Registry<T>::HNode& _node) : node(_node){
	if(_node) ++(_node->_v.use);
};
/********************************************************************************************************************************/
template <class T>
Registry<T>::Link::Link(const Link& b){
	if(b.node) ++(b.node->_v.use);
	node = b.node;
}
/********************************************************************************************************************************/
template <class T>
typename Registry<T>::Link& Registry<T>::Link::operator=(const typename Registry<T>::Link& b){
	if(b.node) ++(b.node->_v.use);
	validate();
	node = b.node;
	return *this;
}
/********************************************************************************************************************************/
template <class T>
T& Registry<T>::Link::operator*() const{
	return node->_v._v;
}
/********************************************************************************************************************************/
template <class T>
Registry<T>::Link::~Link(){
	validate();
}
/********************************************************************************************************************************/
template <class T>
void Registry<T>::Link::validate(){
	if(node) 
		if((--(node->_v.use)) <= 0 && !node->_v.constant){
			Registry<T>::content.erase(node);
			node = 0;
		}
}

/********************************************************************************************************************************/
template <class T>
void Registry<T>::Link::mark_constant() { if(node) node->_v.constant = true; };
/********************************************************************************************************************************/
template <class T>
void Registry<T>::Link::unmark_constant() { if(node) node->_v.constant = false; };


/********************************************************************************************************************************/
template<class T>
Registry<T>::Link::Link(const T& val){
	node = Registry<T>::MakeResource(val); 
	++(node->_v.use);
}

/********************************************************************************************************************************/
template<class T>
_list<typename Registry<T>::entry> Registry<T>::content;