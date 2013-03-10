#define WIN_32
#include "GLWindow.h"


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	DB::Window::Win32::wmain.hInstance = hInstance;
	DB::Window::Win32::wmain.hPrevInstance = hPrevInstance;
	DB::Window::Win32::wmain.lpCmdLine = lpCmdLine;
	DB::Window::Win32::wmain.nCmdShow = nCmdShow;

	return DB::DepthMain();
}

namespace DB {
	namespace Window {
		WNDCLASSEX Win32::wc;
		HWND Win32::hwnd;
		HDC Win32::mhDC;
		HGLRC Win32::mhRC;
		Win32::wminfo Win32::wmain;
		MSG Win32::msg;
		const char* Win32::classname = "dbasewind";
		LARGE_INTEGER _systimerdata::cmp;

		LRESULT CALLBACK Win32::Events (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
			using namespace Window;
			if(evptr) *evptr = msg;
			static POINTS p;
			switch (msg){
			case WM_CHAR:
				Event::Key::utf8 = char(wParam);
				break;
			case WM_UNICHAR:
				Event::Key::utf8 = char(wParam);
				break;
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			case WM_CLOSE: return 0;

			case WM_KEYDOWN:
				if(lParam & 0x40000000) if(evptr) *evptr = 0;
				Event::Key::state[wParam] = true;
				switch(wParam) {
				case VK_CONTROL: wParam = lParam & 0x1000000 ? VK_RCONTROL : VK_LCONTROL; break;
				case VK_SHIFT: wParam = lParam & 0x1000000 ? VK_RSHIFT : VK_LSHIFT; break;
				case VK_MENU: wParam = lParam & 0x1000000 ? VK_RMENU : VK_LMENU; break;
				}
				Event::Key::state[wParam] = true;
				Event::Key::key = wParam;
				break;								

			case WM_KEYUP:							
				Event::Key::state[wParam] = false;
				switch(wParam) {
				case VK_CONTROL: Event::Key::state[VK_RCONTROL] = Event::Key::state[VK_LCONTROL] = false; break;
				case VK_SHIFT:	 Event::Key::state[VK_RSHIFT] = Event::Key::state[VK_LSHIFT] = false; break;
				case VK_MENU:	 Event::Key::state[VK_RMENU] = Event::Key::state[VK_LMENU] = false; break;
				}
				Event::Key::key = wParam;
				break;


			case WM_LBUTTONDOWN:
				Event::Mouse::state[0] = Event::Key::state[Keys::LMouse] = true;  break;
			case WM_RBUTTONDOWN:				    
				Event::Mouse::state[1] = Event::Key::state[Keys::RMouse] = true;  break;
			case WM_LBUTTONDBLCLK:
				Event::Mouse::state[0] = Event::Key::state[Keys::LMouse] = true;  break;
			case WM_RBUTTONDBLCLK:				    
				Event::Mouse::state[1] = Event::Key::state[Keys::RMouse] = true;  break;
			case WM_LBUTTONUP:					    
				Event::Mouse::state[0] = Event::Key::state[Keys::LMouse] = false; break;
			case WM_RBUTTONUP:					    
				Event::Mouse::state[1] = Event::Key::state[Keys::RMouse] = false; break;
			case WM_MOUSEMOVE:
				p = MAKEPOINTS(lParam);
				Event::Mouse::xrel = p.x - Event::Mouse::x;
				Event::Mouse::yrel = p.y - Event::Mouse::y;
				Event::Mouse::x = p.x;
				Event::Mouse::y = p.y;
				if(!Event::Mouse::state[0]) {
					Event::Mouse::drag_origin.x = Event::Mouse::x;
					Event::Mouse::drag_origin.y = Event::Mouse::y;
				}

				break;

			case WM_ACTIVATE:							
				Event::active = (bool)(!HIWORD(wParam));
				break;

			case WM_GETMINMAXINFO:
				if(!fullscreen) {
					((MINMAXINFO*)lParam)->ptMinTrackSize.x = min_w;
					((MINMAXINFO*)lParam)->ptMinTrackSize.y = min_h;
					((MINMAXINFO*)lParam)->ptMaxTrackSize.x = max_w;
					((MINMAXINFO*)lParam)->ptMaxTrackSize.y = max_h;
				}
				break;

			case WM_SIZE:
				//width = LOWORD(lParam);
				//height = HIWORD(lParam);
				break;
			default:
				return DefWindowProc (hwnd, msg, wParam, lParam);
				break;
			}


			return 0;
		}

