#pragma once
class TextArea : public _GuiComponent
{
public:
	CText _text;
TextArea(GLfloat _x = 0.0, GLfloat _y = 0.0, GLfloat _w = 0.0, GLfloat _h = 0.0) : _GuiComponent(_x, _y, _w, _h), _text(CText(CWindow::_DefaultStyle.Font, " ", 10.0, 10.0 ))
{
	_Style = &CWindow::_DefaultStyle;
	_text.Line_Width(_w-20.0);

	
}
_string& Str()
{
	return _text.Str();
}
void Draw();

};

void TextArea::Draw()
{
	if(_text.getheight() > h) h = _text.getheight() + 20;
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

	_text.y = h-_text.h;
	_text.Draw();

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
	glTranslatef(-x, -y, 0.0);
	
}
