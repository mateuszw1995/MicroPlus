#pragma once
#include "math.h"
#include <vector>
#include <algorithm>

using namespace std;

namespace db {
	namespace math {
		namespace pack {
			
			bool area(rect_xywhf* a, rect_xywhf* b) {
				return a->area() > b->area();
			}

			bool r_area(rect_xywhf* a, rect_xywhf* b) {
				return a->area() < b->area();
			}

			bool perimeter(rect_xywhf* a, rect_xywhf* b) {
				return a->perimeter() > b->perimeter();
			}

			bool max_side(rect_xywhf* a, rect_xywhf* b) {
				return std::max(a->w, a->h) > std::max(b->w, b->h);
			}

			bool max_width(rect_xywhf* a, rect_xywhf* b) {
				return a->w > b->w;
			}

			bool max_height(rect_xywhf* a, rect_xywhf* b) {
				return a->h > b->h;
			}


			struct node {
				struct pnode {
					node* pn;
					bool fill;

					pnode() : fill(false), pn(0) {}
					void set(int l, int t, int r, int b) {
						if(!pn) pn = new node(rect_ltrb(l, t, r, b));
						else {
							(*pn).rc = rect_ltrb(l, t, r, b);
							(*pn).id = false;
						}
						fill = true;
					}
				};

				pnode c[2];
				rect_ltrb rc;
				bool id;
				node(rect_ltrb rc = rect_ltrb()) : id(false), rc(rc) {}

				void reset(const rect_wh& r) {
					id = false;
					rc = rect_ltrb(0, 0, r.w, r.h);
					delcheck();
				}

				node* insert(rect_xywhf& img) {
					if(c[0].pn && c[0].fill) {
						node* newn;
						if(newn = c[0].pn->insert(img)) return newn;
						return    c[1].pn->insert(img);
					}

					if(id) return 0;
					int f = img.fits(rect_xywh(rc));

					switch(f) {
					case 0: return 0;
					case 1: img.flipped = false; break;
					case 2: img.flipped = true; break;
					case 3: id = (&img != NULL); img.flipped = false; return this;
					case 4: id = (&img != NULL); img.flipped = true;  return this;
					}

					int iw = (img.flipped ? img.h : img.w), ih = (img.flipped ? img.w : img.h);

					if(rc.w() - iw > rc.h() - ih) {
						c[0].set(rc.l, rc.t, rc.l+iw, rc.b);
						c[1].set(rc.l+iw, rc.t, rc.r, rc.b);
					}
					else {
						c[0].set(rc.l, rc.t, rc.r, rc.t + ih);
						c[1].set(rc.l, rc.t + ih, rc.r, rc.b);
					}

					return c[0].pn->insert(img);
				}

				void delcheck() {
					if(c[0].pn) { c[0].fill = false; c[0].pn->delcheck(); }
					if(c[1].pn) { c[1].fill = false; c[1].pn->delcheck(); }
				}

				~node() {
					if(c[0].pn) delete c[0].pn;
					if(c[1].pn) delete c[1].pn;
				}
			};

