#pragma once
	static int temp_contentw;
	static int temp_contenth;
	static int temp_wadder;

struct ListNode
{
	CText _Text;
	CTexture* Icon;
	_vector<ListNode*>* Subordinates;
	bool advanced;
	bool selected;
	bool double_hited;
	bool hit;
	bool DoubleClick()
	{
		if(double_hited) {
			double_hited = false;

			return true;
		}
		else return false;

	}
    bool Selected()
	{
		return selected;
	}
	bool Hit()
	{
		if(hit) {
			hit = false;

			return true;
		}
		else return false;
	}


	ListNode& operator += (ListNode* newnode)
	{
		if(!Subordinates)
		{
			Subordinates = new _vector<ListNode*>(0);
		}

		Subordinates->push_back(newnode);
		return *this;
	}

	ListNode(_string Title = "Node", CTexture* Ic = NULL) : Subordinates(NULL), Icon(Ic), _Text(CText(CWindow::_DefaultStyle.Font, Title, 0, 0, false )), advanced(false)
	{
		selected = false;
		double_hited = false;
		hit = false;

	}


	void Draw(CEnum out_evState)
	{
	

	//	float* params3 = new float[16];
	//(GL_MODELVIEW_MATRIX, params3);


		float H = _Text.getheight();
		float W = _Text.getwidth();
		float icsize = CWindow::_DefaultStyle.listiconsize;
		
			if(icsize > H) 
				H = icsize;
		//float W = _Text.getheight();
		glTranslatef(0.0, -H-4.0, 0.0);

		

		

		glColor4f(1.0, 1.0, 1.0, 1.0);

		//glTranslatef(-3.0, 0.0, 0.0);

		
			Disable(GL_TEXTURE_2D);


		glBegin(GL_LINES);
				//glVertex2f(-icsize/2.0, icsize/2.0);
				//glVertex2f(-icsize/2.0, -H);
			//for(int x = -3; x < icsize+2.0; x+=2)
				//glVertex2f(-icsize/2.0, -H);
				//glVertex2f(icsize/2.0, -H);
		
		glVertex2f(-icsize/2.0, H/2);
		glVertex2f(-icsize/2.0, H);
		glVertex2f(-icsize/2.0, H/2);
		glVertex2f(0, H/2);
		
		glEnd();

		if(Subordinates)
		{
		if(out_evState == FULLHIT || out_evState == DOUBLE_HIT)
		{
	float* params = new float[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, params);

    int Rx = params[12];
	int Ry = params[13];
	delete []  params;
	if(Hovering(mouse_x, mouse_y, Rx-icsize/4.0-icsize/2.0, DB::Window::getHeight()-Ry-(H/2+icsize/4.0), icsize/2.0, icsize/2.0 ))
		advanced = !advanced;


		}
			glColor4f(0.0, 0.0, 0.0, 1.0);
			glBegin(GL_QUADS);
			
		glVertex2f(-icsize/4.0, H/2-icsize/4.0);
		glVertex2f(-icsize/4.0, H/2+icsize/4.0);
		
		glVertex2f(-icsize/4.0-icsize/2.0, H/2+icsize/4.0);
		glVertex2f(-icsize/4.0-icsize/2.0, H/2-icsize/4.0);

			glEnd();

			
			glColor4f(1.0, 1.0, 1.0, 1.0);
			glBegin(GL_LINE_LOOP);
			
		glVertex2f(-icsize/4.0, H/2-icsize/4.0);
		glVertex2f(-icsize/4.0, H/2+icsize/4.0);
		
		glVertex2f(-icsize/4.0-icsize/2.0, H/2+icsize/4.0);
		glVertex2f(-icsize/4.0-icsize/2.0, H/2-icsize/4.0);
		 glEnd();

		 glBegin(GL_LINES);
		 
			 
		glVertex2f(-icsize/2.0-icsize/8.0, H/2);
		glVertex2f(-icsize/2.0+icsize/8.0, H/2);



			


		   if(!advanced)
		   {
			   
		glVertex2f(-icsize/2.0, H/2-icsize/8);
		glVertex2f(-icsize/2.0, H/2+icsize/8);
		   }

		   glEnd();
		}

		
		glColor4f(1.0, 1.0, 1.0, 1.0);
		if(Icon)
		{
			Enable(GL_TEXTURE_2D);
			
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
		
			glBindTexture(GL_TEXTURE_2D, Icon->texture[0]);
					Enable(GL_BLEND);

			glTranslatef(2.0, 0.0, 0.0);
			glBegin(GL_QUADS);
			  glTexCoord2f( 0.0f, 1.0f ); glVertex2f( 0, 0);
			  glTexCoord2f( 1.0f, 1.0f ); glVertex2f(  icsize, 0);
		      glTexCoord2f( 1.0f, 0.0f ); glVertex2f(  icsize,  icsize);
		      glTexCoord2f( 0.0f, 0.0f ); glVertex2f( 0,  icsize);
			glEnd();

			
		}
		glTranslatef(icsize+2.0, 0.0, 0.0);



		_Text.y = H/2-_Text.getheight()/2;  

		if(out_evState == HIT)
		{
			
	float* params = new float[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, params);

    int Rx = params[12];
	int Ry = params[13];
	delete []  params;
	if(Hovering(mouse_x, mouse_y, Rx-_Text.x-icsize, DB::Window::getHeight()-Ry-(_Text.y)-H, _Text.w+icsize, H ))
	{hit = true;
		selected = true;
	}
	else selected = false;

		} 
		else if(out_evState == DOUBLE_HIT)
		{
				float* params = new float[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, params);

    int Rx = params[12];
	int Ry = params[13];
	delete [] params;
			if(Hovering(mouse_x, mouse_y, Rx-_Text.x-icsize, DB::Window::getHeight()-Ry-(_Text.y)-H, _Text.w+icsize, H ))
			{
			double_hited = true;
			if(Subordinates) advanced = !advanced;
			}
		}

		if(selected)
		{
			Disable(GL_TEXTURE_2D);
			glColor4f(1.0, 1.0, 1.0, 0.3);

				glBegin(GL_QUADS);
	   glVertex2f( _Text.x, _Text.y);
       glVertex2f(  _Text.x+_Text.w, _Text.y);
       glVertex2f(  _Text.x+_Text.w,  _Text.y+_Text.h);
       glVertex2f( _Text.x,  _Text.y+_Text.h);
	glEnd();	
			
		
		
		}
		_Text.Draw();

	float* params = new float[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, params);

    int Rx = params[12];
	int Ry = params[13];

	temp_contenth = Ry;
	if(temp_contentw < Rx+_Text.getwidth()) temp_contentw = Rx+_Text.getwidth();
	//iMBX(W, Rx);
	delete []params;

		if(Icon)
		{
			glTranslatef(-(icsize+4), 0.0, 0.0);
		}
		
	    if(Subordinates && advanced)
		{
			ListNode** firstn = Subordinates->get_ptr();
			int nsize = Subordinates->size();
			glTranslatef(icsize, 0.0, 0.0);
				for(int i = 0; i < nsize; ++i)
				{
					 
					(*(firstn + i))->Draw(out_evState);


				}
			glTranslatef(-icsize, 0.0, 0.0);
		}


		


	}
};


