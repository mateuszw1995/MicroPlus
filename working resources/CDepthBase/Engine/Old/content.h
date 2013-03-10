#ifndef CDBCONTENTBOX
#define CDBCONTENTBOX



struct Node
{
    virtual int getheight() = 0;
    virtual int getwidth() = 0;
	virtual void Draw() = 0;
};

struct ContentLine
{

	void WriteIn(Node*);
};

struct TextNode : public Node
{
	CText Text;
	void Draw()
	{
		Text.Draw();
	}
	int getwidth()
	{
		return Text.getwidth();
	}
	int getheight()
	{
		return Text.getheight();
	}
};
struct GUINode : public Node
{
	_GuiComponent* Elem;
	static int rx;
	static int ry;
	void Draw()
	{
		Elem->Draw();
	}
	int getwidth()
	{
		return Elem->w;
	}
	int getheight()
	{
		return Elem->h;
	}
};
int GUINode::rx = 0;
int GUINode::ry = 0;
struct ImageNode : public Node
{
	CTexture Image;
	void Draw()
	{
	if(Image)
	        glBindTexture(GL_TEXTURE_2D, Image.texture[0]);

	glBegin(GL_QUADS);
	  glTexCoord2f( 0.0f, 1.0f ); glVertex2f( 0, 0);
      glTexCoord2f( 1.0f, 1.0f ); glVertex2f(  Image.w, 0);
      glTexCoord2f( 1.0f, 0.0f ); glVertex2f(  Image.w,  Image.h);
      glTexCoord2f( 0.0f, 0.0f ); glVertex2f( 0,  Image.h);
	glEnd();
	}
	int getwidth()
	{
		return Image.w;
	}
	int getheight()
	{
		return Image.h;
	}
};





class ContentBox : public _GuiComponent
{
	vector<Node*> Drawings;
public:

	ContentBox (GLfloat _x = 0.0, GLfloat _y = 0.0, GLfloat _w = 0.0, GLfloat _h = 0.0) : _GuiComponent(_x, _y, _w, _h), Drawings(vector<Node*>(0))
	{
		_Style = &CWindow::_DefaultStyle;
	}
	void Draw();

	void WriteIn(string _Text)
	{
		TextNode* texnode = new TextNode;
		texnode->Text = CText(_Style->Font, _Text, 0.0, 0.0);
		Drawings.push_back(texnode);
	}
	void WriteIn(_GuiComponent* guielem)
	{
		GUINode* nnode = new GUINode;
		nnode->Elem = guielem;
		Drawings.push_back(nnode);
	}
	void WriteIn(CText _Text)
	{
		TextNode* texnode = new TextNode;
		texnode->Text = _Text;
		Drawings.push_back(texnode);
	}

};


void ContentBox::Draw()
{
		float* params = new float[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, params);

    int Rx = params[12];
	int Ry = params[13];

	delete  [] params;
	CEnum evresult = EventState(Rx, Ry);
	if(evresult != NO_EVENT)
	{
	}

	if(_Style)
		if(_Style->ContentBoxBackground.imported)
		{
	        glBindTexture(GL_TEXTURE_2D, _Style->ContentBoxBackground.texture[0]);
		}

			
	CDepthBase::Enable(GL_TEXTURE_2D);
	CDepthBase::Enable(GL_BLEND);
	
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	glPushMatrix();
	glTranslatef(x, y, 0.0);
	
	glBegin(GL_QUADS);
	  glTexCoord2f( 0.0f, 1.0f ); glVertex2f( 0, 0);
      glTexCoord2f( 1.0f, 1.0f ); glVertex2f(  w, 0);
      glTexCoord2f( 1.0f, 0.0f ); glVertex2f(  w,  h);
      glTexCoord2f( 0.0f, 0.0f ); glVertex2f( 0,  h);
	glEnd();	

	CDepthBase::Disable(GL_BLEND);

	CDepthBase::Disable(GL_TEXTURE_2D);

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

//	glScissor(x,y,w,h);
	GUINode::rx = Rx+x;
	GUINode::ry = Rx+y+h;
    //Node** nptr = Drawings.get_ptr(); 
	glTranslatef(0.0, h, 0.0);
	for(int i = 0; i < Drawings.size(); ++i)
	{
		GUINode::ry -= Drawings[i]->getheight();
	glTranslatef(0.0, - Drawings[i]->getheight(), 0.0);
       Drawings[i]->Draw();
	   
	}
	
	
	glPopMatrix();
}

#endif