		void MBX(const char* title, const char* content) { 
			MessageBox(0, content, title, MB_OK); 
		};
		void iMBX(int title, int content) { 
			MessageBox(0, _string::i2s(content).c_str(), _string::i2s(title).c_str(), MB_OK); 
		}
		void sMBX(_string title, _string content) { 
			MessageBox(0, content.c_str(), title.c_str(), MB_OK); 
		}

		void Cursor(bool flag) {
			ShowCursor(flag);
		}
		void WarpCursor(int x, int y) {
			SetCursorPos(x, y);
		}

		int SetCaption(const char* name) {
			return SetWindowText(Win32::hwnd, name);
		}

		void Exit() {
			if ( Win32::mhRC ){
				wglMakeCurrent( NULL, NULL );
				wglDeleteContext( Win32::mhRC );
			}
			if(Win32::hwnd && Win32::mhDC) ReleaseDC( Win32::hwnd, Win32::mhDC );
			if(Win32::hwnd) DestroyWindow(Win32::hwnd);

			wglMakeCurrent (NULL, NULL);
			Win32::hwnd = NULL;
			Win32::mhDC = NULL;
			Win32::mhRC = NULL;
		}

		bool PollEvents() {
			if(PeekMessage(&Win32::msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&Win32::msg);
				DispatchMessage(&Win32::msg);
				return true;
			}
			return false;
		}

		void SwapBufs() {
			SwapBuffers(Win32::mhDC);
		}

		const EventObject
			Event::Key::Down = WM_KEYDOWN,
			Event::Key::Up = WM_KEYUP,
			Event::Key::Character = WM_CHAR,
			Event::Resize = WM_SIZE,
			Event::Quit = WM_CLOSE,
			Event::Activate = WM_ACTIVATE,
			Event::Mouse::Move = WM_MOUSEMOVE,
			Event::Mouse::LDoubleClick = WM_LBUTTONDBLCLK,
			Event::Mouse::RDoubleClick = WM_RBUTTONDBLCLK,
			Event::Mouse::LDown = WM_LBUTTONDOWN,
			Event::Mouse::LUp = WM_LBUTTONUP,
			Event::Mouse::RDown = WM_RBUTTONDOWN,
			Event::Mouse::RUp = WM_RBUTTONUP;

