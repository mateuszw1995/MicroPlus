#pragma once

class SlideBar : public _GuiComponent
{
	friend class ScrollArea;
public: 
	
	CText _text;
	GLfloat value()
	{
		if(orientation == VERTICAL) return maxv-val;
		return val;
	}

	void setMaxVal(GLfloat newmv)
	{
		if(newmv < 1) newmv = 1;
		val *= newmv/maxv;
		maxv = newmv;
	}

	void setVal(GLfloat newval)
	{
		val = newval;
		if(newval < 0) val = 0.0;
		if(newval > maxv) val = maxv;
	}
	
	Button buts[3];
private:
	GLfloat maxv;
	GLfloat val;
	CEnum orientation;


public:
	SlideBar (GLfloat _x = 0.0, GLfloat _y = 0.0, GLfloat _w = 0.0,  GLfloat _h = 0.0, _string towrite = "", CEnum orient = HORIZONTAL,GLint _max = 100) : _GuiComponent(_x, _y, _w, _h), maxv(_max),  orientation(orient), _text(CText(CWindow::_DefaultStyle.Font, towrite, 2.0, _h/2.0  ))
	{
		
		_Style = &CWindow::_DefaultStyle;
	
		//w;
		if(orientation == HORIZONTAL)
		{
			if(w < 20) w = 20;
			buts[0] = Button(-20.0, 0.0, 20.0, h, "");
			buts[1] = Button(0.0, 0.0, w/maxv, h, "");
			buts[2] = Button(w, 0.0, 20.0, h, ""); 
			if(buts[1].w < 10)buts[1].w = 10; 
		}
		else 
		{
			if(h < 10) h = 10;
			buts[0] = Button(0.0, -20.0, w, 20.0, "");
			buts[1] = Button(0.0, 0.0, w, h/maxv, "");
			buts[2] = Button(0.0, h, w, 20.0, ""); 
			if(buts[1].h < 10) buts[1].h = 10;
		}
		val = 0.0;
	}
	void Draw();

};

