#pragma once
#include "vbo.h"
#include <gl/glew.h>
// jednak chyba template i custom typ z static funkcja dodajaca componenty
namespace db {
	namespace graphics {
		namespace io {
			namespace input {
				class mesh {
					struct base {
						enum flag {
							STATIC, DYNAMIC, STREAM
						};
						
						template <class vertex>
						std::vector<vertex>& get_vertices(vertex);
						
						void update(int begin, int end);

						unsigned get_bytes() const;
						flag get_usage() const;
					
						base(const vertex_spec& spec, flag usage);
					private:
						friend class vbo;
						const vertex_spec spec;
						const flag f;
						vbo* pt;
						
						std::vector<char> vertices;
					};
					unsigned rmin, rmax;
				public:
					enum i_type {
						UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
						UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
						UNSIGNED_INT = GL_UNSIGNED_INT
					} indices_used;

					std::vector<unsigned> indices_ui;
					std::vector<unsigned short> indices_us;
					std::vector<unsigned char> indices_ub;
					base b_static, b_dynamic, b_stream;

					template<class T>
					mesh(vertex_spec& v_static, vertex_spec& v_dynamic = vertex_spec(), vertex_spec& v_stream = vertex_spec());

					vbo* get_vbo();
				};
			}
		}
	}
}