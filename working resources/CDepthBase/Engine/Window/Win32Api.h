#pragma once
#include <Windows.h>

namespace DB {
	namespace Window {
		namespace details {
			struct _systimerdata {
				LARGE_INTEGER freq, ticks;
				static LARGE_INTEGER cmp;

			};

			struct Win32 {
				struct wminfo {
					HINSTANCE hInstance; 
					HINSTANCE hPrevInstance; 
					LPSTR lpCmdLine; 
					int nCmdShow;
				};

				static WNDCLASSEX wc;
				static HWND hwnd;
				static HDC mhDC;
				static HGLRC mhRC;
				static wminfo wmain;
				static const char* classname;
				static MSG msg;

				static LRESULT CALLBACK Events (HWND, UINT, WPARAM, LPARAM);
			};
		}
	}
}