void SlideBar::Draw()
{
	
	if(maxv <= 1) maxv = 1;
				float* params = new float[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, params);

    int Rx = params[12];
	int Ry = params[13];
	CEnum evresult = EventState(Rx, Ry);
	if(evresult == HIT)
	{
		if(orientation == HORIZONTAL)
		{
		val = (mouse_x-Rx-x - buts[1].w/2)*maxv/(w-buts[1].w);
	
		}
		else
		{
		val = (DB::Window::getHeight()-mouse_y-Ry-y - buts[1].h/2)*maxv/(h-buts[1].h);
		}
			if(val <= 0) val = 0;
		if(val >= maxv) val = maxv;
	}

	
	glTranslatef(x, y, 0);
	
	//glRotatef(90.0, 0.0, 0.0, 1.0);
		if(_Style)
		if(_Style->TextBoxBackground.imported)
		{
	   //     glBindTexture(GL_TEXTURE_2D, _Style->TextBoxBackground.texture[0]);
		}

			
	Enable(GL_TEXTURE_2D);
	Enable(GL_BLEND);
	
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	
	glBegin(GL_QUADS);
	//  glTexCoord2f( 0.0f, 1.0f ); glVertex2f( 0, 0);
    //  glTexCoord2f( 1.0f, 1.0f ); glVertex2f(  w, 0);
     // glTexCoord2f( 1.0f, 0.0f ); glVertex2f(  w,  h);
     // glTexCoord2f( 0.0f, 0.0f ); glVertex2f( 0,  h);
	glEnd();	

	Disable(GL_BLEND);

	Disable(GL_TEXTURE_2D);


	glBegin(GL_LINES);

	glColor4ub(_Style->brightborder.r, _Style->brightborder.g, _Style->brightborder.b, _Style->brightborder.alpha);
	   glVertex2f( 0, 0);
       glVertex2f(  w, 0);


	   glVertex2f(  w, h);
       glVertex2f(  w, 0);
	
	glColor4ub(_Style->darkborder.r, _Style->darkborder.g, _Style->darkborder.b, _Style->darkborder.alpha);
       glVertex2f(  0, h);
       glVertex2f(  0, 0);

	   
       glVertex2f(  w, h);
       glVertex2f(  0, h);

	glEnd();

		if(buts[0].Hold() && !(SDL_GetTicks() % 2) )
	{
		//if(orientation == HORIZONTAL)
	//	{

		//	val -= 0.1;
	/*	buts[1].x -= 2.0/maxv;
			
		if(buts[1].x <= 0) buts[1].x = 0;
		if(buts[1].x >= w-buts[1].w) buts[1].x = w-buts[1].w;*/
		//}
	//	else
	//	{
			val -= 0.1;
			
	/*	buts[1].y -= 2.0/maxv;
			
		if(buts[1].y <= 0) buts[1].y = 0;
		if(buts[1].y >= h-buts[1].h) buts[1].y = h-buts[1].h;*/
	//	}
		
			if(val <= 0) val = 0;
	}	
	if(buts[2].Hold() && !(SDL_GetTicks() % 2) )
	{
		//if(orientation == HORIZONTAL)
		//{
		//	
		//	val += 0.1;
		//val += 0.1;
		/*buts[1].x += 2.0/maxv;
			
		if(buts[1].x <= 0) buts[1].x = 0;
		if(buts[1].x >= w-buts[1].w) buts[1].x = w-buts[1].w;*/
		//}
		//else
		//{
			
			
			val += 0.1;
			if(val >= maxv) val = maxv;
		//val += 0.1;
		/*buts[1].y += 2.0/maxv;
			
		if(buts[1].y <= 0) buts[1].y = 0;
		if(buts[1].y >= h-buts[1].h) buts[1].y = h-buts[1].h;*/
	//	}
	}

	buts[0].Draw();
	buts[1].Draw();
	buts[2].Draw();

	//if(buts[1].Hold())
	//{ 
	//	buts[1].x += mouse_xrel;
	//	if(buts[1].x < 20) buts[1].x = 20;
	//	if(buts[1].x > w+buts[1].w) buts[1].x = w+buts[1].w;
	//}



	//if(Hovering(mouse_x, mouse_y, Rx+20.0, Ry, w-40, h))

	//if(maxv == 0) maxv = 1;
		delete []  params;
		/*if(orientation == HORIZONTAL)
		val = (buts[1].x/(w/maxv));
		else if(orientation == VERTICAL)
		val = (buts[1].y/(h/maxv));

			*/


		if(orientation == HORIZONTAL)
		{
			buts[1].w = w/maxv*50;
			if(buts[1].w < 10) buts[1].w = 10;
			if(buts[1].w > w) buts[1].w = w;
			buts[1].x = ((w - buts[1].w)/maxv)*val;
	//	SetCaption((val));
			
			
		}
		else 
		{
			buts[1].h = h/maxv*50;
			if(buts[1].h < 10) buts[1].h = 10;
			if(buts[1].h > h) buts[1].h = h;
			buts[1].y = ((h - buts[1].h)/maxv)*val;
		//	SetCaption((h/maxv));
		}

		glBegin(GL_TRIANGLES);
		if(orientation == HORIZONTAL)
		{
		glVertex2f(-20.0+(20.0/4.0), (h/2.0) );
		glVertex2f(-20.0+((20.0/4.0)*3.0),(h/4.0) );
		glVertex2f(-20.0+((20.0/4.0)*3.0),(h/4.0)*3.0);
		
		glVertex2f(w+(20.0/4.0)*3.0, (h/2.0) );
		glVertex2f(w+((20.0/4.0)),(h/4.0) );
		glVertex2f(w+((20.0/4.0)),(h/4.0)*3.0);
		}
		else
		{
			
		glVertex2f((w/2.0),-20.0+(20.0/4.0));
		glVertex2f((w/4.0),-20.0+((20.0/4.0)*3.0) );
		glVertex2f((w/4.0)*3.0,-20.0+((20.0/4.0)*3.0));
		
		glVertex2f( (w/2.0), h+(20.0/4.0)*3.0 );
		glVertex2f((w/4.0),h+((20.0/4.0)) );
		glVertex2f((w/4.0)*3.0,h+((20.0/4.0)) );
		}
		glEnd();
		
		_text.x = w+4.0 + buts[2].w;
		_text.y = h/2-_text.getheight()/2;
		_text.Draw();
	glTranslatef(-x, -y, 0);

}
