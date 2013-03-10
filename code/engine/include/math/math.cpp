#pragma once
#include "math.h"
#include <algorithm>

namespace db {
	namespace math {

		double deg2rad(double a) {
			return a*0.01745329251994329576923690768489;
		}
		
		point::point(int x, int y) : x(x), y(y) {}
		
		point::point(const pointf& p) : x(int(p.x)), y(int(p.y)) {} 
		point::point(const rect_xywh& p) : x(int(p.x)), y(int(p.y)) {} 
		point::point(const rect_ltrb& p) : x(int(p.l)), y(int(p.t)) {} 
		
		point point::operator-(const point& p) const {
			return point(x - p.x, y - p.y);
		}

		pointf point::operator-(const pointf& p) const {
			return pointf(x - p.x, y - p.y);
		}
		
		point point::operator+(const point& p) const {
			return point(x + p.x, y + p.y);
		}

		pointf point::operator+(const pointf& p) const {
			return pointf(x + p.x, y + p.y);
		}

		point point::operator*(const point& p) const {
			return point(x * p.x, y * p.y);
		}
		
		pointf point::operator*(const pointf& p) const {
			return pointf(x * p.x, y * p.y);
		}
		
		pointf point::operator*(float p) const {
			return pointf(x * p, y * p);
		}
		
		point& point::operator-=(const point& p) {
			x -= p.x;
			y -= p.y;
			return *this;
		}

		point& point::operator-=(const pointf& p) {
			x -= int(p.x);
			y -= int(p.y);
			return *this;
		}

		point& point::operator+=(const point& p) {
			x += p.x;
			y += p.y;
			return *this;
		}

		point& point::operator+=(const pointf& p) {
			x += int(p.x);
			y += int(p.y);
			return *this;
		}
		
		point& point::operator*=(const point& p) {
			x *= p.x;
			y *= p.y;
			return *this;
		}
		
		point& point::operator*=(const pointf& p) {
			x = int(float(x) * p.x);
			y = int(float(y) * p.y);
			return *this;
		}
		
		
		pointf::pointf(const point& p) : x(float(p.x)), y(float(p.y)) {}
		pointf::pointf(const rect_xywh& p) : x(float(p.x)), y(float(p.y)) {}
		pointf::pointf(const rect_ltrb& p) : x(float(p.l)), y(float(p.t)) {}
		
		pointf::pointf(float x, float y) : x(x), y(y) {}
			
		float pointf::length() {
			return sqrt(x*x + y*y);
		}

		void pointf::normalize() {
			float len = 1.0f/length();
			x *= len;
			y *= len;
		}
		
		pointf pointf::operator-(const point& p) const {
			return pointf(x - p.x, y - p.y);
		}

		pointf pointf::operator-(const pointf& p) const {
			return pointf(x - p.x, y - p.y);
		}

		pointf pointf::operator+(const point& p) const {
			return pointf(x + p.x, y + p.y);
		}

		pointf pointf::operator+(const pointf& p) const {
			return pointf(x + p.x, y + p.y);
		}
		
		pointf pointf::operator*(const point& p) const {
			return pointf(x * p.x, y * p.y);
		}
		
		pointf pointf::operator*(const pointf& p) const {
			return pointf(x * p.x, y * p.y);
		}
		
		pointf pointf::operator*(float p) const {
			return pointf(x * p, y * p);
		}
		
		pointf& pointf::operator-=(const point& p) {
			x -= p.x;
			y -= p.y;
			return *this;
		}

		pointf& pointf::operator-=(const pointf& p) {
			x -= p.x;
			y -= p.y;
			return *this;
		}

		pointf& pointf::operator+=(const point& p) {
			x += p.x;
			y += p.y;
			return *this;
		}

		pointf& pointf::operator+=(const pointf& p) {
			x += p.x;
			y += p.y;
			return *this;
		}
		
		pointf& pointf::operator*=(const point& p) {
			x *= p.x;
			y *= p.y;
			return *this;
		}
		
		pointf& pointf::operator*=(const pointf& p) {
			x *= p.x;
			y *= p.y;
			return *this;
		}

		rect_wh::rect_wh(const rect_ltrb& rr) : w(rr.w()), h(rr.h()) {} 
		rect_wh::rect_wh(const rect_xywh& rr) : w(rr.w), h(rr.h) {} 
		rect_wh::rect_wh(int w, int h) : w(w), h(h) {}
		
		int rect_wh::fits(const rect_wh& r) const {
			if(w == r.w && h == r.h) return 3;
			if(h == r.w && w == r.h) return 4;
			if(w <= r.w && h <= r.h) return 1;
			if(h <= r.w && w <= r.h) return 2;
			return 0;
		}
			
		rect_wh rect_wh::operator*(float s) const {
			return rect_wh(int(w*s), int(h*s));
		}
		
		rect_ltrb::rect_ltrb() : l(0), t(0), r(0), b(0) {}
		rect_ltrb::rect_ltrb(const rect_wh& rr) : l(0), t(0), r(rr.w), b(rr.h) {}
		rect_ltrb::rect_ltrb(const rect_xywh& rr) : l(rr.x), t(rr.y), r(rr.x+rr.w), b(rr.y+rr.h) {}
		rect_ltrb::rect_ltrb(int l, int t, int r, int b) : l(l), t(t), r(r), b(b) {}

		int rect_ltrb::w() const {
			return r-l;
		}
		
		int rect_ltrb::h() const {
			return b-t;
		}

		int rect_ltrb::area() const {
			return w()*h();
		}

		int rect_ltrb::perimeter() const {
			return 2*w() + 2*h();
		}

		int rect_ltrb::max_side() const {
			return std::max(w(), h());
		}
		
