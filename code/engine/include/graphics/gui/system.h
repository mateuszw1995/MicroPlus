#pragma once
#include <vector>
#include "../io/texture.h"
#include "../../math/math.h"
#include "../../event.h"
#include "../pixel.h"
#include "../../window/timer.h"
/* window - defined as the rect that fetches events to only itself 

todo:
w textboxie poprawic przeciaganie zaznaczen double/triple clickow

z bboxami ma byc tak jak jest bo zauwaz ze bboxa rozpychaja tylko pierwsze child recty a nie ich wlasne bboxy
BBOX JEST GLOBALNY!!!

*/

namespace db {
	namespace graphics {
		extern window::fpstimer* fps;
		namespace gui {
			struct font;
			using namespace math;
			extern io::input::texture* null_texture;
			
			struct material {
				io::input::texture* tex;
				pixel_32 color;
				material(io::input::texture* = null_texture, const pixel_32& = pixel_32()); 
			};
			
			struct vertex {
				int x, y;
				float u, v;
				pixel_32 col;
			};

			struct quad {
				vertex p[4];
				quad();
				quad(const rect_ltrb&, const material&, const rect_texture& = rect_texture(0.f, 0.f, 1.f, 1.f));

				static quad clipped(const rect_ltrb&, const rect_ltrb&, const material&);
				void clip(const rect_ltrb&);
				void move(const point&);
				void set (const rect_ltrb&);
				void rotate90(int times);
				rect_ltrb get_rect() const;
			};
			
			struct formatted_char {
				font* font_used;
				wchar_t c;
				unsigned char r, g, b, a;
				void set(wchar_t, font* = 0, const pixel_32& = pixel_32());
				void set(font* = 0, const pixel_32& = pixel_32());
			};
			
			struct style {
				font* f;
				pixel_32 color;
				style(font*, pixel_32);
				style(const formatted_char&);
				operator formatted_char();
			};

			typedef std::basic_string<formatted_char> fstr;

			struct rect;
			class system {
				bool own_copy, own_clip;
				rect* focus;
			public:
				struct {
					material mat;
					rect_wh size;
					point pos;
					rect* subject;
					float speed_mult;
				} middlescroll;

				fstr clipboard;

				rect *lholded, *rholded;
				event::state* events;

				std::vector<rect*> windows;
				std::vector<quad> rect_array;


				system(event::state* subscribe_events);

				void copy_clipboard(fstr&);
				void set_focus(rect*);
				rect* get_focus() const;

				void bind();

				bool fetch_clipboard, is_clipboard_own();

				void update_rectangles();
				void poll_events      ();
				void call_updaters    ();
				void update_array     ();
				void default_update   ();
			};

			extern void formatted_text(const wchar_t*, fstr&, style);
			extern fstr formatted_text(const wchar_t*, style);
			extern void paste_clipboard(fstr& out, formatted_char = formatted_char());
			extern void scale_virtual_res(rect_wh vres, rect_wh display, vector<quad>& quads);
		}
	}
	namespace misc {
		extern std::wstring wstr(const graphics::gui::fstr& f);
		template <class T>
		T wnum(const graphics::gui::fstr& f) {
			std::wistringstream ss(wstr(f));
			T v;
			ss >> v;
			return v;
		}
	}
}

