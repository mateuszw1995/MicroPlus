#pragma once
#include "../binpacker.h"

int Rect2DPacker::Rect::operator>(const Rect& b) {
	return (w*h) > (b.w*b.h);
}

void Rect2DPacker::Rect::Add(int width, int height) {
	static Rect r;
	r.x = 0;
	r.y = 0;
	r.w = width;
	r.h = height;
	r.flipped = false;
	recvec += r;
}
bool Rect2DPacker::Rect::Process(int minw, int minh, int maxw, int maxh) {
	struct _rproxy { 
		Rect r;
		int id;
	}:
	static _vectorm<_rproxy> vec;


};
_vectorm<Rect>& getVector();

_vectorm<Rect> recvec;