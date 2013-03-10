#pragma once
#include "../Container/string.h"
#include <cstdlib>

const int _string::npos = -1;

_string operator+(_string& a, _string& b){
	static _string temp;
	temp = a;
	temp += b;
	return temp;
}


_string::_string(const char* a, unsigned n) {
	cstr = 0;
	len = 0;
	_set(a, n);

};
_string::_string(const _string& newstr) {	
	cstr = 0;
	len = 0;
	_set(newstr.cstr);

};
_string::_string(char a)
{
	cstr = 0;
	char* pt = new char[2];
	pt[0] = a;
	pt[1] = '\0';
	_set(pt);
	delete [] pt;

}

_string& _string::_set(const char* a, unsigned n){
	if(!a) {
		cstr = new char[1];
		cstr[0] = '\0';
		return *this;
	}
	if(cstr) {
		delete [] cstr;
		cstr = 0;
	}
	len = strlen(a);
	if(n && n < len) len = n; 
	if(len <= 0) len = 0;
	cstr = new char[len+1];
	strncpy(cstr, a, len );
	cstr[len] = '\0';
	return *this;
}

_string& _string::add(const char* a){
	if(!len || !cstr) { return _set(a); }
	int nlen = strlen(a);
	if(nlen <= 0) return *this;
	char* bigbuf = new char[len + nlen + 1];
	strncpy(bigbuf, cstr, len);
	strncpy(bigbuf+len, a, nlen);
	bigbuf[len + nlen] = '\0'; 
	if(cstr && len)
		delete [] cstr;
	cstr = bigbuf;
	len = strlen(cstr);
	return *this;
}

_string& _string::add(const _string& b){
	const char* a = b.cstr;
	if(!len || !cstr) { return _set(a); }
	int nlen = strlen(a);
	if(nlen <= 0) return *this;
	char* bigbuf = new char[len + nlen + 1];
	strncpy(bigbuf, cstr, len);
	strncpy(bigbuf+len, a, nlen);
	bigbuf[len + nlen] = '\0'; 
	if(cstr && len)
		delete [] cstr;
	cstr = bigbuf;
	len = strlen(cstr);
	return *this;
}

_string& _string::operator +=(const char* a){
	return add(a);
}
_string& _string::operator +=(const _string& a){
	return add(a.cstr);
}
_string& _string::operator()(const char* a) {
	return add(a);
}
_string& _string::operator()(const _string& a) {
	return add(a.cstr);
}

void _string::clear(){
	delete [] cstr;
	cstr = 0;
	len = 0;
}

int _string::empty(){
	return !len;
}

_string& _string::insert(int pos, const _string& sub){
	if(pos > size()-1) return add(sub.cstr);

	if(!cstr) {_set(sub.cstr); return *this; }
	char* bigbuf = new char[len + sub.len + 1];
	strncpy(bigbuf, cstr, pos);
	strcpy(bigbuf+pos, sub.cstr);
	strcpy(bigbuf+pos+sub.len, cstr+pos);
	delete [] cstr;
	cstr = bigbuf;
	cstr[len + sub.len] = '\0';
	len = strlen(cstr);
	return *this;
}	
_string& _string::insert(int pos, const char* sub){
	if(pos > size()-1) return add(sub);

	if(!cstr) {_set(sub);  return *this; }
	char* bigbuf = new char[len + strlen(sub) + 1];
	strncpy(bigbuf, cstr, pos);
	strcpy(bigbuf+pos, sub);
	strcpy(bigbuf+pos+strlen(sub), cstr+pos);
	delete [] cstr;
	cstr = bigbuf;
	cstr[len + strlen(sub)] = '\0';
	len = strlen(cstr);
	return *this;
}
_string& _string::insert(int pos, int n, char c)
{
	char* buf = new char[n];
	while(n)
		buf[--n] = c;

	insert(pos, buf);
	delete [] buf;
	return *this;
}

void _string::erase(int pos, int n){
	if(!cstr || !n  || pos > size()-1) return;
	if(n == -1 || n >= len-pos) n = len-pos;
	char* smallbuf = new char[len-n+1];
	strncpy(smallbuf, cstr, pos);
	strcpy(smallbuf+pos, cstr+pos+n);
	delete [] cstr;
	cstr = smallbuf;
	len = strlen(cstr);
}
int _string::length(){
	return len;
}
int _string::size(){
	return len;
}

