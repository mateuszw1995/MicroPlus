#pragma once

class TickBox : public _GuiComponent
{
public:
	bool flag;
	CText sidetext;

	bool Flag()
	{
		return flag;
	}
	TickBox (GLfloat _x = 0.0, GLfloat _y = 0.0, GLfloat _w = 0.0, GLfloat _h = 0.0, _string towrite = "Tickbox") : _GuiComponent(_x, _y, _w, _h), sidetext(CText(CWindow::_DefaultStyle.Font, towrite, 2.0, _h/2.0  ))
	{
		_Style = &CWindow::_DefaultStyle;
		flag = false;
	}
	void Draw();

};


void TickBox::Draw()
{
	bool hovereff = false;
	bool clickeff = false;
		float* params = new float[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, params);

    int Rx = params[12];
	int Ry = params[13];
	delete []  params;
	//	if(sidetext.getwidth() > w*2+0.5*w) w = (sidetext.getwidth() + 4)/2-0.5*w;
	//	if(sidetext.getheight() > h*2+0.5*h) h = (sidetext.getheight() + 2)/2-0.5*h;
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
		else if(evresult == HIT  || evresult == DOUBLE_HIT)
		{
			clickeff = true;
			hovereff = false;
		}
		 if(evresult == FULLHIT || evresult == DOUBLE_HIT)
		{
			flag = !flag;
		}

	}
	//else EventsOnlyForGui = false;

    }

     if(_Style)
		if(_Style->TickBoxBackground.imported)
		{
	        glBindTexture(GL_TEXTURE_2D, _Style->TickBoxBackground.texture[0]);
		}


		if(hovereff)
		glColor4f(1.0, 1.0, 1.0, 1.0);
		if(clickeff)
		glColor4f(0.7, 0.7, 0.7, 0.7);
		if(!hovereff && !clickeff)
		{
		glColor4f(1.0, 1.0, 1.0, 0.6);
		
		}
		
	Enable(GL_TEXTURE_2D);
	Enable(GL_BLEND);
	
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	glTranslatef(x, y, 0.0);
	
	glBegin(GL_QUADS);
	  glTexCoord2f( 0.0f, 1.0f ); glVertex2f( 0, 0);
      glTexCoord2f( 1.0f, 1.0f ); glVertex2f(  w, 0);
      glTexCoord2f( 1.0f, 0.0f ); glVertex2f(  w,  h);
      glTexCoord2f( 0.0f, 0.0f ); glVertex2f( 0,  h);
	glEnd();	



	Disable(GL_TEXTURE_2D);

	
	if(flag)
	{
		glColor4ub(105, 206, 55, 255);
	glBegin(GL_QUADS);
	   glVertex2f( 0, 0);
       glVertex2f(  w, 0);
       glVertex2f(  w,  h);
       glVertex2f( 0,  h);
	glEnd();

	}
	Disable(GL_BLEND);

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

	glTranslatef(0.5*-w, 0.5*-h, 0.0); 
/*
	w+=sidetext.getwidth()/2;
	glBegin(GL_LINES);
	   glVertex2f( 0, 0);
       glVertex2f(  w*2, 0);


	   glVertex2f(  w*2, h*2);
       glVertex2f(  w*2, 0);


	   glVertex2f(  0, h*2);
       glVertex2f(  0, 0);

       glVertex2f(  w*2, h*2);
       glVertex2f(  0, h*2);

	glEnd();
	
	w-=sidetext.getwidth()/2;
	*/
	glTranslatef(0.5*w, 0.5*h, 0.0); 
	
	Enable(GL_TEXTURE_2D);

	sidetext.x = w+4.0;
	sidetext.y = h/2-sidetext.getheight()/2;
	sidetext.Draw();

	glTranslatef(-x, -y, 0.0);
}


