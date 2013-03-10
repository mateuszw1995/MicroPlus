#pragma once
#include "../../Math/maths.h"

struct RPTR {
	math::RectXYWHf* p;
};
bool byarea = false;
bool operator>(RPTR a, RPTR b) {
	using namespace math;
	if(byarea)return a.p->w * a.p->h < b.p->w * b.p->h; // sort by larger area
	return max(a.p->w, a.p->h) < max(b.p->w, b.p->h); // sort by larger dimension
}

math::RectWH math::rectangle2Dpack(RectXYWHf* arr, int n, bool area, int stride, int nrec, int maxl) {
	static RectWH bin;
	static RPTR p;
	static int expanderw, expanderh;
	static bool fits[2];
	expanderw = expanderh = 0;
	p.p = 0;

	struct Node {
		Node* child[2];
		RectXYWH r;
		bool fill;
		bool use;
		Node() { child[0] = child[1] = 0; fill = false; r.x = r.y = r.w = r.h = 0; use = true; } 
		~Node() {
			if(child[0]) delete child[0];
			if(child[1]) delete child[1]; 
		}
		void delcheck() { 
			if(child[0]) child[0]->delcheck();
			if(child[1]) child[1]->delcheck();
			use = fill = false;
		}
		Node* Insert() {  
			static Node* node;
			static int dw, dh, iw, ih;
			if (child[0] && child[0]->use) { 
				if(node = child[0]->Insert()) return node;
				return child[1]->Insert();
			}
			else {
				if(fill) return 0;

//				p.p->flip = false;				
				dw = p.p->w - r.w;
				dh = p.p->h - r.h;
				if(dw > 0 || dh > 0) { 
					dw = p.p->h - r.w;
					dh = p.p->w - r.h;
					if(dw > 0 || dh > 0) 
						return 0; 
					swap(p.p->w, p.p->h);
					p.p->flip = !p.p->flip;
				}
				p.p->x = r.x;
				p.p->y = r.y;

				if(!dw && !dh) return this;

				if(!child[0]) {
				child[0] = new Node;
				child[1] = new Node;
				}
				child[0]->use = child[1]->use = true;
				
				iw = p.p->w;
				ih = p.p->h;
				//iw = p.p->flip ? p.p->w : p.p->h;
				//ih = p.p->flip ? p.p->h : p.p->w;
				
					//sMBX(_string("iw: ") + _string::i2s(iw), _string("ih: ") + _string::i2s(ih));
				
				if(r.w - iw > r.h - ih) {
					RectXYWH rc0 = { r.x,    r.y,     iw, r.h };
					RectXYWH rc1 = { r.x+iw, r.y, r.w-iw, r.h };
					child[0]->r = rc0;
					child[1]->r = rc1;
					if(r.h == ih) 
						return child[0];
				//	sMBX(_string("iw: ") + _string::i2s(iw), _string("r.h: ") + _string::i2s(r.h));
				}
				else {
					RectXYWH rc0 = { r.x, r.y,	  r.w,     ih };
					RectXYWH rc1 = { r.x, r.y+ih, r.w, r.h-ih };
					child[0]->r = rc0;
					child[1]->r = rc1;
					if(r.w == iw) 
						return child[0];
				//	sMBX(_string("r.w: ") + _string::i2s(r.w), _string("ih: ") + _string::i2s(ih));
				}
				return child[0]->Insert();
			}	
		}
	};
	bin.w = bin.h = 0;
	byarea = area;
	if(n < 1 || !arr) return bin;

	RPTR* ptr = new RPTR[n];

	for(int i = 0; i < n; ++i)
		ptr[i].p = (RectXYWHf*)(((char*)arr)+i*(stride+sizeof(RectXYWHf)));

	sort::heap(ptr, n);
	bin.w = bin.h = 0;
	Node* root, *node = 0;
	
	int i, r_size = maxl/2, turn = maxl/2, far_x = 0, far_y = 0;
	root = new Node;
	while(turn) {
		turn /= 2;
		root->r.w = root->r.h = r_size;
		for(i = 0; i < nrec; ++i) {
			p = ptr[i];
			if(!(node = root->Insert())) break;
			node->fill = true;
		}
		root->delcheck();
		if(!turn && !node) turn = 1;
		r_size += (node ? -1 : 1) * turn;
	}
	
	for(; i < nrec; ++i) {
	ptr[i].p->x = ptr[i].p->y = -1; 
	ptr[i].p->flip = false; 
	}
	for(i = 0; i < nrec; ++i) {
		p = ptr[i];
		far_x = max(far_x, p.p->x+p.p->w);
		far_y = max(far_y, p.p->y+p.p->h);
	}
	bin.w = far_x;
	bin.h = far_y;
	
	delete root;
	delete [] ptr;
	return bin;
};



//int MinW = 0, MinH = 0, MaxW = 0, MaxH = 0, MinArea = 0; // actual stats
//static RectXYWHf* r;
//for(RPTR* i = ptr; i < ptr+n;) {
//	r=i++->p;
//	if(r->w > MinW) MinW = r->w;
//	if(r->h > MinH) MinH = r->h;
//	MaxW += r->w;
//	MaxH += r->h;
//	MinArea += r->h*r->w;
//}
//MaxW = min(MaxW, maxw);
//MaxH = min(MaxH, maxh);

//#include "../binpacker.h"
//#include "../maths.h"
//
//int Rect2DPacker::Rect::operator>(const Rect& b) {
//	return (w*h) > (b.w*b.h);
//}
//
//void Rect2DPacker::Add(int width, int height) {
//	static Rect r;
//	r.x = 0;
//	r.y = 0;
//	r.w = width;
//	r.h = height;
//	r.flip = false;
//	recvec += r;
//}
//bool Rect2DPacker::Process(int minw, int minh, int maxw, int maxh) {
//	struct _rproxy { 
//		Rect r;
//		int arid;
//		int operator>(_rproxy& b) { return r > b.r; }
//	} pr;
//	static _vectorm<_rproxy> vec;
//	vec.reserve(recvec.size());
//	for(int i = 0; i < recvec.size(); ++i) {
//		pr.arid = i; pr.r = recvec[i];
//		vec[i] = pr;
//	}
//
//	heapsort(vec.get_ptr(), vec.size());
//
//
//};
//_vectorm<Rect2DPacker::Rect>& Rect2DPacker::getVector() { return recvec; }
//_vectorm<Rect2DPacker::Rect> Rect2DPacker::recvec;