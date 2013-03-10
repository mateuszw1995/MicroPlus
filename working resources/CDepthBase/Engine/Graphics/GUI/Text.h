#pragma once
#include "../../../Dependencies/ftype/ft2build.h"
#include "../../../Dependencies/ftype/freetype/freetype.h"
#include "../../../Dependencies/ftype/freetype/ftbitmap.h"
#include "../../../Dependencies/ftype/freetype/ftglyph.h"
#include "../../../Dependencies/ftype/freetype/ftoutln.h"
#include "../../../Dependencies/ftype/freetype/fttrigon.h"
#include "../../Container/vector.h"
#include "../../Container/string.h"
#include "../../Math/maths.h"
#include "../../Graphics/Color.h"
#include "../../Graphics/Shader/MeshShader.h"
#include "../../Math/Transformation.h"

#pragma comment(lib, "freetype.lib")

class Font {
	int _size;
	bool imported;
public:
	struct UNIstr {
		_vectorm<unsigned short> s;
		UNIstr();
		UNIstr(const _vectorm<unsigned short>&);
		UNIstr(const _string&);
		operator _string();
		UNIstr& operator=(const _vectorm<unsigned short>&);
		UNIstr& operator=(const _string&);
		_vectorm<unsigned short>& operator ()();
	};
	struct Glyph {
		math::RectXYWHf g_rect;
		unsigned unicode;
		int bearing_x, bearing_y, advance;
		float u, v, u2, v2;
		
	//	int operator>(const Glyph&); 
	//	int operator==(const Glyph&); 
	};
	struct Range {
		unsigned left, right;
		Range(unsigned left, unsigned right);
	};
	int height;
	_vectorm<Glyph> glyphs;
	unsigned tex_id;
	math::RectWH tex_r;
public:
	int Import(const char* fname, int size, _vectorm<unsigned>* chars, _vectorm<Range>* ranges, bool filtering);
	Font();
	~Font();

};

class Printer {
	static void* vertptr;
	struct Char {
		struct _ver {
			int x,y;
			float u,v;
			RGBA_1 col;
		} points[4];
	};
	
	Font* f;

public:
	class Paragraph {
		static Char::_ver* itc, *ite;
		friend class Printer;
		Printer* p;
		void print(int);
	public:
		int x,y, left, n;
		int line_space, char_space;
		Font::UNIstr text;
		Paragraph();
		void Update();
	};
	_vectorm<Char> buffer;

	MeshShader sp;

	bool color;
	Printer(bool);
	Printer(bool, MeshShader);

	void Add(Paragraph*);
	void Remove(Paragraph*);
	Paragraph* getParagraph(int);

	void Print();
	void Attach_Font(Font*);
	_vectorm<Paragraph*> paragraph;
};

