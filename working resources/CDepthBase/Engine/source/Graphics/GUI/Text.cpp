#pragma once
#include "../../../Graphics/GUI/Text.h"
#include "../../../Defines.h"
#include "../../../Math/maths.h"
#include "../../../Window/GLWindow.h"
#include "../../../Math/Transformation.h"
//
//int Font::Glyph::operator>(const Glyph& b) {
//	return unicode > b.unicode;
//}
//int Font::Glyph::operator==(const Glyph& b) {
//	return unicode == b.unicode;
//}


Font::UNIstr::UNIstr() {}
Font::UNIstr::UNIstr(const _vectorm<unsigned short>& v) : s(v) {}
Font::UNIstr::UNIstr(const _string& v) { 
	const char* it = v.c_str();
	while(it) s.push_back(*it++);
}
Font::UNIstr::operator _string() {
	char* t = new char[s.size()+1];
	t[s.size()] = 0;
	for(int i = 0; i < s.size(); ++i) 
		t[i] = (unsigned char)s[i];

	_string str(t);
	delete [] t;
	return str;
}

Font::UNIstr& Font::UNIstr::operator=(const _vectorm<unsigned short>& v) {
	s = v;
	return *this;
}
Font::UNIstr& Font::UNIstr::operator=(const _string& v) {
	s.clear();
	const char* it = v.c_str();
	while(*it) s.push_back(*it++);
	return *this;
}
_vectorm<unsigned short>& Font::UNIstr::operator()() {return s;}

Font::Range::Range(unsigned left, unsigned right) : left(left), right(right) {}

Font::Font() : _size(0), tex_id(0), imported(false) {}

