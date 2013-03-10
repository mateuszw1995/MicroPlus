#pragma once

class ScrollArea : public _GuiComponent
{
public: 
	_vector <_GuiComponent*> Drawings;
	SlideBar SliderV;
	SlideBar SliderH;
	GLfloat scrollx;
	GLfloat scrolly;




	ScrollArea(GLfloat _x = 0.0, GLfloat _y = 0.0, GLfloat _w = 0.0, GLfloat _h = 0.0) : _GuiComponent(_x, _y, _w, _h), Drawings(_vector<_GuiComponent*>(0))
	{
		scrollx = 0;
		scrolly = 0;
		_Style = &CWindow::_DefaultStyle;
		SliderH = SlideBar(20, 0.0, w-60, 20, "", HORIZONTAL,2);
		SliderV = SlideBar(w-20, 40.0, 20, h-60, "", VERTICAL);

	}
	void operator += ( _GuiComponent* newgui)
	{
		Add(newgui);
	}
	void operator -= ( _GuiComponent* newgui)
	{
				Drawings.erase(newgui);

	}
	void operator () ( _GuiComponent* newgui)
	{
		Add(newgui);
	}
	void Add( _GuiComponent* newgui)
	{
	Drawings.push_back(newgui);

	}

	void Draw();

};

void ScrollArea::Draw()
{
	 scrollcontentw = 0;
	 scrollcontenth = 0;
	float* params = new float[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, params);

    int Rx = params[12];
	int Ry = params[13];

	delete [] params;

	Enable(GL_TEXTURE_2D);
	glTranslatef(x, y, 0.0);
	
	if(_Style)
		if(_Style->ScrollableAreaBackground.imported)
		{
	        glBindTexture(GL_TEXTURE_2D, _Style->ScrollableAreaBackground.texture[0]);
		}

	glBegin(GL_QUADS);
	  glTexCoord2f( 0.0f, 1.0f ); glVertex2f( 0, h);
      glTexCoord2f( 1.0f, 1.0f ); glVertex2f(  w, h);
	  glTexCoord2f( 1.0f, 0.0f ); glVertex2f(  w,  h);
      glTexCoord2f( 0.0f, 0.0f ); glVertex2f( 0,  h);
	glEnd();	

	
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
	


	
	  GLint prevsciss[4] = { 0,0,0,0} ;
	  glGetIntegerv(GL_SCISSOR_BOX, prevsciss);

	_Rectangle cPlane = clipProduct(_Rectangle(prevsciss[0], prevsciss[1], prevsciss[2], prevsciss[3]),
			                               _Rectangle(Rx+x, Ry+y+20, w-20, h-20));
	
	glScissor(cPlane.x, cPlane.y, cPlane.w, cPlane.h);

	_GuiComponent** firstg = &Drawings[0];
	const int Size = Drawings.size();



	glPushMatrix();
	//glTranslatef(-scrollx, scrolly, 0);
	for(int i = 0; i < Size; ++i, ++firstg)
	{
		GLfloat tempY = (*firstg)->y;
		GLfloat tempX = (*firstg)->x;
		(*firstg)->y = h - (*firstg)->y - (*firstg)->h + scrolly;
		(*firstg)->x -= scrollx;
		(*firstg)->Draw();
		(*firstg)->y = tempY;
		(*firstg)->x = tempX;
	    if(tempY + (*firstg)->h > scrollcontenth) scrollcontenth = tempY + (*firstg)->h;
	    if((*firstg)->x + (*firstg)->w > scrollcontentw) scrollcontentw = (*firstg)->x + (*firstg)->w ; 
	}
	glPopMatrix();



	glScissor(prevsciss[0], prevsciss[1], prevsciss[2], prevsciss[3]);

	//scrollcontentw+=30;
	//scrollcontenth+=30;
	//
	GLfloat divid = 1;
	{
	SliderH.setMaxVal((scrollcontentw-w+20)/divid);
	
	//SetCaption(SliderH.maxv );
	scrollx = SliderH.value()*divid;

	//SetCaption(scrollcontentw);
	}
	//
	{
	    SliderV.setMaxVal((scrollcontenth-h+20)/divid);
		scrolly = SliderV.value()*divid;
		
	}
	
for(int f = 0; f < ActiveEvents.size(); ++f)
if(ActiveEvents[f].type == SDL_MOUSEBUTTONDOWN)
{

    if(ActiveEvents[f].button.button == SDL_BUTTON_WHEELUP )
    {
		SliderV.val += 12.0;
		if(SliderV.val >  SliderV.maxv) SliderV.val =  SliderV.maxv;
    }

    if(ActiveEvents[f].button.button == SDL_BUTTON_WHEELDOWN )
    {
      
		SliderV.val -= 12.0;
		if(SliderV.val < 0) SliderV.val = 0;
    }

}

	if(scrollcontenth > h-20)
	glColor4f(1.0, 1.0, 1.0, 0.5);
		SliderV.Draw();
	glColor4f(1.0, 1.0, 1.0, 1.0);
		if(scrollcontentw > w-20)
	glColor4f(1.0, 1.0, 1.0, 0.5);

		SliderH.Draw();
		
	glColor4f(1.0, 1.0, 1.0, 1.0);

	glTranslatef(-x, -y, 0.0);
}