		const int
			Keys::LMouse		    = VK_LBUTTON      ,
			Keys::RMouse		    = VK_RBUTTON      ,
			Keys::Cancel		    = VK_CANCEL       ,
			Keys::MMouse		    = VK_MBUTTON      ,    
			Keys::Backspace	    = VK_BACK         ,
			Keys::Tab			= VK_TAB          ,
			Keys::Clear		    = VK_CLEAR        ,
			Keys::Enter		    = VK_RETURN       ,
			Keys::Shift		    = VK_SHIFT        ,
			Keys::Ctrl			= VK_CONTROL      ,
			Keys::Alt			= VK_MENU         ,
			Keys::Pause		    = VK_PAUSE        ,
			Keys::CapsLock	    = VK_CAPITAL      ,
			Keys::Esc   		    = VK_ESCAPE       ,
			Keys::Space		    = VK_SPACE        ,
			Keys::PageUp		    = VK_PRIOR        ,
			Keys::PageDown	    = VK_NEXT         ,
			Keys::End			= VK_END          ,
			Keys::Home		    = VK_HOME         ,
			Keys::Left		    = VK_LEFT         ,
			Keys::Up			    = VK_UP           ,
			Keys::Right		    = VK_RIGHT        ,
			Keys::Down		    = VK_DOWN         ,
			Keys::Select		    = VK_SELECT       ,
			Keys::Print		    = VK_PRINT        ,
			Keys::Execute		= VK_EXECUTE      ,
			Keys::PrintScreen	= VK_SNAPSHOT     ,
			Keys::Insert		    = VK_INSERT       ,
			Keys::Delete		    = VK_DELETE       ,
			Keys::Help		    = VK_HELP         ,
			Keys::LWin		    = VK_LWIN         ,
			Keys::RWin		    = VK_RWIN         ,
			Keys::Apps		    = VK_APPS         ,
			Keys::Sleep		    = VK_SLEEP        ,
			Keys::Numpad0		= VK_NUMPAD0      ,
			Keys::Numpad1		= VK_NUMPAD1      ,
			Keys::Numpad2		= VK_NUMPAD2      ,
			Keys::Numpad3		= VK_NUMPAD3      ,
			Keys::Numpad4		= VK_NUMPAD4      ,
			Keys::Numpad5		= VK_NUMPAD5      ,
			Keys::Numpad6		= VK_NUMPAD6      ,
			Keys::Numpad7		= VK_NUMPAD7      ,
			Keys::Numpad8		= VK_NUMPAD8      ,
			Keys::Numpad9		= VK_NUMPAD9      ,
			Keys::Multiply	    = VK_MULTIPLY     ,
			Keys::Add			= VK_ADD          ,
			Keys::Separator	    = VK_SEPARATOR    ,
			Keys::Subtract	    = VK_SUBTRACT     ,
			Keys::Decimal		= VK_DECIMAL      ,
			Keys::Divide		    = VK_DIVIDE       ,
			Keys::F1			    = VK_F1           ,
			Keys::F2			    = VK_F2           ,
			Keys::F3			    = VK_F3           ,
			Keys::F4			    = VK_F4           ,
			Keys::F5			    = VK_F5           ,
			Keys::F6			    = VK_F6           ,
			Keys::F7			    = VK_F7           ,
			Keys::F8			    = VK_F8           ,
			Keys::F9			    = VK_F9           ,
			Keys::F10			= VK_F10          ,
			Keys::F11			= VK_F11          ,
			Keys::F12			= VK_F12          ,
			Keys::F13			= VK_F13          ,
			Keys::F14			= VK_F14          ,
			Keys::F15			= VK_F15          ,
			Keys::F16			= VK_F16          ,
			Keys::F17			= VK_F17          ,
			Keys::F18			= VK_F18          ,
			Keys::F19			= VK_F19          ,
			Keys::F20			= VK_F20          ,
			Keys::F21			= VK_F21          ,
			Keys::F22			= VK_F22          ,
			Keys::F23			= VK_F23          ,
			Keys::F24			= VK_F24          ,
			Keys::A			    = 'A'            ,
			Keys::B			    = 'B'            ,
			Keys::C			    = 'C'            ,
			Keys::D			    = 'D'            ,
			Keys::E			    = 'E'            ,
			Keys::F			    = 'F'            ,
			Keys::G			    = 'G'            ,
			Keys::H			    = 'H'            ,
			Keys::I			    = 'I'            ,
			Keys::J			    = 'J'            ,
			Keys::K			    = 'K'            ,
			Keys::L			    = 'L'            ,
			Keys::M			    = 'M'            ,
			Keys::N			    = 'N'            ,
			Keys::O			    = 'O'            ,
			Keys::P			    = 'P'            ,
			Keys::Q			    = 'Q'            ,
			Keys::R			    = 'R'            ,
			Keys::S			    = 'S'            ,
			Keys::T			    = 'T'            ,
			Keys::U			    = 'U'            ,
			Keys::V			    = 'V'            ,
			Keys::W			    = 'W'            ,
			Keys::X			    = 'X'            ,
			Keys::Y			    = 'Y'            ,
			Keys::Z			    = 'Z'            ,
			Keys::_0			    = '0'            ,
			Keys::_1			    = '1'            ,
			Keys::_2			    = '2'            ,
			Keys::_3			    = '3'            ,
			Keys::_4			    = '4'            ,
			Keys::_5			    = '5'            ,
			Keys::_6			    = '6'            ,
			Keys::_7			    = '7'            ,
			Keys::_8			    = '8'            ,
			Keys::_9			    = '9'            ,
			Keys::Numlock		= VK_NUMLOCK      ,
			Keys::Scroll		= VK_SCROLL       ,
			Keys::LShift		= VK_LSHIFT       ,
			Keys::RShift		= VK_RSHIFT       ,
			Keys::LCtrl			= VK_LCONTROL     ,
			Keys::RCtrl			= VK_RCONTROL     ,
			Keys::LAlt		    = VK_LMENU        ,
			Keys::RAlt		    = VK_RMENU        ,
			Keys::Dash			= VK_OEM_3			
			;				



