#pragma once


bool Hovering(int mx, int my, int x, int y, int w, int h)
{

	return (mx <= x + w && mx >= x &&
		    my <= y + h && my >= y);

	
}

struct _WindowStyleInfo
{
	CTexture Caption;
	CTexture Border;
	CTexture Close;
	CTexture Background;

	/* GUI COMPONENTS STYES */
	
	CTexture ButtonBackground;
	CTexture TextBoxBackground;
	CTexture TickBoxBackground;
	CTexture ContentBoxBackground;
	CTexture DropDownListBackground;
	CTexture OverlapBackground;
	CTexture OverlapContentBackground;
	CTexture ScrollableAreaBackground;
	
	float borderw;
	float downborderh;
	float captionh;
	float closerw;
	float closerh;

	float listiconsize;


	CTextStyle* Font;
	
	RGBA_256 brightborder;
	RGBA_256 darkborder;


	_WindowStyleInfo(
		CTexture a, 
		CTexture b, 
		CTexture c, 
		CTexture d, 
		CTexture bb,
		CTexture tt,
		CTexture ti,
		CTexture cc, 
		CTexture dl, 
		CTexture ob, 
		CTexture oc,  
		CTexture sb
		) : 
	    Caption(a), 
		Border(b), 
		Close(c), 
		Background(d), 
		Font(NULL), 
		ButtonBackground(bb), 
		TextBoxBackground(tt), 
		TickBoxBackground(ti), 
		ContentBoxBackground(cc), 
		DropDownListBackground(dl), 
		OverlapContentBackground(oc), 
		OverlapBackground(ob),
		ScrollableAreaBackground(sb),
		brightborder(RGBA_256(255, 255, 255, 255)),
		darkborder(RGBA_256(255, 255, 255, 255))
	{
		
	}
};

#include "GUI/guicomponent.h"
struct EventRectangle
{
	float x;
	float y;
	float w;
	float h;
	bool badhold;
	bool CheckGrab();
	bool CheckHover();
	bool CheckClick();

	EventRectangle(GLfloat _x = 0.0, GLfloat _y = 0.0, GLfloat _w = 0.0, GLfloat _h = 0.0) : x(_x), y(_y), w(_w), h(_h), badhold(false)
	{}

};
class Button;
class CWindow
{
public: //for debug

	bool visible;
	friend class CScene;
	GLfloat x, y;
	GLfloat w, h;
	bool badhold;
	bool active;
	CText caption;
	Button* xcloser;
	bool closeable;
	bool resizable;
	bool moveable;

    static _WindowStyleInfo _DefaultStyle;
	_WindowStyleInfo* _Style;

	_vector<_GuiComponent*> Contents;

	void operator () (_GuiComponent* newcomp)
	{
		Add(newcomp);
	}
	
	void operator -= ( _GuiComponent* newoverlap)
	{

				Contents.erase(newoverlap);

	}
	void Add (_GuiComponent* newcomp)
	{
		Contents.push_back(newcomp);
		newcomp->y = h-newcomp->y-newcomp->h-1;
	}

	void Draw();
	void PollEvents();
    bool WindowAreaClicked();
	
	void Closeable(bool flag)
	{
		closeable = flag;
	}
	void Resizable(bool flag)
	{
		resizable = flag;
	}
	void Moveable(bool flag)
	{
		moveable = flag;
	}
	~CWindow()
	{
		delete xcloser;
	}

public:

	CWindow(GLfloat _x, GLfloat _y, GLfloat _w, GLfloat _h, _string captext = "Window");

};

