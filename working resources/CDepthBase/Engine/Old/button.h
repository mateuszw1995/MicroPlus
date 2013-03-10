#pragma once
class Button : public _GuiComponent
{
public:
	CText _text;

	bool holding;
		bool hited;
		bool switchtype;
		bool switchflag;
		bool movetext;
	Button (GLfloat _x = 0.0, GLfloat _y = 0.0, GLfloat _w = 0.0, GLfloat _h = 0.0, _string towrite = "Button", bool _switch = false) : _GuiComponent(_x, _y, _w, _h), _text(CText(CWindow::_DefaultStyle.Font, towrite, 2.0, _h/2.0  ))
	, switchtype(_switch){
		_Style = &CWindow::_DefaultStyle;
		hited = false;
		hold = false;
		holding = false;
		switchflag = false;
		movetext = true;
	}
	void Draw();


	bool Hit()
	{
		if(hited) 
		{
		hited = false;
		return true;
		}
		else return false;
	}
	bool Hold()
	{
		return holding;
	}
	bool SwitchFlag()
	{
		if(switchtype)
		{
			return switchflag;
		}
		else return false;
	}
};

void Button::Draw()
{
	bool hovereff = false;
	bool clickeff = false;

	float* params = new float[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, params);

    int Rx = params[12];
	int Ry = params[13];
	delete  [] params;

	
		if(_text.getwidth() > w) w = _text.getwidth() + 12;
		if(_text.getheight() > h) h = _text.getheight() + 4;
	//if(poll)
	{
		
	CEnum evresult = EventState(Rx, Ry);

	if(evresult != NO_EVENT)
	{
		if(evresult == HOVER)
		{
			hovereff = true; 
			clickeff = false;
		}
		else if(evresult == HIT  )
		{
			clickeff = true;
			hovereff = false;
		}
		else if(evresult == FULLHIT || evresult == DOUBLE_HIT)
		{
			hited = true;
			
			switchflag = !switchflag;
		}
	}
	//else EventsOnlyForGui = false;

	holding = clickeff;
    }

     if(_Style)
		if(_Style->ButtonBackground.imported)
		{
	        glBindTexture(GL_TEXTURE_2D, _Style->ButtonBackground.texture[0]);
		}
		if(switchtype && switchflag )
		{
			clickeff = true;
		}

		
		
	Enable(GL_TEXTURE_2D);
	Enable(GL_BLEND);
	
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	glTranslatef(x, y, 0.0);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
	  glTexCoord2f( 0.0f, 1.0f ); glVertex2f( 0, 0);
      glTexCoord2f( 1.0f, 1.0f ); glVertex2f(  w, 0);
      glTexCoord2f( 1.0f, 0.0f ); glVertex2f(  w,  h);
      glTexCoord2f( 0.0f, 0.0f ); glVertex2f( 0,  h);
	glEnd();	

	if(hovereff)
		glColor4ub(227, 180, 34, 150);
		if(clickeff )
		glColor4f(0.1, 0.1, 0.1, 1.0);
		if(!hovereff && !clickeff)
		{
		glColor4f(1.0, 1.0, 1.0, 0.0);
		
		}
	
	Disable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
	   glVertex2f( 0, 0);
       glVertex2f(  w, 0);
       glVertex2f(  w,  h);
       glVertex2f( 0,  h);
	glEnd();	
	//Disable(GL_BLEND);

	if(!clickeff)
	glColor4f(1.0, 1.0, 1.0, 1.0);

	glBegin(GL_LINES);
	if(!clickeff)
	glColor4ub(_Style->darkborder.r, _Style->darkborder.g, _Style->darkborder.b, _Style->darkborder.alpha);
	else
	glColor4ub(_Style->brightborder.r, _Style->brightborder.g, _Style->brightborder.b, _Style->brightborder.alpha);
	   glVertex2f( 0, 0);
       glVertex2f(  w, 0);


	   glVertex2f(  w, h);
       glVertex2f(  w, 0);

	   
	
	if(clickeff)
	glColor4ub(_Style->darkborder.r, _Style->darkborder.g, _Style->darkborder.b, _Style->darkborder.alpha);
	else
	glColor4ub(_Style->brightborder.r, _Style->brightborder.g, _Style->brightborder.b, _Style->brightborder.alpha);
       glVertex2f(  0, h);
       glVertex2f(  0, 0);

	   
       glVertex2f(  w, h);
       glVertex2f(  0, h);

	glEnd();

	Enable(GL_TEXTURE_2D);

	_text.x = 2+(w - _text.getwidth())/2 ;
	_text.y = (h - _text.getheight())/2;
	if(movetext && clickeff){ _text.x += 2;
	                    _text.y -= 2; }
	_text.Draw();

	glTranslatef(-x, -y, 0.0);
}
