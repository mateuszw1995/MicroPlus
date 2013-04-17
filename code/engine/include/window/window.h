#pragma once
#include "../options.h"

#include <Windows.h>
#undef min
#undef max
#include "../misc/stream.h"
#include "../error/error.h"
#include "../math/math.h"
#include "../event.h"
#include "timer.h"
#include <functional>

namespace db {
	namespace window {
		extern db::error errlog;

		class glwindow {
			friend struct math::rect_xywh;
			friend int WINAPI ::WinMain (HINSTANCE, HINSTANCE, LPSTR, int);
			static WNDCLASSEX wcl;
			static HINSTANCE hinst;
			static glwindow* context, *parent;
			static glwindow* getcurrent();

			HWND hwnd;
			HDC hdc;
			HGLRC hglrc;
			MSG wmsg;
			RECT srect;

			int minw, minh, maxw, maxh, bpp, style, exstyle,
			    cminw, cminh, cmaxw, cmaxh, menu, vsyn;
			const char* name;
			bool active, transparent, doublebuf;

			static LRESULT CALLBACK wndproc(HWND, UINT, WPARAM, LPARAM);
			
			void _poll(event::message&, WPARAM, LPARAM);
			
			timer triple_timer;
			bool doubled;
		public:
			enum mode {
				MINIMIZE = SW_MINIMIZE,
				MAXIMIZE = SW_MAXIMIZE,
				RESTORE = SW_RESTORE,
				SHOW = SW_SHOW,
				HIDE = SW_HIDE
			} show;

			enum flag {
				CAPTION = WS_CAPTION,
				MENU = CAPTION | WS_SYSMENU,
				RESIZABLE = CAPTION | WS_THICKFRAME,
				MAXIMIZE_BOX = MENU | WS_MAXIMIZEBOX,
				MINIMIZE_BOX = MENU | WS_MINIMIZEBOX,
				ALL = CAPTION | MENU | RESIZABLE | MINIMIZE_BOX | MAXIMIZE_BOX
			};

			db::event::state events;
				
			/* user settings */
			std::function<void (glwindow&)> resize; /* resize function */
			bool limit_resize; /* should we limit window resizing to arbitrary rect_wh's (see set_minimum/maximum_size */
			unsigned triple_click_delay; /* maximum delay time for the next click (after doubleclick) to be considered tripleclick (in miliseconds) */
			
			static bool init();
			glwindow(const char* name, bool limit_resize);
			
			bool create(const math::rect_xywh& client_rectangle, bool doublebuffer = true, int _menu = ALL, int bitsperpixel = 24),
			     swap_buffers(), 
				 focus_keyboard(), 
				 current(),
				 vsync(int);
#ifdef include_DWM
			bool transparency(bool);
#endif
			bool poll_events(event::message& out);

			void set_minimum_size(math::rect_wh),
			     set_maximum_size(math::rect_wh);
			bool set_window_rect(const math::rect_xywh&),
			     set_adjusted_rect(const math::rect_xywh&),
			     set_show(mode);
			int  set_caption(const char*);

			mode get_show() const;
			const char* get_caption() const;

			math::rect_wh 
				get_minimum_size() const, 
				get_maximum_size() const;
			math::rect_xywh
				get_window_rect() const,
				get_adjusted_rect() const;

			int get_vsync() const;
			bool is_menu() const, is_transparent() const, is_active() const, is_doublebuffered() const;

			void destroy();
			~glwindow();
		};


		extern bool set_display(int width, int height, int bpp);
		extern math::rect_xywh get_display();
		extern int get_refresh_rate();
		extern void warp_cursor(int x, int y);
		extern void cursor(bool flag);

		extern void mbx(const char* title, const char* content);
		extern void imbx(int title, int content);
		extern void smbx(string title, string content);
		extern void copy_clipboard (std::wstring& from);
		extern void paste_clipboard(std::wstring& to);

	}
}