#pragma once

#include "../../Graphics/GUI/GUI.h"
#include "../../Container/vector.h"
#include "../../Container/list.h"
#include <string.h>
#include <GL/gl.h>

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif


GUI::Window* GUI::active_window = 0;

GUI::Window*& GUI::ActiveWindow() {
	return active_window;
}

GUI::EventRect::EventRect(int x, int y, int w, int h) : x(x), y(y), w(w), h(h), down(false) {}

bool GUI::EventRect::hover() {
	using namespace DB::Event;
	return Mouse::x >= x &&	Mouse::x <= x+w && Mouse::y >= y && Mouse::y <= y+h;
}
bool GUI::EventRect::hold() {
	return down;
}

int GUI::EventRect::poll() {
	using namespace DB::Event::Mouse;
	
		 if(*DB::evptr == LDown)						return (down = hover()) ? Hit : 0;
	else if(*DB::evptr == RDown)						return hover() ? Menu : 0;
	else if(*DB::evptr == LDoubleClick)					return (down = hover()) ? DoubleHit : 0;
	else if(*DB::evptr == LUp && down) { down = false;	return hover() ? ReleaseIn : ReleaseOut; }
	else if(*DB::evptr == Move && state[0]) {			return Drag; }

	return 0;
}

GUI::Window::Window() : update(true) {}

GUI::Window::TexInfo::TexInfo(int tex, int w, int h) : w(w), h(h), p_w(1.0f/float(w)), p_h(1.0f/float(h)), texture(tex) {}

GUI::GUIelement::GUIelement() {}

bool operator&(const GUI::Point& p, const GUI::Rect& r){
	return (p.x >= r.l && p.x <= r.r && p.y >= r.t && p.y <= r.b);
}

void RectProduct(const GUI::Rect& r1, const GUI::Rect& r2, GUI::Rect& out){
	if((r1.l > r2.r || r1.r < r2.l || r1.t > r2.b || r1.b < r2.t))
		memset(&out, 0, sizeof(GUI::Rect));
	else {
		out.l =	max(r1.l, r2.l); 
		out.t = max(r1.t, r2.t);
		out.r = min(r1.r, r2.r); 
		out.b = min(r1.b, r2.b);
	}
}

GUI::Parent::Parent(GUIelement* el) : elem(el) {}

GUI::HGUI GUI::Window::AddParent(GUIelement* g) {
	update = true;
	return Hierarchy += Parent(g);
}
GUI::HGUI GUI::Window::AddChild(HGUI h, GUIelement* g) {
	update = true;
	return h->_v.children += Parent(g);
}
void GUI::Window::RemoveParent(HGUI h) {
	update = true;
	Hierarchy.erase(h);
}
void GUI::Window::RemoveChild(HGUI h1, HGUI h2) {
	update = true;
	h1->_v.children.erase(h2);
}

void GUI::Window::HierarchyOP(HGUI first, void (Window::*op)(_list<HGUI>::Node*)) {
	static _list<HGUI> levels;
	static _list<HGUI>::Node* level_iter;

	level_iter = (levels += first);
	while(level_iter) {
		if(!level_iter->_v) {
			level_iter = level_iter->prev;
			levels.pop_back();
			continue;
		}
		HGUI iter = level_iter->_v->_v.children.first;
		if(iter)
			levels += iter;

		(this->*op)(level_iter);

		level_iter->_v = level_iter->_v->next;

		if(iter)
			level_iter = level_iter->next;

	}
	levels.clear();
}

void GUI::Window::Draw() {
	struct Level {
		HGUI h;
		Rect c_rect;
		Level() : h(0) { }
		Level(Rect& c_rect, HGUI h) : h(h), c_rect(c_rect) {} 
		Level(HGUI h) : h(h) {} 
	};

	static Rect rect;
	static DrawCall* dc;
	static DrawCall _dc;
	static GUIelement* g;
	static HGUI iter;
	static Level* level_iter;
	static Parent* p_iter;
	static _vectorm<Level> levels(2000);
	static _vectorm<DrawCall> disp_data;

	dc = update ? &_dc : disp_data._p;

	if(update) {
		disp_data.clear();
		levels.clear();
	}

	(level_iter = &levels.front())->h = Hierarchy.first;

	while(level_iter) {

		if(!level_iter->h) {
			--level_iter;
			continue;
		}
		iter = level_iter->h->_v.children.first;

		if(iter) {
			RectProduct(level_iter->c_rect, iter->_v.elem->area, (level_iter++)->c_rect);
			level_iter->h = iter;
		}

		g = level_iter->h->_v.elem;
		RectProduct(level_iter->c_rect, g->area, rect);
		if(memcmp(&rect, &g->area, sizeof(Rect)) || update ) {
			memcpy(&dc->ver[0], &rect, sizeof(Point));
			dc->ver[1].x = rect.r;
			dc->ver[1].y = rect.t;
			memcpy(&dc->ver[2], &rect.r, sizeof(Point));
			dc->ver[3].x = rect.l;
			dc->ver[3].y = rect.b;
			TexCoord adj_t[2] = { 
				g->tex[0].u - (g->area.l - rect.l)*texinfo.p_w,
				g->tex[0].v - (g->area.t - rect.t)*texinfo.p_h,
				g->tex[1].u - (g->area.r - rect.r)*texinfo.p_w,
				g->tex[1].v - (g->area.b - rect.b)*texinfo.p_h 
			};

			memcpy(dc->tex, adj_t, sizeof(Point));
			dc->tex[1].u = adj_t[1].u;
			dc->tex[1].v = adj_t[0].v;
			memcpy(dc->tex+2, adj_t+1, sizeof(Point));
			dc->tex[3].u = adj_t[0].u;
			dc->tex[3].v = adj_t[1].v;

			if(update) {
				memcpy(&dc->col, &g->color, sizeof(RGBA_1));
				disp_data.push_back(_dc);
			}
			else ++dc;
		}
		else {
			--level_iter;
			iter = 0;
		}

		level_iter->h = level_iter->h->next;

		if(iter)
			++level_iter;
	}

	if(update) {
		glVertexPointer(2, GL_INT, sizeof(DrawCall) - sizeof(Point[4]), disp_data._p);
		glTexCoordPointer(2, GL_FLOAT, sizeof(DrawCall) - sizeof(TexCoord[4]), disp_data._p->tex);
		glColorPointer(4, GL_FLOAT, sizeof(DrawCall) - sizeof(RGBA_1), &disp_data._p->col);
		disp_data.trim();
		update = false;
	}

	levels.clear();

	glDrawArrays(GL_QUADS, 0, disp_data.size());
}
//
//disp_data.clean();
//
//_list<HGUI> levels;
//_list<HGUI>::Node* level_iter = (levels += Hierarchy.first); 
//
//while(level_iter) {
//	HGUI iter = level_iter->_v->_v.children.first;
//	if(iter)
//		levels += iter;
//
//	// operacja
//
//	level_iter->_v = level_iter->_v->next;
//
//	if(iter) {
//		level_iter = level_iter->next;
//		continue;
//	}
//	if(!level_iter->_v) {
//		level_iter = level_iter->prev;
//		levels.erase(levels.last);
//	}
//
//}