void _string::operator=(const _string& newstring){
	_set(newstring.cstr);
}
void _string::operator=(const char* newstring){
	_set(newstring);
}

int _string::find(char tofind, int spos){
	char* ptr = strchr(cstr+spos, tofind);
	if(!ptr) return -1;
	else 
		return (ptr-cstr)/sizeof(char);
}

int _string::find(const _string& tofind, int spos){
	const char* ptr = strstr(cstr+spos, tofind.cstr);
	if(!ptr) return -1;
	else 
		return (ptr-cstr)/sizeof(char);
}	

int _string::find(const char* tofind, int spos){
	const char* ptr = strstr(cstr+spos, tofind);
	if(!ptr) return -1;
	else 
		return (ptr-cstr)/sizeof(char);
}

int _string::find_first_of(const _string& tofind, int spos){
	int res = strcspn(cstr+spos, tofind.cstr);
	if(res+spos == len) return -1;
	else return res+spos;
}
int _string::find_first_not_of(const _string& tofind,int spos){
	int res = strspn(cstr+spos, tofind.cstr);
	if(res+spos == len) return -1;
	else return res+spos;
}

int _string::find_last_of(const _string& tofind, int spos){
	int cnt = (spos == -1 ? size() : spos) + 1;

	const char* _cstr = tofind.cstr;
	while(cnt >= 0)
	{
		--cnt;
		if(strchr(_cstr, cstr[cnt])) return cnt; 
	}
	return -1;
}

int _string::find_last_not_of(const _string& tofind, int spos){
	int cnt = (spos == -1 ? size() : spos);

	const char* _cstr = tofind.cstr;
	while(cnt >= 0)
	{
		--cnt;
		if(!strchr(_cstr, cstr[cnt])) return cnt+1; 
	}
	return -1;

}

char _string::operator[](register unsigned pos){
	return cstr[pos];
}
const char* _string::c_str(){
	return (cstr ? cstr : "");
}

_string _string::substr(unsigned  pos, int n){

	if(pos > len-1 || !cstr || n == 0)
		return _string("");
	if(n == -1) 
		n = len-pos;
	return _string(cstr+pos, n); 
}

int _string::operator==(const _string& other) const{
	if(len != other.len) return false;
	return !strcmp(cstr, other.cstr);
}
int _string::operator!=(const _string& other) const{
	if(len != other.len) return true;
	return !strcmp(cstr, other.cstr);
}
int _string::operator==(const char* other) const{
	return !strcmp(cstr, other);
}
int _string::operator!=(const char* other) const{
	return !strcmp(cstr, other);
}
void _string::tag_str(const char** keywords, const char** tags, int keycount)
{
	size_t i_pos = -1;
	int* lens = new int[keycount];
	int* taglens = new int[keycount*2];
	for(int i = 0; i < keycount; ++i)
		lens[i] = strlen(keywords[i]);
	for(int i = 0; i < keycount*2; ++i)
		taglens[i] = strlen(tags[i]);

	for(int i = 0; i < keycount; ++i)
		while(true) 
		{
			i_pos = find(keywords[i], i_pos+1);
			if(i_pos == npos) break;
			insert(i_pos, tags[i*2]);
			insert(i_pos+lens[i]+taglens[i*2], tags[i*2+1]); 
			i_pos += taglens[i*2]+taglens[i*2+1];
		}

		delete [] lens;
		delete [] taglens;

}


_string::~_string(){
	clear();
}


/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
_string _string::i2s(const int& i){
	char buf[33];
	itoa(i, buf, 10);
	return _string(buf);
}

/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
int _string::s2i(const _string& s){
	return atoi(s.cstr);
}
#include <sstream>
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
_string _string::d2s(double f, int precision){
	//static char buf[100];
	//itoa((int)f, buf, 10);
	//int _len = strlen(buf);
	//buf[_len] = '.'; 
	//buf[_len+1] = 0;
	//f -= (int)f;
	//while(precision--) {
	//	f *= 10.0; 
	//	buf[++_len] = '0' + (((int)f)%10);
	//}
	//buf[_len+1] = 0;
	//
	//return _string(buf);
	//	
	std::ostringstream buffer;
	buffer << f;
	return buffer.str().c_str();//(strstr(buffer.str().c_str(), ".") ?  _string(buffer.str().c_str()) : _string((buffer.str() + std::string(".0")).c_str()));
}
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
int _string::hex2i(const _string& s){
	// LOLOLOLOLOLOLO
	return 0;
}


