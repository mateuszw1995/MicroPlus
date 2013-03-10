#pragma once

class Overlap
{
public:
	_vector< _GuiComponent* > Drawings;
	Button But;
	bool closeable;
	_string title;
	GLfloat scrollx;
	Overlap(_string newtitle = "Overlap") : title(newtitle), Drawings(_vector<_GuiComponent*>(0))
	{
		newtitle += "       ";
		But = Button(0, 0, 5, 5, newtitle, true);
		closeable = false;
		But.movetext = false;
		
	}
	
	void operator += ( _GuiComponent* newgui)
	{
		Add(newgui);
	}
	void operator () ( _GuiComponent* newgui)
	{
		Add(newgui);
	}
	void Add( _GuiComponent* newgui)
	{
		Drawings(newgui);
	}
	void Disable_Closer()
	{
		closeable = false;
	}
	void Enable_Closer()
	{
		closeable = true;
	}

};

class OverlapSet : public _GuiComponent
{
public:
	
	int viewed;
	Button ScrollerL;
	Button ScrollerR;
	GLfloat scrollx;
	int stoppedon;
	_vector<Overlap*> Overlaps;
	Button uniX;

	OverlapSet (GLfloat _x = 0.0, GLfloat _y = 0.0, GLfloat _w = 0.0, GLfloat _h = 0.0) : _GuiComponent(_x, _y, _w, _h), Overlaps(_vector<Overlap*>(0))
	{
		_Style = &CWindow::_DefaultStyle;
		viewed = 0;
		ScrollerL = Button(0, 0, 5, 5, "<");
		ScrollerR = Button(0, 0, 5, 5, ">");
		uniX = Button(0, 0, 5, 5, "X");
		scrollx = 0;
		stoppedon = 0;
	}
	void operator += ( Overlap* newoverlap)
	{
		Add(newoverlap);
	}
	void operator -= ( Overlap* newoverlap)
	{
		Overlaps.erase(newoverlap);

	}
	void operator () ( Overlap* newoverlap)
	{
		Add(newoverlap);
	}
	void Add( Overlap* newoverlap)
	{
		
		//Overlaps( Button(0, 0, 5, 5, newoverlap, true) );
	
		newoverlap->But.y = h;
	Overlaps.push_back(newoverlap);
	(*Overlaps[0]).But.switchflag = true;

	}
	void Draw();


};
void OverlapSet::Draw()
{	
	//if(Overlaps.size() == 0) return;
	float* params = new float[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, params);

    int Rx = params[12];
	int Ry = params[13];


	glTranslatef(x, y, 0.0);
	
	if(_Style)
		if(_Style->OverlapBackground.imported)
		{
	        glBindTexture(GL_TEXTURE_2D, _Style->OverlapBackground.texture[0]);
		}

	glBegin(GL_QUADS);
	  glTexCoord2f( 0.0f, 1.0f ); glVertex2f( 0, h);
      glTexCoord2f( 1.0f, 1.0f ); glVertex2f(  w, h);
	  glTexCoord2f( 1.0f, 0.0f ); glVertex2f(  w,  h+Overlaps[0]->But.h);
      glTexCoord2f( 0.0f, 0.0f ); glVertex2f( 0,  h+Overlaps[0]->But.h);
	glEnd();	

	if(_Style)
		if(_Style->OverlapContentBackground.imported)
		{
	        glBindTexture(GL_TEXTURE_2D, _Style->OverlapContentBackground.texture[0]);
		}

	glBegin(GL_QUADS);
	  glTexCoord2f( 0.0f, 1.0f ); glVertex2f( 0, 0);
      glTexCoord2f( 1.0f, 1.0f ); glVertex2f(  w, 0);
	  glTexCoord2f( 1.0f, 0.0f ); glVertex2f(  w,  h);
      glTexCoord2f( 0.0f, 0.0f ); glVertex2f( 0,  h);
	glEnd();	

	glColor4f(1.0, 1.0, 1.0, 1.0);
	Disable(GL_TEXTURE_2D);
	
		//glScissor(x,y,w,h);
	//glDisable(GL_SCISSOR_TEST);
	
		          // glScissor(sx,sy,sw+1,sh+Overlaps[0]->But.h);

		glBegin(GL_LINE_STRIP);
	
      glVertex2f(  w, h);
       glVertex2f(  w,  h+Overlaps[0]->But.h);
       glVertex2f( 0,  h+Overlaps[0]->But.h);
	    glVertex2f( 0, h);

	glEnd();	
	
	
	  GLint prevsciss[4] = { 0,0,0,0} ;
	  glGetIntegerv(GL_SCISSOR_BOX, prevsciss);


	ScrollerL.x = 1;
	ScrollerL.y = h;
	ScrollerR.x = ScrollerL.w+2;
	ScrollerR.y = h;
	
		_Rectangle _cPlane = clipProduct(_Rectangle(prevsciss[0], prevsciss[1], prevsciss[2], prevsciss[3]),
			                               _Rectangle(Rx+x, Ry+y+h, w, ScrollerL.h));

		

	ScrollerR.Draw();
	ScrollerL.Draw();



	Overlap** firstb = &Overlaps[0];
	float wsum = ScrollerL.w + ScrollerR.w + 4;
	(*Overlaps[viewed]).But.switchflag = true;
	(*Overlaps[viewed]).But.y = h-2;


	//glScissor(_cPlane.x+ScrollerL.w + ScrollerR.w + 4, _cPlane.y-5, _cPlane.w-1-(ScrollerL.w + ScrollerR.w + 4), _cPlane.h+5);
		Disable(GL_TEXTURE_2D);
		
		glColor4ub(_Style->darkborder.r, _Style->darkborder.g, _Style->darkborder.b, _Style->darkborder.alpha);
		
			glBegin(GL_LINES);
	 
       glVertex2f( 0,  h);
       glVertex2f(  w,  h);
	glEnd();	
		
	for(int i = 0; i < Overlaps.size(); ++i, ++firstb)
	{
		if(i >= stoppedon)
		{
		if(i != viewed)
		{
			(*firstb)->But.y = h;
			if((*firstb)->But.switchflag)
			{
				(*Overlaps[viewed]).But.switchflag = false;
				(*firstb)->But.switchflag = true;
				viewed = i;
			}
		}
	
			    
			//glDisable(GL_SCISSOR_TEST);
			
	//	           glScissor(sx,sy,sw+1,sh+(*firstb)->But.h);
		         //  glScissor(x,y,w,h);
		(*firstb)->But.x = wsum;
	
		char temp = _Style->brightborder.alpha;
		char temp2 = _Style->darkborder.alpha;
		
		int tempt1 = _Style->ButtonBackground.texture[0];
		
		_Style->brightborder.alpha = 0;
	//	_Style->darkborder.alpha = 0;
	if(i == viewed)
	{
		if(_Style->ButtonBackground)
		_Style->ButtonBackground.texture[0] = _Style->OverlapContentBackground.texture[0];
		//(*firstb)->But.h += 2;
		(*firstb)->But.y -= 1;
	}
	
	glScissor(_cPlane.x+ScrollerL.w + ScrollerR.w + 4, _cPlane.y, _cPlane.w-1-(ScrollerL.w + ScrollerR.w + 4), _cPlane.h+1);
		 (*firstb)->But.Draw();
		 if( i == viewed) {// (*firstb)->But.h -= 2;  
		(*firstb)->But.y += 1; }
		 if(i == viewed)
		{
			if((*firstb)->closeable)
			{
			uniX.x = wsum+(*firstb)->But.w-uniX.w-1;
			uniX.y = h+((*firstb)->But.h/2) - (uniX.h/2)-1;
			
			_Style->brightborder.alpha = 0;
			_Style->darkborder.alpha = 0;
			uniX.Draw();
			_Style->brightborder.alpha = 0;
			_Style->darkborder.alpha = 0;
			if(uniX.Hit())
			{
				operator-=(*firstb);
				if(viewed > Overlaps.size()-1 ) viewed =Overlaps.size()-1; 

		if(_Style->ButtonBackground)
		_Style->ButtonBackground.texture[0] = tempt1;		

		_Style->brightborder.alpha = temp;
		_Style->darkborder.alpha = temp2;		break;
			}
			}

	
		}


	

		glScissor(prevsciss[0], prevsciss[1], prevsciss[2], prevsciss[3]);
	if(i == viewed){
	
		if(_Style->ButtonBackground)
		_Style->ButtonBackground.texture[0] = tempt1;


	}
		_Style->brightborder.alpha = temp;
		_Style->darkborder.alpha = temp2;
		
		         //  glScissor(sx,sy,sw,sh);
		//glEnable(GL_SCISSOR_TEST);
		}
		if(i == viewed)
		{
		_GuiComponent** firstelem = (*firstb)->Drawings.get_ptr();
		int sizee = (*firstb)->Drawings.size();
		_Rectangle cPlane = clipProduct(_Rectangle(prevsciss[0], prevsciss[1], prevsciss[2], prevsciss[3]),
			                               _Rectangle(Rx+x, Ry+y, w, h));

		

		for(int j = 0; j < sizee; ++j, ++firstelem)
		{
			glScissor(cPlane.x, cPlane.y, cPlane.w, cPlane.h);
			(*firstelem)->Draw();
			
		         //  glScissor(sx,sy,sw,sh);
		}
		glScissor(prevsciss[0], prevsciss[1], prevsciss[2], prevsciss[3]);
		}
		if(i >= stoppedon)
		wsum += (*firstb)->But.w + 4;
	}
	
	if(ScrollerR.Hit())
	{
		if(stoppedon < Overlaps.size()-1 ) 
		{
	//		scrollx += //(*Overlaps[stoppedon]).But.w + 4;
		//		0.1;
		//	if(scrollx > wsum) scrollx = wsum;
			stoppedon += 1;
		}
	}
	if(ScrollerL.Hit())
	{
		
		if(stoppedon > 0 ) 
		{
		//	scrollx -= //((*Overlaps[stoppedon]).But.w + 4);
		//	0.1;
		//	if(scrollx < 0) scrollx = 0;
			stoppedon -= 1;
		}
	}
	
	glColor4f(1.0, 1.0, 1.0, 1.0);
	Disable(GL_TEXTURE_2D);

		glBegin(GL_LINE_STRIP);
	 
       glVertex2f( 0,  h);
	   glVertex2f( 0, 0);
      glVertex2f(  w, 0);
       glVertex2f(  w,  h);
	glEnd();	


	glTranslatef(-x, -y, 0.0);
		delete  [] params;
}