class DropDownList : public _GuiComponent
{
	_vector<ListNode> List;
	int scrollx;
	int scrolly;



public:

	
	DropDownList& operator += (const ListNode& newnode)
	{

		List.push_back(newnode);
		return *this;
	}

	DropDownList (GLfloat _x = 0.0, GLfloat _y = 0.0, GLfloat _w = 0.0, GLfloat _h = 0.0) : _GuiComponent(_x, _y, _w, _h), List(_vector<ListNode>(0)),
		scrollx(0), scrolly(0)
	{
		_Style = &CWindow::_DefaultStyle;
	}
	void Draw();



};

void DropDownList::Draw()
{
	temp_contentw = 0;
	temp_contenth = 0;
	temp_wadder = 0;
		float* params = new float[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, params);

    int Rx = params[12];
	int Ry = params[13];
	
	delete  [] params;
	CEnum evresult = EventState(Rx, Ry);
	if(evresult != NO_EVENT)
	{
	}

	//if(_Style)
	//	if(_Style->DropDownListBackground.imported)
		//{
	      //  glBindTexture(GL_TEXTURE_2D, _Style->DropDownListBackground.texture[0]);
	//	}

			
	Enable(GL_TEXTURE_2D);
	Enable(GL_BLEND);
	
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	glTranslatef(x, y, 0.0);

//	iMBX(params[12], params[13]);
	
/*	glBegin(GL_QUADS);
	  glTexCoord2f( 0.0f, 1.0f ); glVertex2f( 0, 0);
      glTexCoord2f( 1.0f, 1.0f ); glVertex2f(  w, 0);
      glTexCoord2f( 1.0f, 0.0f ); glVertex2f(  w,  h);
      glTexCoord2f( 0.0f, 0.0f ); glVertex2f( 0,  h);
	glEnd();	
	*/
/*	Disable(GL_BLEND);

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

	glEnd();*/


	glPushMatrix();

	ListNode* firstn = List.get_ptr(); 
	int nsize = List.size();

	glTranslatef(CWindow::_DefaultStyle.listiconsize, h, 0.0);

	// draw pass

	
			  GLint prevsciss[4] = { 0,0,0,0} ;
			  glGetIntegerv(GL_SCISSOR_BOX, prevsciss);

			  int sx =x +Rx, sy = y +Ry, sw = w, sh = h;
		
		if(x+w > prevsciss[2]) sw-= ( x+w - prevsciss[2]);
		if(y+h > prevsciss[3]) sh-= ( y+h - prevsciss[3]);
		if(x<prevsciss[0]) sx = prevsciss[0];
		if(y<prevsciss[1]) sy = prevsciss[1];

	//	           glScissor(sx,sy,sw,sh);
				   
	for(int i = 0; i < nsize; ++i, ++firstn)
	{
		//Enable(GL_BLEND);
		(firstn)->Draw(evresult);


	}
	//temp_contentw -= (Rx+x);
	

	temp_contentw -= (Rx+x);
	temp_contenth -= (Ry+y+h);
	temp_contenth *= -1;

	//w = temp_contentw;
	//h = temp_contenth;


//	iMBX(temp_contentw, temp_contenth);




	// event pass
	/*if(evresult == HIT )
	{
		firstn = List.get_ptr();
	for(int i = 0; i < nsize; ++i, ++firstn)
	  {
		//Enable(GL_BLEND);
		(firstn)->PollEvents();

	  }
	}*/



	glPopMatrix();
	
	glTranslatef(-x, -y, 0.0);

	/*glPushMatrix();
	glLoadIdentity();
	glDisable(GL_SCISSOR_TEST);
	glBegin(GL_LINES);
	
		glVertex2f((Rx+x), 0);
		glVertex2f((Rx+x), DB::Window::getHeight());
		glVertex2f(temp_contentw, 0);
		glVertex2f(temp_contentw, DB::Window::getHeight());

		
		                          glVertex2f(0, Ry+y+h);
		glVertex2f(DB::Window::getWidth(), Ry+y+h);
		                          glVertex2f(0, temp_contenth);
		glVertex2f(DB::Window::getWidth(), temp_contenth);

	glEnd();

	glPopMatrix();*/
	
}