		Timer::Timer() {
			QueryPerformanceFrequency(&tm.freq);
			tm.ticks.QuadPart = 0;
		}
		double Timer::Microseconds() {
			static LARGE_INTEGER delta;
			QueryPerformanceCounter(&_systimerdata::cmp);
			delta.QuadPart = _systimerdata::cmp.QuadPart - tm.ticks.QuadPart;
			tm.ticks.QuadPart = _systimerdata::cmp.QuadPart;
			return delta.QuadPart * (1000000.0/tm.freq.QuadPart);
		}

		double Timer::Miliseconds() {
			return Microseconds() * 0.001;
		}
		double Timer::Seconds() { 
			return Microseconds() * 0.000001;
		}
	}
	using namespace Window;
	void Init::Init(const char* name, bool _fullscreen, int bpp, int _width, int _height, int _min_w, int _min_h, int _max_w, int _max_h) {
		height = _height;
		width = _width;
		min_w = _min_w;
		min_h = _min_h;
		max_w = _max_w;
		max_h = _max_h;
		fullscreen = _fullscreen;
		Win32::wc.cbSize = sizeof (WNDCLASSEX);
		Win32::wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;	
		Win32::wc.lpfnWndProc		= (WNDPROC) Win32::Events;					
		Win32::wc.cbClsExtra		= 0;									
		Win32::wc.cbWndExtra		= 0;									
		Win32::wc.hInstance		= Win32::wmain.hInstance;							
		Win32::wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			
		Win32::wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			
		Win32::wc.hbrBackground	= NULL;									
		Win32::wc.lpszMenuName	= NULL;									
		Win32::wc.lpszClassName	= Win32::classname;								
		Win32::wc.hIconSm = LoadIcon (NULL, IDI_APPLICATION);

		if (!RegisterClassEx(&Win32::wc)) { 
			MessageBox (NULL, "Failed to register window class!", "Error", MB_ICONEXCLAMATION | MB_OK);
			exit(0);
		}

		DWORD	exstyle;				
		DWORD	style;	

		if (fullscreen)						
		{
			DEVMODE screen;						
			ZeroMemory(&screen,sizeof(screen));	
			screen.dmSize=sizeof(screen);		
			screen.dmPelsWidth	= width;		
			screen.dmPelsHeight	= height;		
			screen.dmBitsPerPel	= bpp;			
			screen.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
			fullscreen = (ChangeDisplaySettings(&screen,CDS_FULLSCREEN)==DISP_CHANGE_SUCCESSFUL);
		}
		exstyle = (fullscreen ? WS_EX_APPWINDOW : (WS_EX_APPWINDOW | WS_EX_WINDOWEDGE));
		style = (fullscreen ? WS_POPUP : WS_OVERLAPPEDWINDOW);

		RECT wrect;
		wrect.left=(long)0;			
		wrect.right=(long)_width;	
		wrect.top=(long)0;			
		wrect.bottom=(long)_height;	

		AdjustWindowRectEx(&wrect, style, FALSE, exstyle);	

		Win32::hwnd = CreateWindowEx(	exstyle,Win32::classname, name, style |WS_CLIPSIBLINGS|WS_CLIPCHILDREN,0,0,					
			wrect.right-wrect.left,	
			wrect.bottom-wrect.top,	
			NULL,								
			NULL,								
			Win32::wmain.hInstance,			
			NULL);								

		if(!Win32::hwnd) MessageBox (NULL, "CreateWindowEx Failed", name, MB_ICONEXCLAMATION);

		PIXELFORMATDESCRIPTOR pfd;

		Win32::mhDC = GetDC(Win32::hwnd);
		ZeroMemory(&pfd, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = bpp;
		pfd.iLayerType = PFD_MAIN_PLANE;
		SetPixelFormat(Win32::mhDC, ChoosePixelFormat(Win32::mhDC, &pfd), &pfd);

		Win32::mhRC = wglCreateContext(Win32::mhDC);
		wglMakeCurrent(Win32::mhDC, Win32::mhRC);

		ShowWindow (Win32::hwnd, SW_SHOW); 

		SetForegroundWindow(Win32::hwnd);						

		SetFocus(Win32::hwnd);
	}
}