int Font::Import(const char* fname, int size, _vectorm<unsigned>* chars, _vectorm<Range>* ranges, bool filtering) {
	using namespace math;
	Font::~Font();
	_size = size;
	//Create and initilize a freetype font library.
	FT_Library library;
	if(FT_Init_FreeType(&library)) return 0; 
	FT_Face face;

	if(FT_New_Face(library, fname, 0, &face)) return 0;
	//iMBX(face->num_faces, face->num_glyphs);
	FT_Set_Char_Size( face, size << 6, size << 6, 96, 96);
	height = face->size->metrics.height>>6;

	Glyph g;
	FT_Glyph_Metrics* mt;

	int many = 0;
	if(chars) many += chars->size();
	if(ranges)
		for(int i = 0; i < ranges->size(); ++i) 
			many += ranges->at(i).right-ranges->at(i).left+1;

	_vectorm<FT_Glyph> glyph;
	FT_Glyph add_g;
	bool missing_box = false;

	if(chars) {
		for(int i = 0; i < chars->size(); ++i) {
			FT_UInt indx = FT_Get_Char_Index(face, chars->at(i));
			FT_Load_Glyph(face, indx, FT_LOAD_DEFAULT);
			FT_Get_Glyph(face->glyph, &add_g);
			mt = &face->glyph->metrics;
			g.g_rect.w = mt->width>>6;
			g.g_rect.h = mt->height>>6;
			g.bearing_x = mt->horiBearingX>>6;
			g.bearing_y = mt->horiBearingY>>6;
			g.advance = mt->horiAdvance>>6;
			g.g_rect.flip = false;
			g.unicode = chars->at(i);
			if(!indx && missing_box) { 
				FT_Done_Glyph(add_g);
				continue;
			}
			if(!indx) missing_box = true;
			glyph.push_back(add_g);
			glyphs.push_back(g);
		}
	}  

	int left, right;
	if(ranges) {
		for(int i = 0; i < ranges->size(); ++i) {
			left = ranges->at(i).left;
			right = ranges->at(i).right;
			for(int j = left; j <= right; ++j) {
				FT_UInt indx = FT_Get_Char_Index(face, j);
				FT_Load_Glyph(face, indx, FT_LOAD_DEFAULT/*antialiasing ? (FT_LOAD_RENDER | FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT) : (FT_LOAD_RENDER | FT_LOAD_DEFAULT | FT_LOAD_TARGET_MONO)*/ );
				FT_Get_Glyph(face->glyph, &add_g);
				mt = &face->glyph->metrics;
				g.g_rect.w = mt->width>>6;
				g.g_rect.h = mt->height>>6;
				g.bearing_x = mt->horiBearingX>>6;
				g.bearing_y = mt->horiBearingY>>6;
				g.advance = mt->horiAdvance>>6;
				g.g_rect.flip = false;
				g.unicode = j;
				if(!indx && missing_box) { 
					FT_Done_Glyph(add_g);
					continue;
				}
				if(!indx) missing_box = true;
				glyph.push_back(add_g);
				glyphs.push_back(g);
			}
		}
	}  
	//iMBX(glyphs.size(), 9999);
	int maxsize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxsize);

	tex_r = rectangle2Dpack(&glyphs.get_ptr()->g_rect, glyphs.size(), false, sizeof(Glyph)-sizeof(RectXYWHf), glyphs.size(), maxsize);

	GLubyte* texture = new GLubyte[2*tex_r.w*tex_r.h];
	memset(texture, 0, sizeof(GLubyte)*2*tex_r.w*tex_r.h);

	unsigned char* ptr, *buf;
	FT_Bitmap b;
	int yright;
	RectXYWHf* rc;
	/*
	struct {
	unsigned int bit:3;
	} o;
	int bit = 0;
	int _2[8] = {1, 2, 4, 8, 16, 32, 64, 128};*/

	for(int i = 0; i < glyphs.size(); ++i) {
		//origin.x = glyph[i]->
		FT_Glyph_To_Bitmap(glyph.get_ptr()+i, FT_RENDER_MODE_NORMAL, 0, true);
		b = ((FT_BitmapGlyph) glyph[i])->bitmap;
		rc = &glyphs[i].g_rect;
		glyphs[i].u  = (float)rc->x/(float)tex_r.w;
		glyphs[i].v  = (float)rc->y/(float)tex_r.h;
		glyphs[i].u2 = glyphs[i].u + (float)rc->w/(float)tex_r.w;
		glyphs[i].v2 = glyphs[i].v + (float)rc->h/(float)tex_r.h; 

		//	iMBX(rc->x, rc->y);
		//if(b.pitch != b.width)	iMBX(b.pitch, b.width);

		ptr = buf = b.buffer;
		yright = rc->y+rc->h;
		//iMBX(b.num_grays, FT_PIXEL_MODE_MONO);
		if(!rc->flip) 
			for(int y = rc->y; y < yright; ++y) {

				left = tex_r.w*y + rc->x;
				right = left+rc->w;

				for(int x = left; x < right; ++x)
					texture[2*x] = texture[2*x+1] = *ptr++;

			}
		else 
			for(int y = rc->y, i = 0; y < yright; ++y, ++i) {
				ptr = buf+i+b.width*(b.rows-1);
				left = tex_r.w*y + rc->x;
				right = left+rc->w;

				for(int x = left; x < right; ++x) {
					texture[2*x] = texture[2*x+1] = *ptr;
					ptr -= b.width;
				}
			}
	}
	Glyph* missingBox = 0;
	for(int i = 0; i < glyphs.size(); ++i)
		if(!glyphs[i].unicode) missingBox = &glyphs[i];

	Glyph* gtab = new Glyph[65536];

	if(missingBox) 
		for(int i = 0; i < 65536; ++i) 
			gtab[i] = *missingBox;
	else
		memset(gtab, 0, sizeof(Glyph)*65536);

	unsigned uni_max = 0;
	for(int i = 0; i < glyphs.size(); ++i) {
		gtab[glyphs[i].unicode] = glyphs[i];
		uni_max = max(glyphs[i].unicode, uni_max);
	}
	++uni_max;
	glyphs.resize(uni_max);

	memcpy(glyphs.get_ptr(), gtab, uni_max*sizeof(Glyph));
	delete [] gtab;

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,filtering ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,filtering ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);     
	glTexImage2D(GL_TEXTURE_2D,0,2,tex_r.w,tex_r.h,0,GL_LUMINANCE_ALPHA,GL_UNSIGNED_BYTE,texture);

	delete [] texture;

	FT_Done_Face(face);
	FT_Done_FreeType(library);
	imported = true;

	return 1;
}

Font::~Font() {
	if(imported) { 
		glDeleteTextures(1, &tex_id);
		imported = false;
	}
}

Printer::Char::_ver* Printer::Paragraph::itc, *Printer::Paragraph::ite;
Printer::Paragraph::Paragraph() : line_space(0), char_space(0), x(0), y(0), left(0), n(0), p(0) { 
}


void* Printer::vertptr = 0;
Printer::Printer(bool color) : f(0), color(color) {
	static Matrix<> mat;
	_string vsrc, fsrc;
	mat.identity();
	mat.ortho(0.f, (GLfloat)DB::Window::getWidth(), 0.f, (GLfloat)DB::Window::getHeight(), -1.0f, 1.0f);

	_string mats = "mat4(";
	for(int i = 0; i < 16; ++i) 
		mats(_string::d2s(mat.MATRIX[i]))(i == 15 ? ")" : ", ");

	if(color)
		vsrc("\n attribute vec4 at_Color;")
			("\n varying vec4 color;");
	vsrc("\n attribute vec2 at_Vertex;")
		("\n attribute vec2 at_Texcoord;")
		("\n varying vec2 texCoord;")
		("\n uniform mat4 ModelViewProjectionMatrix;")
	    ("\n\n\n void main(void) {\n\t texCoord = at_Texcoord;");
	if(color) vsrc("\n\t color = at_Color;");
		
	vsrc("\n\tgl_Position = ModelViewProjectionMatrix * vec4(at_Vertex.x, at_Vertex.y, 0, 1);\n}");

	
		if(color) fsrc("\n varying vec4 color;");
	fsrc("\n varying vec2 texCoord;")
		("\n uniform sampler2D diffuseMap;")
		("\n void main(void) {")
		("\n\t gl_FragColor = texture2D(diffuseMap, texCoord)");
	if(color) fsrc("* color");
	fsrc("; \n }");

	 __MeshShader::ComponentSet c;
	 c.Enable(__MeshShader::DIFFUSE_MAPPING);
	sp = MeshShader(__MeshShader(vsrc, fsrc, c));
	(*sp).modelViewProjection.set(mat.MATRIX);

}
Printer::Printer(bool color, MeshShader sp) : f(0), sp(sp), color(color) {
}

