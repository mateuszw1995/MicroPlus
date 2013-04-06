#pragma once
#include "../../misc/container/registry.h"
#include "../../math/math.h"
#include "image.h"
#include <vector>

using namespace db::math;
using namespace pack;
using namespace std;

/* zrobic w klasie texture atlasu dodawanie tekstur i generowanie in place, potem do poszczegolnych interfejsow uzytkownik sam daje wskaznik do atlasu == elastycznosc i mozliwosc all in one
 + statyczna funkcja na multiple atlas distribution
 jeszcze taka opcja ze do texture atlas funkcja pack() zeby mozna bylo robic cos w stylu online packing
 najlepiej jakby przyjmowal image's i zwracal jakies info typu rect_texture dla kazdego
 kurwa po ludzku musi to wygladac
 tekstura NIE MUSI wiedziec do jakiego atlasu nalezy, interfejsy maja wiedziec and thats sufficient 
 
 */

namespace db {
	namespace graphics {
		namespace gui {
			struct font_file;
			struct font;
		}
		namespace io {
			namespace input {
				class texture {
					friend class atlas;
					image* img;
					rect_xywhf rect;
					float x, y, w, h;
					bool ltoa;

					void set_uv_unit(double, double);
				public:

					texture();
					texture(image* img);
					void set(image* img);
					void luminosity_to_alpha(bool);

					const rect_xywhf& get_rect() const;
					
					void translate_uv(pointf pixels);
					void scale_uv(float u_scalar, float v_scalar);
					void get_uv(const rect_texture& uv, rect_texture& out) const;
					void get_uv(float u, float v, float& u_out, float& v_out) const;

					float get_u_unit() const, get_v_unit() const;
				};

				class atlas {
					static unsigned current;
					unsigned id;
					bool mipmaps, lin, rep, built;
					vector<rect_xywhf*> ptr_arr; // for reallocations
					bin b;
					float adder, mult;
					
				public:
					atlas(int white_pixel_size = 0); ~atlas();

					static bool gen_packed(vector<texture*>& in_textures, 
										   vector<atlas*>& inout_atlases);
					
					vector<texture*> textures;

					texture atlas_texture;
					image img;

					void quick_add(image* in_images, texture* out_textures, int n_tex, gui::font_file* in_files, gui::font* out_fonts, int n_fonts);
					bool pack(), // max texture size 
						 pack(int max_size);

					void create_image(int atlas_channels, bool destroy_images);
					void create_raw_texture(const image&);
					void build(bool mipmaps = false, bool linear = false, image* raw_texture = 0), bind(), _bind(), nearest(), linear(), clamp(), repeat();
					
					bool is_mipmapped() const;
					void destroy();
				};
			}
		}
	}
}