#pragma once
#include <ft2build.h>
#include <freetype/freetype.h>
#include <unordered_map>

#include "../io/texture.h"

namespace db {
	namespace graphics {
		namespace gui {
			class font_in {
				FT_Library library;
				friend struct font_file;
				bool inited;
			public:
				font_in();
				~font_in();

				bool init(), deinit();
			};

			struct font_file {
				struct glyph {
					io::image img;
					int adv, bear_x, bear_y, width, height;
					std::vector<std::pair<unsigned, int> > kerning;
				private:
					friend struct font_file;
					FT_UInt index, unicode;
				};

				font_file();
				
				bool open(font_in&, const char* filename, unsigned pt, std::pair<wchar_t, wchar_t> range);
				bool open(font_in&, const char* filename, unsigned pt, std::vector<std::pair<wchar_t, wchar_t> > ranges);

				bool open(font_in&, const char* filename, unsigned _pt, const std::wstring& characters);
				
				glyph* get_glyphs(), *get_glyph(unsigned unicode);
				unsigned get_count() const, get_pt() const;

				void free_images(), destroy();
			private:
//				font_file(const font_file&) = delete;
				friend struct font;
				friend class text_printer;
				int ascender, descender;
				std::vector<glyph> glyphs;
				unsigned pt;
				std::unordered_map<unsigned, unsigned> unicode;
			};
			
			struct font {
				struct glyph {
					font_file::glyph* info;
					io::input::texture tex;
				};
				font *bold, *italics, *bi, *regular;

				font(); ~font();
				void build(font_file*), add_to_atlas(io::input::atlas&), destroy();

				glyph& get_glyph(unsigned unicode);
				font_file* get_font();

				void set_styles(font* bold, font* italics, font* bi);

				bool can_be_bolded();
				bool can_be_italicsed();
				bool is_bolded();
				bool is_italicsed();
				font* get_bold(bool flag);
				font* get_italics(bool flag);
			private:
				friend class text_printer;
				glyph *glyphs;
				font_file* parent;
			};
		}
	}
}