void Printer::Attach_Font(Font* ff) {
	f = ff;
}

void Printer::Add(Paragraph* p) {
	if(p->p) return;
	p->left = buffer.size();
	buffer.resize(buffer.size()+p->text().size());
	p->n = p->text().size();
	paragraph.push_back(p);
	p->p = this;
	p->Update();
}
void Printer::Remove(Paragraph* p) {
	int i;
	for(i = 0; i < paragraph.size(); ++i)
		if(p == paragraph[i])
			paragraph.erase(i);
	for(;i < paragraph.size(); ++i) 
		paragraph[i]->left -= p->n;

	buffer.erase(p->left, p->left+p->n-1);
	p->p = 0;

}
void Printer::Paragraph::Update() {
	if(!p) return;
	for(int i = 0; i < p->paragraph.size(); ++i)
		if(this == p->paragraph[i])
			print(i+1);
}
Printer::Paragraph* Printer::getParagraph(int n) {
	return paragraph[n];
}

void Printer::Paragraph::print(int i) {
	static unsigned short* it, *end;
	static int cursor, top, len, _len;
	static Font::Glyph g;

	len = text().size();
	_len = len;
	it = text().get_ptr();
	end = text().get_ptr()+len;
	while(it != end) {
		if(*it==' '||*it=='\n') --_len;
		++it;
	}

	if(n != _len) {
		for(;i<p->paragraph.size();++i)
			p->paragraph[i]->left += _len-n;

		if(_len < n) 
			p->buffer.erase(left+_len, left+n-1);
		else p->buffer.insert(left+n, _len-n, Printer::Char());
		n = _len;
	}

	cursor = top = 0;
	it = text().get_ptr();
	end = it+len;
	itc = (p->buffer.get_ptr()+left)->points;
	while(it != end) {
		if(*it == '\n') {
			cursor = 0;
			top -= p->f->height+line_space;
			++it;
			continue;
		}
		g = p->f->glyphs[*it >= p->f->glyphs.size() ? 0 : *it];
		if(*it != ' ') {
			if(!g.g_rect.flip) {
				itc[0].x = itc[3].x = cursor+x+g.bearing_x;
				itc[0].y = itc[1].y = top+y-(g.g_rect.h-g.bearing_y);
				itc[1].x = itc[2].x = itc[0].x + g.g_rect.w;
				itc[2].y = itc[3].y = itc[0].y + g.g_rect.h;

				itc[0].u = itc[3].u = g.u;
				itc[1].u = itc[2].u = g.u2;
				itc[0].v = itc[1].v = g.v2;
				itc[2].v = itc[3].v = g.v; 
			}
			else {
				itc[0].x = itc[3].x = cursor+x+g.bearing_x;
				itc[0].y = itc[1].y = top+y-(g.g_rect.w-g.bearing_y);
				itc[1].x = itc[2].x = itc[0].x + g.g_rect.h;
				itc[2].y = itc[3].y = itc[0].y + g.g_rect.w;

				itc[0].u = itc[1].u = g.u;
				itc[2].u = itc[3].u = g.u2;
				itc[0].v = itc[3].v = g.v;
				itc[1].v = itc[2].v = g.v2; 
			}
			itc += 4;
		}
		cursor += g.advance+char_space;
		++it;
	}

}

void Printer::Print() {
	(*sp).Use();
	
	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT );
	glPushAttrib(GL_TEXTURE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glEnableVertexAttribArray(__MeshShader::ATTRIB_VERTEX);
	glEnableVertexAttribArray(__MeshShader::ATTRIB_TEXCOORD);
	
	if(color)
		glEnableVertexAttribArray(__MeshShader::ATTRIB_COLOR);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, f->tex_id);
	glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

	glVertexAttribPointer(__MeshShader::ATTRIB_VERTEX, 2, GL_INT, GL_FALSE, sizeof(Char::_ver),   &buffer.get_ptr()->points[0].x);
	glVertexAttribPointer(__MeshShader::ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE,  sizeof(Char::_ver), &buffer.get_ptr()->points[0].u); 
	if(color) glVertexAttribPointer(__MeshShader::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE,  sizeof(Char::_ver), &buffer.get_ptr()->points[0].col);

	glDrawArrays(GL_QUADS, 0, buffer.size()*4);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glPopAttrib();
	glPopClientAttrib();
	
}