		void rect_ltrb::contain(const rect_ltrb& rc) {
			l = min(l, rc.l);
			t = min(t, rc.t);
			r = max(r, rc.r);
			b = max(b, rc.b);
		}

		bool rect_ltrb::clip(const rect_ltrb& rc) {
			if(l >= rc.r || t >= rc.b || r <= rc.l || b <= rc.t) {
				*this = rect_ltrb();
				return false;
			}
			*this = rect_ltrb(std::max(l, rc.l),
				std::max(t, rc.t),
				std::min(r, rc.r),
				std::min(b, rc.b));
			return true;
		}
		
		bool rect_ltrb::hover(const point& m) const {
			return m.x >= l && m.y >= t && m.x <= r && m.y <= b;
		}

		bool rect_ltrb::hover(const rect_ltrb& rc) const {
			return !(l >= rc.r || t >= rc.b || r <= rc.l || b <= rc.t);
		}
		
		bool rect_ltrb::inside(const rect_ltrb& rc) const {
			return l >= rc.l && r <= rc.r && t >= rc.t && b <= rc.b;
		}
		
		bool rect_ltrb::stick_x(const rect_ltrb& rc) {
			point offset(0, 0);
			if(l < rc.l) offset.x += rc.l - l; 
			if(r > rc.r) offset.x += rc.r - r; 
			operator+=(offset);

			return offset.x == 0;
		}

		bool rect_ltrb::stick_y(const rect_ltrb& rc) {
			point offset(0, 0);
			if(t < rc.t) offset.y += rc.t - t; 
			if(b > rc.b) offset.y += rc.b - b; 
			operator+=(offset);

			return offset.y == 0;
		}
		
		pointf rect_ltrb::center() {
			return pointf(l + w()/2.f, t + h()/2.f); 
		}

		void rect_wh::stick_relative(const rect_ltrb& bbox, pointf& pen) const {
			pen.x = min(pen.x, float(bbox.r - w));
			pen.x = max(pen.x, 0.f);
			pen.y = min(pen.y, float(bbox.b - h));
			pen.y = max(pen.y, 0.f);
		}

		bool rect_wh::is_sticked(const rect_ltrb& bbox, pointf& pen) const {
			return pen.x >= 0.f && pen.x <= bbox.r - w && pen.y >= 0 && pen.y <= bbox.b - h; 
		}
		
		void rect_ltrb::center_x(int c) {
			int _w = w();
			l = c - _w/2;
			r = l + _w;
		}

		void rect_ltrb::center_y(int c) {
			int _h = h();
			t = c - _h/2;
			b = t + _h;
		}

		void rect_ltrb::center(const point& c) {
			center_x(c.x);
			center_y(c.y);
		}
		
		void rect_ltrb::x(int xx) {
			*this+=(point(xx-l, 0));
		}

		void rect_ltrb::y(int yy) {
			*this+=(point(0, yy-t));
		}

		void rect_ltrb::w(int ww) {
			r = l+ww;
		}
		
		void rect_ltrb::h(int hh) {
			b = t+hh;
		}

		bool rect_ltrb::good() {
			return w() > 0 && h() > 0;
		}
		
		bool rect_wh::  good() {
			return w   > 0 && h   > 0;
		}

		rect_xywh::rect_xywh() : x(0), y(0) {}
		rect_xywh::rect_xywh(const rect_wh& rr) : x(0), y(0), rect_wh(rr) {}
		rect_xywh::rect_xywh(const rect_ltrb& rc) : x(rc.l), y(rc.t) { b(rc.b); r(rc.r); }
		rect_xywh::rect_xywh(int x, int y, int w, int h) : x(x), y(y), rect_wh(w, h) {}
		rect_xywh::rect_xywh(int x, int y, const rect_wh& r) : x(x), y(y), rect_wh(r) {}

		bool rect_xywh::clip(const rect_xywh& rc) {
			if(x >= rc.r() || y >= rc.b() || r() <= rc.x || b() <= rc.y) {
				*this = rect_xywh();
				return false;
			}
			*this = rect_ltrb(std::max(x, rc.x),
				std::max(y, rc.y),
				std::min(r(), rc.r()),
				std::min(b(), rc.b()));
			return true;
		}

		bool rect_xywh::hover(const point& m) {
			return m.x >= x && m.y >= y && m.x <= r() && m.y <= b();
		}
		
		int rect_xywh::r() const {
			return x+w;
		};
		
		int rect_xywh::b() const {
			return y+h;
		}

		void rect_xywh::r(int right) {
			w = right-x;
		}
		
		void rect_xywh::b(int bottom) {
			h = bottom-y;
		}

		int rect_wh::area() const {
			return w*h;
		}
		
		int rect_wh::perimeter() const {
			return 2*w + 2*h; 
		}
		
		int rect_wh::max_side() const {
			return std::max(w, h);
		}

		
		rect_xywhf::rect_xywhf(const rect_ltrb& rr) : rect_xywh(rr), flipped(false) {}
		rect_xywhf::rect_xywhf(const rect_xywh& rr) : rect_xywh(rr), flipped(false) {}
		rect_xywhf::rect_xywhf(const rect_wh  & rr) : rect_xywh(rr), flipped(false) {}
		rect_xywhf::rect_xywhf(int x, int y, int width, int height, bool flipped) : rect_xywh(x, y, width, height), flipped(flipped) {}
		rect_xywhf::rect_xywhf() : flipped(false) {}

		void rect_xywhf::flip() { 
			flipped = !flipped;
			std::swap(w, h);
		}
		
		rect_xywh rect_xywhf::rc() const {
			return rect_xywh(x, y, flipped ? h : w, flipped ? w : h);
		}

		rect_texture::rect_texture(float u1, float v1, float u2, float v2) : u1(u1), v1(v1), u2(u2), v2(v2) {}

	}
}