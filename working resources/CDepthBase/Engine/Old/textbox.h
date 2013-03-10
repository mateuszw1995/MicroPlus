#pragma once

class TextBox :  public _GuiComponent
{
public:
	
	bool selected;
	bool keyhold;
	int lastPress;
	int curspos;
	int scrollx;
	_string CutList;
	bool CutListing;
	unsigned timerr;
	unsigned maxchars;
	int selpos;
	bool holded;

	void SetCutList(_string cutlist)
	{
		CutList = cutlist;
		CutListing = true;
	}

	void DisableCutList()
	{
	CutListing = false;
	}

	CText content;
	TextBox (GLfloat _x = 0.0, GLfloat _y = 0.0, GLfloat _w = 0.0, GLfloat _h = 0.0, _string initcontent = "Text", unsigned _maxchars = 255) : _GuiComponent(_x, _y, _w, _h), content(CText(CWindow::_DefaultStyle.Font, initcontent, 2.0, _h/2.0, false ))
	{
		maxchars = _maxchars;
		_Style = &CWindow::_DefaultStyle;
		selected = false;
		keyhold = false;
		lastPress = 0;
		curspos = initcontent.length();
		CutListing = false;
		timerr = 0;
	scrollx = 0;
	selpos = curspos;
	holded = false;
	}
	void Draw();

	void Set(_string newstr)
	{
		if(newstr.length() <= maxchars)
		{
		content.Str() = newstr;
		curspos = content.Str().length();
		}
		else 
		{
		content.Str() = newstr.substr(0, maxchars);
		curspos = content.Str().length();
		}
	}
	_string& Str()
	{
		return content.Str(); 
	}
};