#include "GUI/button.h"
CWindow::CWindow(GLfloat _x, GLfloat _y, GLfloat _w, GLfloat _h, _string captext) : x(_x), y(_y), w(_w), h(_h), visible(true), active(true), closeable(false), moveable(true), resizable(true), caption(CText(_DefaultStyle.Font, captext, 0.0, 0.0)), Contents(_vector<_GuiComponent*>())
	{
		if(h < 20.0) h = 20.0;
		if(w < 40.0) w = 40.0;
		_Style = &_DefaultStyle;

		
		x += _Style->borderw;
		y = DB::Window::getHeight()-y-h-_Style->captionh;

		badhold = false;
		xcloser = new Button(0, 0, _Style->closerw, _Style->closerh, "");
	//	Rects(EventRectangle(0, CWindow::_DefaultStyle.captionh, w, CWindow::_DefaultStyle.captionh ));
	//	Rects(EventRectangle());
	//	Rects(EventRectangle());
	//	Rects(EventRectangle());

	}

_WindowStyleInfo CWindow::_DefaultStyle(
	EMPTY_TEXTURE, 
	EMPTY_TEXTURE, 
	EMPTY_TEXTURE, 
	EMPTY_TEXTURE, 
	EMPTY_TEXTURE, 
	EMPTY_TEXTURE, 
	EMPTY_TEXTURE, 
	EMPTY_TEXTURE, 
	EMPTY_TEXTURE, 
	EMPTY_TEXTURE, 
	EMPTY_TEXTURE, 
	EMPTY_TEXTURE
	
	);


/* DEFAULT GUI STYLES */
/* DEFAULT GUI STYLES */
/* DEFAULT GUI STYLES */


void Set_GUI_Style()
{
	CWindow::_DefaultStyle.Caption.         Import_Texture_From_Color(RGBA_256(45, 45, 65, 255));
	CWindow::_DefaultStyle.Border.          Import_Texture_From_Color(RGBA_256(45, 45, 65, 200));
	CWindow::_DefaultStyle.Close.           Import_Texture_From_Color(RGBA_256(255, 0, 0, 255));
	CWindow::_DefaultStyle.Background.      Import_Texture_From_Color(RGBA_256(0, 22, 45, 170));
	CWindow::_DefaultStyle.ButtonBackground.Import_Texture_From_Color(RGBA_256(45, 45, 45, 255));
	CWindow::_DefaultStyle.TextBoxBackground.Import_Texture_From_Color(RGBA_256(45, 45, 45, 255));
	CWindow::_DefaultStyle.TickBoxBackground.Import_Texture_From_Color(RGBA_256(45, 45, 45, 255));
	CWindow::_DefaultStyle.ContentBoxBackground.Import_Texture_From_Color(RGBA_256(45, 45, 45, 255));
	CWindow::_DefaultStyle.DropDownListBackground.Import_Texture_From_Color(RGBA_256(45, 45, 45, 255));
	CWindow::_DefaultStyle.OverlapContentBackground.Import_Texture_From_Color(RGBA_256(0, 11, 24, 255));
	CWindow::_DefaultStyle.OverlapBackground.Import_Texture_From_Color(RGBA_256(0, 11, 24, 255));
	CWindow::_DefaultStyle.ScrollableAreaBackground.Import_Texture_From_Color(RGBA_256(0, 136, 136, 255));
	
	CWindow::_DefaultStyle.borderw = 7;
	CWindow::_DefaultStyle.downborderh = 7;
	CWindow::_DefaultStyle.captionh = 18;
	CWindow::_DefaultStyle.closerw = 10;
	CWindow::_DefaultStyle.closerh = 10;
	CWindow::_DefaultStyle.listiconsize = 20;

	CWindow::_DefaultStyle.brightborder = RGBA_256(192, 192, 192, 255);
	CWindow::_DefaultStyle.darkborder = RGBA_256(64, 64, 64, 255);
	
	//CWindow::_DefaultStyle.brightborder = CWindow::_DefaultStyle.darkborder;

	CWindow::_DefaultStyle.Font = new CTextStyle("arial.ttf", CWindow::_DefaultStyle.captionh/1.5);

}


	GLfloat scrollcontentw = 0;
	GLfloat scrollcontenth = 0;
#include "CWindow.hpp"

#include "GUI/textbox.h"
#include "GUI/tickbox.h"
#include "GUI/list.h"
#include "GUI/slidebar.h"
#include "GUI/overlap.h"
#include "GUI/scrollarea.h"
#include "GUI/textarea.h"

