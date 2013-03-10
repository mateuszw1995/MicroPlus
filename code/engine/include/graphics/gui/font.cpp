#pragma once
#include "font.h"
#include "../../error/error.h"
#include "../../error/errmacro.h"
namespace db {
	namespace graphics {
		extern db::error errlog;

		namespace gui {
			font_in::font_in() : inited(false) {
			}

			font_in::~font_in() {
				deinit();
			}

			bool font_in::init() {
				inited = true;
				int f = 1;
				errf(!FT_Init_FreeType(&library), f);
				return f != 0;
			}
			
			bool font_in::deinit() {
				if(!inited) return true;
				inited = false;
				return err(!FT_Done_FreeType(library)) == 0;
			}

			font_file::font_file() : pt(0) {}
			
			bool font_file::open(font_in& f, const char* filename, unsigned pt, std::pair<wchar_t, wchar_t> range) {
				std::vector<std::pair<wchar_t, wchar_t> > ranges;
				ranges.push_back(range);
				return open(f, filename, pt, ranges);
			}

			bool font_file::open(font_in& f, const char* filename, unsigned pt, std::vector<std::pair<wchar_t, wchar_t> > ranges) {
				std::basic_string<wchar_t> characters;
				for(unsigned i = 0; i < ranges.size(); ++i) {
					characters.reserve(characters.capacity() + ranges[i].second - ranges[i].first);

					for(unsigned l = ranges[i].first; l < ranges[i].second; ++l) 
						characters.append(1, l);
				}

				return open(f, filename, pt, characters);
			}

			bool font_file::open(font_in& fin, const char* filename, unsigned _pt, const std::wstring& characters) {
				pt = _pt;
				FT_Face face;
				int f = 1, error = FT_New_Face(fin.library, filename, 0, &face );
				
				errsf(error != FT_Err_Unknown_File_Format, "font format unsupported", f);
				errsf(!error, "coulnd't open font file", f);
				errsf(!FT_Set_Char_Size(face, 0, pt << 6, 96, 96), "couldn't set char size", f);
				errsf(!FT_Select_Charmap(face, FT_ENCODING_UNICODE), "couldn't set encoding", f);
				
				if(!f) return false;

				ascender = face->size->metrics.ascender >> 6;
				descender = face->size->metrics.descender >> 6;

				//glyphs.reserve(characters.size());
				//unicode.reserve(characters.size());

				FT_UInt g_index;
				glyph* g;

				for(unsigned i = 0; i < characters.size(); ++i) {
					g_index = FT_Get_Char_Index(face, characters[i]);

					if(g_index) {
						glyphs.push_back(glyph());
						g = &glyphs[glyphs.size()-1];

						g->index = g_index;
						g->unicode = characters[i];

						if(characters[i] == 0x00A0) {
							int asda;
							asda = 32;
						}

						errsf(!FT_Load_Glyph(face, g_index, FT_LOAD_DEFAULT | FT_LOAD_IGNORE_TRANSFORM), "couldn't load glyph", f);
						errsf(!FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL), "couldn't render glyph", f);

						if(face->glyph->bitmap.width)
							g->img.copy(face->glyph->bitmap.buffer, 1, face->glyph->bitmap.pitch, math::rect_wh(face->glyph->bitmap.width, face->glyph->bitmap.rows));
						
						g->adv = face->glyph->metrics.horiAdvance >> 6;
						g->bear_x = face->glyph->metrics.horiBearingX >> 6;
						g->bear_y = face->glyph->metrics.horiBearingY >> 6;
						g->width = face->glyph->metrics.width >> 6;
						g->height = face->glyph->metrics.height >> 6;
						unicode[characters[i]] = glyphs.size()-1;
					}
				}


				FT_Vector delta;
				if(FT_HAS_KERNING(face)) {
					for(unsigned i = 0; i < glyphs.size(); ++i) {
						for(unsigned j = 0; j < glyphs.size(); ++j) {
							FT_Get_Kerning(face, glyphs[j].index, glyphs[i].index, FT_KERNING_DEFAULT, &delta);
							if(delta.x) 
								glyphs[i].kerning.push_back(std::pair<unsigned, int>(glyphs[j].unicode, delta.x >> 6));
						}

						glyphs[i].kerning.shrink_to_fit();
					}
				}
				
				FT_Done_Face(face);
				return f != 0;
			}
			
			font_file::glyph* font_file::get_glyphs() {
				return glyphs.data();
			}

			font_file::glyph* font_file::get_glyph(unsigned _unicode) {
				return glyphs.data() + unicode[_unicode];
			}

			unsigned font_file::get_count() const {
				return glyphs.size();	
			}
			
			unsigned font_file::get_pt() const {
				return pt;
			}

			void font_file::free_images() {
					for(unsigned i = 0; i < glyphs.size(); ++i)
						glyphs[i].img.destroy();
			}
			
			void font_file::destroy() {
				glyphs.clear();
				glyphs.shrink_to_fit();
			}
			
			font::font() : glyphs(0), parent(0), bold(0), italics(0), regular(this) {}

			font::~font() { 
				destroy();
			}

			void font::build(font_file* _parent) {
				destroy();
				parent = _parent;
				glyphs = new glyph[parent->get_count()];
				for(unsigned i = 0; i < parent->get_count(); ++i) {
					glyphs[i].info = parent->glyphs.data() + i;
					glyphs[i].tex.set(&glyphs[i].info->img);
					glyphs[i].tex.luminosity_to_alpha(true);
				}
			}
			
			void font::add_to_atlas(io::input::atlas& atl) {
				for(unsigned i = 0; i < parent->get_count(); ++i) {
					if(glyphs[i].tex.get_rect().w) {
						atl.textures.push_back(&glyphs[i].tex);
					}
				}
			}

			void font::set_styles(font* b, font* i, font* bi) {
				b->regular	= i->regular = bi->regular = this;
				b->bold		= i->bold    = bi->bold    = b;
				b->italics	= i->italics = bi->italics = i;
				b->bi		= i->bi		 = bi->bi	   = bi;
				bold = b; italics = i; bi = bi;
			}
			
			bool font::can_be_bolded() {
				return (regular == this && bold) || (italics == this && bi) || this == bold || this == bi;
			}

			bool font::can_be_italicsed() {
				return (regular == this && italics) || (bold == this && bi) || this == italics || this == bi;
			}
			
			bool font::is_bolded() {
				return this == bold || this == bi;
			}

			bool font::is_italicsed() {
				return this == italics || this == bi;
			}

			font* font::get_bold(bool flag) {
				if(flag) {
					if(this == regular || this == bold) return bold ? bold : regular;
					if(this == italics || this == bi)   return bi   ? bi   : italics;
				}
				else {
					if(this == regular || this == bold) return regular;
					if(this == italics || this == bi)   return italics ? italics : bi;
				}
				return 0;
			}

			font* font::get_italics(bool flag) {
				if(flag) {
					if(this == regular || this == italics) return italics ? italics : regular;
					if(this == bold    || this == bi)      return bi      ? bi : bold;
				}
				else {
					if(this == regular || this == italics) return regular;
					if(this == bold    || this == bi)      return bold ? bold : bi;
				}
				return 0;
			}

			void font::destroy() {
				if(glyphs) {
					delete [] glyphs;
					glyphs = 0;
				}
			}

			font::glyph& font::get_glyph(unsigned unicode) {
				return glyphs[parent->unicode[unicode]];
			}

			font_file* font::get_font() {
				return parent;
			}
		}
	}
}