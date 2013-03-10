#pragma once
#include "../../Container/list.h"
#include "../../Container/vector.h"
#include "../../Window/GLWindow.h"

namespace GUI {
	struct Point	{ int	x,y;		 };
	struct Rect		{ int	l,t,r,b;	 };
	struct TexCoord { float u,v;		 };
	struct RGBA_1	{ float r,g,b,alpha; };
	
	struct DrawCall {
		Point ver[4];
		TexCoord tex[4];
		RGBA_1 col;
	};

	struct GUIelement {
		Rect clip_area, area;
		TexCoord tex[2];
		RGBA_1 color;

		GUIelement();
	};

	struct Parent {
		GUIelement* elem;
		_list<Parent> children;
		Parent(GUIelement* = 0);
		void Update();
	};	

	enum {
		Hit = 1,
		Menu,
		ReleaseIn,
		ReleaseOut,
		DoubleHit,
		Drag
	};

	struct EventRect {
		EventRect(int x, int y, int w, int h);
		int x,y,w,h;
		bool down;

		bool hover(), hold();
		int poll();
	};

	typedef _list<Parent>::Node* HGUI;

	struct Window {
		struct TexInfo {
			float p_w,p_h;
			int texture,w,h;
			TexInfo(int=0, int=0, int=0);

		} texinfo;

		int x,y,w,h;
		
		bool update;

		HGUI AddParent(GUIelement*),
	         AddChild(HGUI, GUIelement*);
		void RemoveParent(HGUI),
		     RemoveChild(HGUI, HGUI);

		_list<Parent> Hierarchy;

		void HierarchyOP(HGUI, void (Window::*op)(_list<HGUI>::Node*)),
		     Draw();

		Window();
	};

	extern _list<Window*> wnds;

	void Draw();

	extern Window* active_window;
	Window*& ActiveWindow();

	class Button {
		EventRect evnt;
	public:
		Button(int x, int y, int w, int h);
		int Poll();
		void Draw();
	};

	class TextBox {
		EventRect evnt;
	public:
		TextBox(int x, int y, int w, int h);
		int Poll();
		void Draw();
	};

	class TextArea {


	};

	class ContentBox {

	};


};
void RectProduct(const GUI::Rect&, const GUI::Rect&, GUI::Rect&);