			rect_wh _rect2D(rect_xywhf* const * v, int n, int max_s, vector<rect_xywhf*>* succ, vector<rect_xywhf*>* unsucc) {
				node root;

				enum {
					SQUARE = 0,
					HEIGHT = 1,

					WIDTH = 2
				};
				int pass = SQUARE;

				// just add a function name to execute more heuristics
				bool (*cmpf[])(rect_xywhf*, rect_xywhf*) = { 
					area, perimeter, max_side, max_width, max_height
				};

				const int funcs = (sizeof(cmpf)/sizeof(bool (*)(rect_xywhf*, rect_xywhf*)));

				rect_xywhf** order[funcs];

				for(int f = 0; f < funcs; ++f) { 
					order[f] = new rect_xywhf*[n];
					memcpy(order[f], v, sizeof(rect_xywhf*) * n);
					sort(order[f], order[f]+n, cmpf[f]);
				}

				rect_wh min_bin = rect_wh(max_s, max_s);
				int min_func = -1, best_func = 0, best_area = 0, _area = 0, step, fit, i;

				bool fail = false;

				for(int f = 0; f < funcs; ++f) {
					v = order[f];
					step = min_bin.w / 2;
					root.reset(min_bin);
					pass = SQUARE;

					while(true) {
						if(root.rc.area() > min_bin.area() || root.rc.max_side() > max_s) {
							if(min_func > -1) break;
							_area = 0;

							root.reset(min_bin);
							for(i = 0; i < n; ++i)
								if(root.insert(*v[i]))
									_area += v[i]->area();

							fail = true;
							break;
						}

						fit = -1;

						for(i = 0; i < n; ++i)
							if(!root.insert(*v[i])) {
								fit = 1;
								break;
							}

							if(fit == -1 && step <= 64) {
								++pass;
								if(pass > WIDTH) break;
								step = (pass == WIDTH) ? root.rc.w() : root.rc.h();
								step /= 2;
							}
							
							root.reset(rect_wh(
									   root.rc.w() + ((pass == SQUARE || pass == WIDTH  )? fit*step : 0), 
									   root.rc.h() + ((pass == SQUARE || pass == HEIGHT )? fit*step : 0)
									   ));

							step /= 2;
							if(!step) 
								step = 1;
					}

					if(!fail && (min_bin.area() >= root.rc.area())) {
						min_bin = rect_wh(root.rc);
						min_func = f;
					}

					else if(fail && (_area > best_area)) {
						best_area = _area;
						best_func = f;
					}
					fail = false;
				}

				v = order[min_func == -1 ? best_func : min_func];

				int clip_x = 0, clip_y = 0;
				node* ret;

				root.reset(min_bin);

				for(i = 0; i < n; ++i) {
					if(ret = root.insert(*v[i])) {
						v[i]->x = ret->rc.l;
						v[i]->y = ret->rc.t;

						if(v[i]->flipped) {
							v[i]->flipped = false;
							v[i]->flip();
						}

						clip_x = std::max(clip_x, ret->rc.r);
						clip_y = std::max(clip_y, ret->rc.b); 

						if(succ) succ->push_back(v[i]);
					}
					else {
						if(unsucc) unsucc->push_back(v[i]);
						else return rect_wh(-1, -1);

						v[i]->flipped = false;
					}
				}

				for(int f = 0; f < funcs; ++f)
					delete [] order[f];

				return rect_wh(clip_x, clip_y);
			}

			
			int rect2D(rect_xywhf* const * v, int n, int max_s, bin& out_bin) {
				out_bin.size = rect_wh(max_s, max_s);

				for(int i = 0; i < n; ++i) 
					if(!v[i]->fits(out_bin.size)) return 2;

				out_bin.size = _rect2D(v, n, max_s);

				if(out_bin.size.w == -1) return 1;
				return 0;
			}

			int rect2D(rect_xywhf* const * v, int n, int max_s, std::vector<bin>& bins) {
				return rect2D(v, n, max_s, bins, -1);
			}

			int rect2D(rect_xywhf* const * v, int n, int max_s, vector<bin>& bins, unsigned max_bins) {
				rect_wh _rect(max_s, max_s);

				for(int i = 0; i < n; ++i) 
					if(!v[i]->fits(_rect)) return 2;

				vector<rect_xywhf*> vec[2], *p[2] = { vec, vec+1 };
				vec[0].resize(n);
				vec[1].clear();
				memcpy(&vec[0][0], v, sizeof(rect_xywhf*)*n);

				bin* b = 0;

				while(true) {
					bins.push_back(bin());
					b = &bins[bins.size()-1];

					b->size = _rect2D(&((*p[0])[0]), p[0]->size(), max_s, &b->rects, p[1]);
					b->rects.shrink_to_fit();
					
					p[0]->clear();

					if(!p[1]->size()) break;
					else if(max_bins > -1 && bins.size() <= max_bins) 
						return 1; 

					std::swap(p[0], p[1]);
				}

				return 0;
			}

		}
	}
}