void TextBox::Draw()
{	
	
	float* params = new float[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, params);

    int Rx = params[12];
	int Ry = params[13];
	delete  [] params;
	CEnum evresult = EventState(Rx, Ry);
	if(evresult != NO_EVENT)
	{
		if(evresult == FULLHIT || evresult == DOUBLE_HIT)
		{
			selected = true;
		holded = false;
	//		GLuint (*fptr)(string);
//			fptr = &(content.tstyle->sizetext);
		
		}
		if(evresult == MISSED_HIT )
		{
			//if(!holded)
			selected = false;
			holded = false;
			//holded = false;
			
		}
	//else 
	}
	else holded = false;
	
	if(content.getheight() > h) h = content.getheight() + 4;
	if(_Style)
		if(_Style->TextBoxBackground.imported)
		{
	        glBindTexture(GL_TEXTURE_2D, _Style->TextBoxBackground.texture[0]);
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

	Disable(GL_BLEND);

	Disable(GL_TEXTURE_2D);

	
	content.x = 2 ;
	content.y = (h - content.getheight())/2;

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
	
	
	int Wt=0, Ht=0, sWt=0;
      //        if(content.Str().length() > 0) TTF_SizeUTF8(content.tstyle->our_font, content.Str().substr(0, curspos).c_str(), &Wt, &Ht);
        //      else
          //    {
            //         TTF_SizeUTF8(content.tstyle->our_font,  "OMG", NULL, &Ht);
              //       Wt = 0;
               // }

			  Ht = content.tstyle->size;
			  Wt = content.tstyle->sizetext(content.Str().substr(0, curspos));
			//  if(curspos == 0) Wt = 24;
			  if(curspos != selpos)
			  sWt = ( selpos < curspos ? content.tstyle->sizetext(content.Str().substr(selpos, curspos-selpos)) : 
				  content.tstyle->sizetext(content.Str().substr(curspos, selpos-curspos))
				  )
				  ;
			//  iMBX(Wt, Wt);
			  GLint prevsciss[4] = { 0,0,0,0} ;
			  glGetIntegerv(GL_SCISSOR_BOX, prevsciss);

             //   Enable(GL_SCISSOR_TEST);
				if(Wt+2-scrollx > w)
					scrollx += w / 3.0;
				else if(Wt+2-scrollx < 0)
					scrollx -= w / 3.0;

				if(scrollx < 0 ) scrollx = 0;


				glTranslatef(-scrollx, 0.0, 0.0);
				content.x += -scrollx;
				
	   if(selected && SDL_GetTicks()/500 % 2)
	   {
				glBegin(GL_LINES);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	
		   
       glVertex2f(  Wt+2, h/2-Ht/2);
       glVertex2f(  Wt+2, h/2+Ht/2);
	   

	glEnd();
	   }

	glTranslatef(scrollx, 0.0, 0.0);
	Enable(GL_TEXTURE_2D);
	if(selected)
	{
		EventsOnlyForGui = true;
		for(int f = 0; f < ActiveEvents.size(); ++f)
		{
		          if(ActiveEvents[f].type == SDL_KEYDOWN)
                    {
						lastPress = ActiveEvents[f].key.keysym.sym;
						timerr = SDL_GetTicks();
						//iMBX(1,1);
						keyhold = true;
						if(ActiveEvents[f].key.keysym.sym == SDLK_BACKSPACE)
						{
							lastPress = SDLK_BACKSPACE;
							if(selpos == curspos)
							{if(curspos > 0 && content.Str().length() > 0 ) {
								content.Str().erase(curspos-1, 1);
								--curspos; }
							}
							else
							{
								if(content.Str().length() > 0 ) {
								if(selpos < curspos) 
								{
									content.Str().erase(selpos, curspos-selpos);
									curspos = selpos;
								}
								else
								{
									content.Str().erase(curspos, selpos-curspos);
								}
								 }
							}
							
						selpos = curspos;
						}
						else if(ActiveEvents[f].key.keysym.sym == SDLK_RIGHT)      
						{
							
							lastPress = SDLK_RIGHT+800;
						if(curspos < content.Str().length())
                        {
                            ++curspos;
                        }
						if(!key_states[SDLK_LSHIFT] && !key_states[SDLK_RSHIFT] )
						selpos = curspos;
						}
						else if(ActiveEvents[f].key.keysym.sym == SDLK_LEFT)               
						{
							lastPress = SDLK_LEFT+800;
						if(curspos > 0)
                        {
                            --curspos;
                        }
						
						if(!key_states[SDLK_LSHIFT] && !key_states[SDLK_RSHIFT] )
						selpos = curspos;
						}
						else if(ActiveEvents[f].key.keysym.sym == SDLK_HOME)               
						{
							curspos = 0;
						if(!key_states[SDLK_LSHIFT] && !key_states[SDLK_RSHIFT] )
						selpos = curspos;
						}
						else if(ActiveEvents[f].key.keysym.sym == SDLK_END)               
						{
							curspos = content.Str().length();
						if(!key_states[SDLK_LSHIFT] && !key_states[SDLK_RSHIFT] )
						selpos = curspos;
						}
						else if(ActiveEvents[f].key.keysym.sym == SDLK_DELETE)
						{
							lastPress = SDLK_DELETE;
							if(selpos == curspos)
							{if(content.Str().length() > 0 && curspos < content.Str().length() ) {
								content.Str().erase(curspos, 1);
								 }
							}
							else
							{
								if(content.Str().length() > 0 ) {
								if(selpos < curspos) 
								{
									content.Str().erase(selpos, curspos-selpos);
									curspos = selpos;
								}
								else
								{
									content.Str().erase(curspos, selpos-curspos);
								}
								 }
							}
							
						selpos = curspos;
						}

						//ActiveEvents[f].key.keysym.unicode = 'a';
                        else if( ( ((ActiveEvents[f].key.keysym.unicode >= (Uint16)' ') && (ActiveEvents[f].key.keysym.unicode <= (Uint16)'~')) ) )
                        {
								if(content.Str().length() > 0  ) {
								if(selpos < curspos) 
								{
									content.Str().erase(selpos, curspos-selpos);
									curspos = selpos;
								}
								else
								{
									content.Str().erase(curspos, selpos-curspos);
								}
								 }

							lastPress = ActiveEvents[f].key.keysym.unicode;
                            _string aa;
                            aa.clear();
                            aa += (char)ActiveEvents[f].key.keysym.unicode;
                            bool adding = true;

                            if(CutListing)
                            if(CutList.find_first_of(ActiveEvents[f].key.keysym.unicode) == string::npos)
                            {
                                adding = false;
                            }

                            if(adding && content.Str().length() < maxchars )
                         {
                                if(content.Str().length() <= 0)
                                {
                                    content.Str() = aa;
									curspos = 1;
                                }
                                else
                                {
                                content.Str().insert(curspos, aa);
                            curspos++;
                                 }
							}
						selpos = curspos;
                           }
	


                       // }
						if(key_states[SDLK_v] && key_states[SDLK_LCTRL])
						{
							
								if(content.Str().length() > 0  ) {
								if(selpos < curspos) 
								{
									content.Str().erase(selpos, curspos-selpos);
									curspos = selpos;
								}
								else
								{
									content.Str().erase(curspos, selpos-curspos);
								}
								 }

							lastPress = SDLK_LCTRL + 800;
                                HANDLE clip = NULL;
                                if(OpenClipboard(NULL))
                                {
                                    clip = GetClipboardData(CF_TEXT);
                                     CloseClipboard();
                                    if(clip)
                                    {

                                 _string copied = (char*)clip;

                                 if(CutListing)
                                 while(true)
                                 {

                                        size_t poss = copied.find_first_not_of(CutList);
                                        if(poss != string::npos)
                                        {
                                        copied.erase(poss, 1);
                                        }
                                        else break;
                                    }

                                    _string temp = content.Str();

                                    temp += copied;

                                    if(temp.length() > maxchars)
                                    {
                                       copied =  copied.substr(0, copied.length()-((temp.length()-maxchars)) ) ;
                                    }

									//MBX(copied.c_str(), "AsdHSDGasgdahsd");
                                    content.Str().insert(curspos, copied);
                                    curspos += copied.length();


                                      }

                                    }
                                    else  CloseClipboard();

									
						selpos = curspos;
						}
						if(key_states[SDLK_RIGHT] && key_states[SDLK_LCTRL])
						{
							lastPress = SDLK_LCTRL + 1000;
							curspos = content.Str().find_first_of(" ", curspos);
							if(curspos == -1) curspos = content.Str().length();
							
						
						if(!key_states[SDLK_LSHIFT] && !key_states[SDLK_RSHIFT] )selpos = curspos;
						}
						if(key_states[SDLK_LEFT] && key_states[SDLK_LCTRL])
						{
							lastPress = SDLK_LCTRL + 1100;
							curspos = content.Str().find_last_of(" ", curspos);
							if(curspos == -1) curspos = 0;
							
						
						if(!key_states[SDLK_LSHIFT] && !key_states[SDLK_RSHIFT] )selpos = curspos;
						}
						

						
						if(key_states[SDLK_a] && key_states[SDLK_LCTRL])
						{
							lastPress = SDLK_LCTRL + 900;
							selpos = 0;
							curspos = content.Str().length();
						}
		
                    }
				  else if(ActiveEvents[f].type == SDL_KEYUP)
				  {
					
					  keyhold = false;
				  }

		}
		
	//SetCaption(lastPress);
		if(false && keyhold && SDL_GetTicks() - timerr > 300 )
		{
				
			
				//(lastPress);
			if(!(SDL_GetTicks() % 20))
			{
					 if(lastPress == SDLK_BACKSPACE)
						{
							if(curspos > 0 && content.Str().length() > 0) {
								content.Str().erase(curspos-1, 1);
								--curspos; }
								selpos = curspos;
						}
					 else if(lastPress == SDLK_DELETE)
					 {
							if(content.Str().length() > 0 && curspos < content.Str().length()-1 ) {
								content.Str().erase(curspos, 1);
								 }
							
						selpos = curspos;
					 }
						else if(lastPress == SDLK_RIGHT+800)      
						{
							
						if(curspos < content.Str().length())
                        {
                            ++curspos;
                        }
							
						if(!key_states[SDLK_LSHIFT] && !key_states[SDLK_RSHIFT] )selpos = curspos;
						}
						else if(lastPress == SDLK_LEFT+800)               
						{
						if(curspos > 0)
                        {
                            --curspos;
                        }	
						if(!key_states[SDLK_LSHIFT] && !key_states[SDLK_RSHIFT] )selpos = curspos;
						}
						else if(lastPress == SDLK_LCTRL + 800)
						{
							HANDLE clip = NULL;
                                if(OpenClipboard(NULL))
                                {
                                    clip = GetClipboardData(CF_TEXT);
                                     CloseClipboard();
                                    if(clip)
                                    {

                                 _string copied = (char*)clip;

                                 if(CutListing)
                                 while(true)
                                 {

                                        int poss = copied.find_first_not_of(CutList);
                                        if(poss !=-1)
                                        {
                                        copied.erase(poss, 1);
                                        }
                                        else break;
                                    }

                                    _string temp = content.Str();

                                    temp += copied;

                                    if(temp.length() > maxchars)
                                    {
                                       copied =  copied.substr(0, copied.length()-((temp.length()-maxchars)) ) ;
                                    }

                                    content.Str().insert(curspos, copied);
                                    curspos += copied.length();


                                      }

                                    }
                                    else  CloseClipboard();
										selpos = curspos;
					
	//SetCaption("noi chuj lol copy");
						}
					
						else if( ( ((lastPress >= (Uint16)' ') && (lastPress <= (Uint16)'~')) ) )
						{
			_string aa;
                            aa.clear();
                            aa += (char)lastPress;
			                    if(content.Str().length() <= 0)
                                {
                                    content.Str() = aa;
                                }
                                else
                                {
									if(  content.Str().length() < maxchars )
									{
                                content.Str().insert(curspos, aa);
									curspos++;
									}
                                 }
                            
								selpos = curspos;
						}
			}
		
		}
		}
		//else 
	//	EventsOnlyForGui = false;
		
	//Enable(GL_SCISSOR_TEST);

		int sx =x +Rx, sy = y +Ry, sw = w, sh = h;
		
		_Rectangle bigger;
		bigger.x = prevsciss[0];
		bigger.y = prevsciss[1];
		bigger.w = prevsciss[2];
		bigger.h = prevsciss[3];

		_Rectangle smaller;
		smaller.x = Rx + x;
		smaller.y = Ry + y;
		smaller.w = w;
		smaller.h = h;
		_Rectangle result = clipProduct(bigger, smaller); 

		           glScissor(result.x,result.y,result.w,result.h);
				   	   if(curspos != selpos && selected){
	   glTranslatef(-scrollx, 0, 0);
						   Enable(GL_BLEND);
	Disable(GL_TEXTURE_2D);
	glColor4f(1.0, 1.0, 1.0, 0.2);
	
	glBegin(GL_QUADS);
	
       glVertex2f(  Wt+2, h/2-content.h/2);
       glVertex2f(  Wt+2, h/2+content.h/2);
       glVertex2f(  Wt+(selpos < curspos ? -sWt : sWt ), h/2+content.h/2);
       glVertex2f(  Wt+(selpos < curspos ? -sWt : sWt ), h/2-content.h/2);

	glEnd();
	Disable(GL_BLEND);
	glTranslatef(scrollx, 0, 0);
	   }
    Enable(GL_TEXTURE_2D);
	//Disable(GL_SCISSOR_TEST);
			
					   if(result.w > 0 && result.h > 0)			   
	content.Draw();
	if(evresult == HIT || evresult == DOUBLE_HIT || evresult == MISSED_HOLD && selected)
	{
	//	if(!holded)
		{	
		//curspos = content.Str().length();
	//	MBX("START", "START");
			
		for(int i = 0; i < content.Str().length(); ++i)
		{
			_string _substr = content.Str().substr(0, i);
				if(content.tstyle->sizetext(_substr)+content.x >= mouse_x-(Rx+x)) 
				{
			//		iMBX(content.tstyle->sizetext(_substr)-content.x , 999999);
		//			if(holded)
				     curspos = i;
			
					
					
			//			selpos = i;
				
				break;
				
				}
		if(i == content.Str().length() - 1 ) curspos = content.Str().length();
		}
		}
		
		if(!holded) selpos = curspos;
		
		holded = true;
			//	MBX("END", "END");
				 if(evresult == DOUBLE_HIT)

					 {
						// if(content.Str()[i] != ' ' )
					//	 selpos = content.Str().find_last_of(" ", i);
						 if(content.Str()[curspos] != ' ')
						 {
						 selpos = content.Str().find_last_of(" ", curspos)+1;
						 if(selpos == -1) selpos = 0;
						 curspos = content.Str().find_first_of(" ", curspos);
						 if(curspos != -1)  curspos = content.Str().find_first_not_of(" ", curspos);
						 if(curspos == -1) curspos = content.Str().length();
						 } 
						 else
						 {
							 selpos = content.Str().find_last_not_of(" ", curspos);
							 if(selpos != -1)
						     selpos = content.Str().find_last_of(" ", selpos)+1;
							 if(selpos == -1) selpos = 0;
							 curspos = content.Str().find_first_not_of(" ", curspos);
							 if(curspos == -1) curspos = content.Str().length();

						 }


					 }
	//iMBX(mouse_x, Rx);
	}
	//else holded = false;
				//glScissor(prevsciss[0], prevsciss[1], prevsciss[2], prevsciss[3]);
	//Disable(GL_SCISSOR_TEST);
	
	//Enable(GL_SCISSOR_TEST);

	glTranslatef(-x, -y, 0.0);

}