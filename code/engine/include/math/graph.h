#pragma once
#include <vector>
#include "../misc/container/circular_buffer.h"

/*
ma byc mega generyczna

template i cos takiego:

getdistance(unsigned, unsigned) wymiennie z getdistance(vertex*, vertex*)

getvertex(num) albo ewentualnie iteratory
getedge(num)
getedgesize();
A*: http://theory.stanford.edu/~amitp/GameProgramming/MapRepresentations.html#hierarchical
http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html
http://www.valvesoftware.com/publications/2009/ai_systems_of_l4d_mike_booth.pdf
?? iterated_/numbered_ graph
*/
namespace db {
	namespace math {
		namespace graphs {
			struct vertex;
			struct edge {
				vertex *d;
				unsigned w;

				edge(vertex* destination, unsigned weight = 1);
				vertex* destination();
				unsigned weight();
			};
		
			struct vertex {
				std::vector<edge> edges;

				edge* get_edge(unsigned i) {
					return &edges[i];
				}
				unsigned edges_size() {
					return edges.size();
				}
			};
			
			template <class V, class E>
			struct path {
				unsigned distance;
				std::vector<E*> edges;
				V* start;

				class iterator {
					const path& p;
				public:
					V* current_vertex;
					E* current_edge;

					unsigned current_stage;
					unsigned current_distance;

					iterator(const path& p) : p(p)  { reset(); } 

					void reset() {
						current_stage = current_distance = 0;
						current_vertex = p.start;
						current_edge = 0;
					}

					bool advance(unsigned num) {
						for(int i = 0; i < num; ++i)
							if(current_stage < p.edges.size()) {
								current_edge = p.edges[current_stage++];
								current_distance += current_edge->weight();
								current_vertex = current_edge->destination();
							} else return false;
						return true;
					}
				};
			};
			
			template <class V = vertex, class E = edge>
			class graph { 
				misc::container::circular_buffer <V*> q;
				
				struct base {
					bool visited:1;
					vertex* previous_vertex;
					edge* previous_edge;
				};
				std::vector<base> basic;

				int ind(const V* ptr) {
					return ptr-vertices.data();
				}

			public:
				enum ALGORITHM {
					BFS,
					DFS
				};

				std::vector<V> vertices;

				graph(unsigned max_vertices) : q(misc::container::circular_buffer <V*>(max_vertices)) {
					basic.resize(max_vertices);
					vertices.reserve(max_vertices);
					clear_graph();
				}
				
				void clear_graph() {
					memset(basic.data(), 0, sizeof(vertex_base) * basic.size());
				}

				bool unit_search(ALGORITHM type, V* from, V* to = 0, bool (*found)(V*) = 0, path* out = 0) {
					V* v, *a;
					E* e;
					base* b;

					V*   (circular_buffer<T*>::*get)() = 0;
					void (circular_buffer<T*>::*remove)() = 0;

					switch(type) {
					case BFS: 
						get = &circular_buffer<T*>::front;
						remove = &circular_buffer<T*>::pop;
						break;

					case DFS:
						get = &circular_buffer<T*>::back;
						remove = &circular_buffer<T*>::pop_back;
						break;
					}

					bool is_found = false;
					int size;
					
					bases[ind(from)].visited = true;
					q.clear();
					q.push(from);
					while(!q.empty()) {
						v = q.*get();
						q.*remove();

						if((found && found(v)) || to == v) {
							is_found = true;
							break;
						}

						size = v->edges_size();
						for(int i = 0; i < size; ++i) {
							e = v->get_edge(i);
							a = e->destination();
							b = &bases[ind(a)];
							if(!b->visited) {
								b->visited = true;
								b->previous_vertex = v;
								b->previous_edge = e;

								q.push(a);
							}
						}
					}

					if(is_found) {
						if(out) path_to_source(to, out);
						return true;
					}

					return false;
				}
				
				void path_to_source  (V* target, path& out) {
					out.edges.clear();
					out.distance = 0;
					out.start = target;

					while(out.start) {
						out.edges.push_back(out.start->previous_edge);
						out.start = out.start->previous_vertex;
					}

					std::reverse(out.edges.begin(), out.edges.end());
				}
			};
		}
	}
}