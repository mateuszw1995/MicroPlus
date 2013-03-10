#pragma once
class _GuiComponent
{
public:
	float x,y,h,w;
	bool badhold;
	bool hold;

	unsigned fullhited;
	_WindowStyleInfo* _Style;
	_GuiComponent(GLfloat _x = 0.0, GLfloat _y = 0.0, GLfloat _w = 0.0, GLfloat _h = 0.0) : x(_x), y(_y), w(_w), h(_h), badhold(false)
	{
//		_Style = &CWindow::_DefaultStyle;
		hold = false;
		
		fullhited = 0;
	}
	
	virtual void Draw() = 0;
	CEnum EventState(int, int);


};

CEnum _GuiComponent::EventState(int Rx, int Ry)
{
	int X = x+Rx;
	int Y = DB::Window::getHeight() - y - h - Ry;
	
	int W = w;
	int H = h;
	int MX = mouse_x;
	int MY = mouse_y;
	
	bool clicked = clickstate;
	bool hovstate = false; 
	
	  GLint _sp[4] = { 0,0,0,0} ;
	  glGetIntegerv(GL_SCISSOR_BOX, _sp);
	_Rectangle _rect(_sp[0], DB::Window::getHeight()-_sp[1]-_sp[3], _sp[2], _sp[3]);
	


	
	_Rectangle cPlane = (glIsEnabled(GL_SCISSOR_TEST) ? clipProduct(_rect, _Rectangle(X, Y, W, H)) : _Rectangle(X, Y, W, H) );
	 
	//MBX(string(_Rectangle(X, Y, W, H)).c_str(), "Rectangle");
	//MBX(string(cPlane).c_str(), "Clipping plane");
	//	iMBX(X, W);
	//	iMBX(cPlane.x, cPlane.w);
	//	if(X < 700)SetCaption(cPlane.w);
//	iMBX(Y, DB::Window::getHeight()-cPlane.y);
		hovstate = Hovering(MX, MY, cPlane.x, cPlane.y, cPlane.w,cPlane.h);

	
	if(clicked)
	{
		//if(!badhold)
		//EventsOnlyForGui = true;
		if(!hovstate)
		{
		  //  EventsOnlyForGui = false;
			if(!hold && !badhold)
			return MISSED_HIT;
			badhold = true;
			
		}

	}   
	else { badhold = false;
   EventsOnlyForGui = false;
      }
	
	if(clicked && hovstate && !badhold) {  EventsOnlyForGui = true;  hold = true; return HIT; } else if (clicked) { EventsOnlyForGui = true;  hold = true;  return MISSED_HOLD; }
	 if(hold) { hold = false; if(hovstate) { if(SDL_GetTicks() - fullhited < 300) return DOUBLE_HIT;  fullhited = SDL_GetTicks(); return FULLHIT;} else return MISSED_FULLHIT; } 
	if(hovstate) {  return HOVER; }

	return NO_EVENT;

}

