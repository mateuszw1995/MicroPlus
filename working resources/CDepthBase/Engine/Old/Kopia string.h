#pragma once
using namespace std;

template<class T>
class _string
{
	int   strlen(const char *s);
	const char* strstr(const char *s1, const char *s2);
	char *strcpy(char *dst, const char *src);
	char* strncpy(char *s1, const char *s2, long n);
	char* strchr(const char *p, int ch);
	size_t strspn(const char *s1, const char *s2);
	size_t strcspn (const char *s, const char *reject);
	int strcmp(const char *a, const char *b) const;
public:
	const static int npos;
	char* cstr;
	unsigned len;


	_string(const char* a = 0, unsigned n = 0);
	_string(const _string&);
	_string(char);

	_string& _set(const char*, unsigned n = 0);
	_string& add(const char*);
	_string& add(const _string&);

	void erase(int pos, int n=-1);
	void clear();
	int empty();

	_string& insert(int, const _string&);
	_string& insert(int, const char*);
	_string& insert(int, int, char);

	int length();
	int size();

	int find(char, int spos = 0);
	int find(const _string&, int spos = 0);
	int find(const char*, int spos = 0);

	int find_first_of(const _string&, int spos = 0);
	int find_first_not_of(const _string&,int spos = 0);
	int find_last_of(const _string&, int spos = -1);
	int find_last_not_of(const _string&, int spos = -1);

	_string substr(unsigned, int n = -1);

	void operator=(const _string&);
	void operator=(const char*);
	_string& operator+=(const char*);
	_string& operator+=(const _string&);
	_string& operator()(const char*);
	_string& operator()(const _string&);
	int operator==(const _string&) const;
	int operator!=(const _string&) const;
	int operator==(const char*) const;
	int operator!=(const char*) const;
	char operator[](register unsigned);

	void tag_str(const char**, const char**, int);

	const char* c_str();

	static int s2i(const _string& s);
	static int hex2i(const _string& s);
	static _string i2s(const int& i);
	static _string d2s(double f, int precision = 2);

	~_string();

	static _string ivec2(int*);
	static _string ivec3(int*);
	static _string ivec4(int*);
	static _string vec2(float*);
	static _string vec3(float*);
	static _string vec4(float*);
	static _string* ivec2v(int*, int);
	static _string* ivec3v(int*, int);
	static _string* ivec4v(int*, int);
	static _string* vec2v(float*, int);
	static _string* vec3v(float*, int);
	static _string* vec4v(float*, int);
	static _string* fv2sv(float*, int);
	static _string* gen_name_suffixes(_string&, int);

};

template<class T>
_string operator+(_string&, _string&);
template<class T>
_string dosuffix(int nn);

#include "string.cpp"
