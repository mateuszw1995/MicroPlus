#pragma once
#include <vector>

// trzeba usprawnic rect2D - rozszerzyc max_size na rect_wh bo z samego max_s: wielkie prostokaty rozpychaja kwadrat a male wykorzystuja miejsce na dole

using namespace std;
namespace db {
	namespace window {
		class glwindow;
	}
	namespace math {
		struct rect_ltrb;
		struct rect_xywh;
		struct pointf;

		struct point {
			int x, y;
			point(int x = 0, int y = 0);
			point(const rect_ltrb&);
			point(const rect_xywh&);
			point(const pointf&);
			point operator-() const;
			point operator-(const point&) const;
			pointf operator-(const pointf&) const;
			point operator+(const point&) const;
			pointf operator+(const pointf&) const;
			point operator*(const point&) const;
			pointf operator*(const pointf&) const;
			point& operator-=(const point&);
			point& operator-=(const pointf&);
			point& operator+=(const point&);
			point& operator+=(const pointf&);
			point& operator*=(const point&);
			point& operator*=(const pointf&);
			pointf operator*(float) const;
		};

		double deg2rad(double);

		struct pointf {
			float x, y;
			pointf(const point&);
			pointf(const rect_ltrb&);
			pointf(const rect_xywh&);
			pointf(float x = 0.f, float y = 0.f);
			void normalize();
			float length() const;
			
			pointf operator-(const point&) const;
			pointf operator-(const pointf&) const;
			pointf operator+(const point&) const;
			pointf operator+(const pointf&) const;
			pointf operator*(const point&) const;
			pointf operator*(const pointf&) const;
			pointf& operator-=(const point&);
			pointf& operator-=(const pointf&);
			pointf& operator+=(const point&);
			pointf& operator+=(const pointf&);
			pointf& operator*=(const point&);
			pointf& operator*=(const pointf&);
			pointf operator*(float) const;
		};

		struct rect_wh {
			rect_wh(const rect_ltrb&);
			rect_wh(const rect_xywh&);
			rect_wh(int w = 0, int h = 0);
			int w,h, area() const, perimeter() const, max_side() const, 
				fits(const rect_wh& bigger) const; // 0 - no, 1 - yes, 2 - flipped, 3 - perfectly, 4 perfectly flipped
			void stick_relative(const rect_wh& content, pointf& scroll) const;
			bool is_sticked(const rect_wh& content, pointf& scroll) const;
			bool inside(const rect_wh& bigger) const;

			bool good() const;
			rect_wh operator*(float) const;
		};
		
		struct rect_ltrb {
			rect_ltrb();
			rect_ltrb(const rect_wh&);
			rect_ltrb(const rect_xywh&);
			rect_ltrb(int left, int top, int right, int bottom);
			rect_ltrb(const point&, const rect_wh&);

			void contain(const rect_ltrb& smaller);
			void contain_positive(const rect_ltrb& smaller);

			bool clip(const rect_ltrb& bigger);
			bool hover(const point& mouse) const;
			bool hover(const rect_ltrb&) const;
			bool inside(const rect_ltrb& bigger) const;
			
			bool stick_x(const rect_ltrb& bigger);
			bool stick_y(const rect_ltrb& bigger);
			
			pointf center() const;
			void center_x(int x);
			void center_y(int y);
			void center(const point&);

			int l, t, r, b, w() const, h() const, area() const, perimeter() const, max_side() const; // false - null rectangle
			void x(int), y(int), w(int), h(int);
			bool good() const;

			template <class P>
			rect_ltrb& operator+=(const P& p) {
				l += int(p.x);
				t += int(p.y);
				r += int(p.x);
				b += int(p.y);
				return *this;
			}
			
			template <class P>
			rect_ltrb operator-(const P& p) const {
				return rect_ltrb(l - int(p.x), t - int(p.y), r - int(p.x), b - int(p.y));
			}

			template <class P>
			rect_ltrb operator+(const P& p) const {
				return rect_ltrb(l + int(p.x), t + int(p.y), r + int(p.x), b + int(p.y));
			}
		};

		struct rect_xywh : public rect_wh {
			rect_xywh();
			rect_xywh(const rect_wh&);
			rect_xywh(const rect_ltrb&);
			rect_xywh(int x, int y, int width, int height);
			rect_xywh(int x, int y, const rect_wh&);
			rect_xywh(const point&, const rect_wh&);
			
			bool clip(const rect_xywh& bigger); // false - null rectangle
			bool hover(const point& mouse);

			rect_xywh& adjust(const window::glwindow* const);

			int x, y, r() const, b() const;
			void r(int), b(int);
			
			template <class P>
			rect_xywh& operator+=(const P& p) {
				x += int(p.x);
				y += int(p.y);
				return *this;
			}
			
			template <class P>
			rect_xywh operator-(const P& p) const {
				return rect_xywh(x - int(p.x), y - int(p.y), w, h);
			}

			template <class P>
			rect_xywh operator+(const P& p) const {
				return rect_xywh(x + int(p.x), y + int(p.y), w, h);
			}
		};
		
		struct rect_xywhf : public rect_xywh {
			rect_xywhf(const rect_wh  &);
			rect_xywhf(const rect_ltrb&);
			rect_xywhf(const rect_xywh&);
			rect_xywhf(int x, int y, int width, int height, bool flipped = false);
			rect_xywhf();
			void flip();
			rect_xywh rc() const;
			bool flipped;
		};

		struct rect_texture {
			float u1, v1, u2, v2;
			rect_texture(float u1 = 1.0, float v1 = 1.0, float u2 = 1.0, float v2 = 1.0);
		};

		struct point_texture {
			float u, v;
		};

		namespace geometry {
			template <class T> class vector;
			template <class T> class matrix;
			template <class T> class quaternion;
		}

		namespace pack {
			struct bin {
				rect_wh size;
				std::vector<rect_xywhf*> rects;
			};

			rect_wh _rect2D(rect_xywhf* const * v, int n, int max_s, vector<rect_xywhf*>* succ = 0, vector<rect_xywhf*>* unsucc = 0);
			
			/*
			0 - succesful
			1 - couldn't pack into max_bins/one bin
			2 - rect larger than bin exists
			*/
			
			int rect2D(rect_xywhf* const * v, int n, int max_side, bin& out_bin);
			int rect2D(rect_xywhf* const * v, int n, int max_side, vector<bin>& bins);
			int rect2D(rect_xywhf* const * v, int n, int max_side, vector<bin>& bins, int max_bins); 
		}

		
		namespace graphs {
			struct edge;
			struct vertex;
			template <class V, class E> struct path;
			template <class V, class E> class graph;
		}
	}
}