_string _string::ivec2(int* v){
	return _string("ivec2(").add(i2s(v[0])).add(",").add(i2s(v[1])).add(")");
}
_string _string::ivec3(int* v){
	return _string("ivec3(").add(i2s(v[0])).add(",").add(i2s(v[1])).add(",").add(i2s(v[2])).add(")");
}
_string _string::ivec4(int* v){
	return _string("ivec4(").add(i2s(v[0])).add(",").add(i2s(v[1])).add(",").add(i2s(v[2])).add(",").add(i2s(v[3])).add(")");
}
_string _string::vec2(float* v){
	return _string("vec2(").add(d2s(v[0])).add(",").add(d2s(v[1])).add(")");
}
_string _string::vec3(float* v){
	return _string("vec3(").add(d2s(v[0])).add(",").add(d2s(v[1])).add(",").add(d2s(v[2])).add(")");
}
_string _string::vec4(float* v){
	return _string("vec4(").add(d2s(v[0])).add(",").add(d2s(v[1])).add(",").add(d2s(v[2])).add(",").add(d2s(v[3])).add(")");
}/*
 _string* _string::ivec2v(int* v, int){

 }		 
 _string* _string::ivec3v(int* v, int){

 }		 
 _string* _string::ivec4v(int* v, int){

 }		
 _string* _string::vec2v(float* v, int){

 }		
 _string* _string::vec3v(float* v, int){

 }		
 _string* _string::vec4v(float* v, int){

 }*/
_string* _string::fv2sv(float* f, int cnt) {
	_string* s = new _string[cnt];
	for(int i = 0; i < cnt; ++i) 
		s[i] = d2s(f[i]);
	return s;
}

_string dosuffix(int nn){
	static _string lightPrefix;
	lightPrefix = "[";
	lightPrefix += _string::i2s(nn);
	lightPrefix += "]";
	return lightPrefix;
}

_string* _string::gen_name_suffixes(_string& str, int cnt){
	_string* s = new _string[cnt];
	for(int i = 0; i < cnt; ++i) 
		s[i] = str + dosuffix(i);

	return s;
}

int   _string::strlen(const char *s){
	int len = 0;
	while(*s++) len++;
	return len;
}
const char* _string::strstr(const char *s1, const char *s2){
	int l1 = strlen(s1);
	int l2 = strlen(s2);

	while (l1 >= l2) {
		if (memcmp(s1, s2, l2) == 0) {
			return s1;
		}
		s1++;
		l1--;
	}
	return 0;
}
char *_string::strcpy(char *dst, const char *src){
	char *ret = dst;
	while (*dst++ = *src++) ;
	return ret;
}
char* _string::strncpy(char *s1, const char *s2, long n){
	int i;
	char *os1;

	os1 = s1;
	for(i = 0; i < n; i++)
		if((*s1++ = *s2++) == 0) {
			while(++i < n)
				*s1++ = 0;
			return os1;
		}
		return os1;
}
char* _string::strchr(const char *p, int ch){
	for (;; ++p) {
		if (*p == ch)
			return((char *)p);
		if (!*p)
			return((char *)NULL);
	}
}
size_t _string::strspn(const char *s1, const char *s2){
	const char *p = s1, *spanp;
	char c, sc;

	/*
	* Skip any characters in s2, excluding the terminating \0.
	*/
cont:
	c = *p++;
	for (spanp = s2; (sc = *spanp++) != 0;)
		if (sc == c)
			goto cont;
	return (p - 1 - s1);
}
size_t _string::strcspn (const char *s, const char *reject)
{
	size_t count = 0;

	while (*s != '\0')
		if (strchr (reject, *s++) == NULL)
			++count;
		else
			return count;

	return count;
}
int _string::strcmp(const char *a, const char *b) const
{
    while(*a && *b) {
        if(*a++ != *b++) return 1;
    }
    if(*a || *b) return 1;
    return 0;
}
