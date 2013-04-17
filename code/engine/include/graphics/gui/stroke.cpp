#pragma once
#include "stroke.h"
#include "rect.h"

namespace db {
	namespace graphics {
		namespace gui {
			solid_stroke::solid_stroke(int width, const material& mat, type _type) : _type(_type) {
				left.width = width; left.mat = mat;
				right = top = bottom = left;
			}

			void solid_stroke::set_width(int w) {
				left.width = top.width = right.width = bottom.width = w; 
			}

			void solid_stroke::set_material(const material& mat) {
				left.mat = top.mat = right.mat = bottom.mat = mat; 
			}
				
			void solid_stroke::draw(std::vector<quad>& out, const rect& r) const {
				draw(out, r.get_rect_absolute(), r.parent ? &r.parent->get_clipped_rect() : nullptr);
			}

			void solid_stroke::draw(std::vector<quad>& out, rect_ltrb g, const rect_ltrb* clipper) const {
				if(_type == OUTSIDE) {
					g.l -= left.width;
					g.t -= top.width;
					g.r += right.width;
					g.b += bottom.width;
				}

				rect_ltrb lines[4] = { g, g, g, g }; 

				lines[0].r = g.l + left.width;
				lines[1].b = g.t + top.width;
				lines[2].l = g.r - right.width;
				lines[3].t = g.b - bottom.width;

				if(top.width) {
					lines[0].t += top.width;
					lines[2].t += top.width;
				}
				if(bottom.width) {
					lines[0].b -= top.width;
					lines[2].b -= top.width;
				}

				add_quad(left.  mat, lines[0], clipper, out);
				add_quad(top.   mat, lines[1], clipper, out);
				add_quad(right. mat, lines[2], clipper, out);
				add_quad(bottom.mat, lines[3], clipper, out);
			}
			
		}
	}
}