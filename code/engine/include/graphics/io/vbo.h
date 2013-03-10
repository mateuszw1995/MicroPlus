#pragma once

#include <vector>
#include <Windows.h>
#undef min
#undef max
#include <gl/glew.h>
// outdated(vbo template vertex static part, dynamic part)
// [vertex structures with vector of components] as argument to builds
namespace db {
	namespace graphics {
		namespace io {
			namespace input {
				
				struct vertex_spec {
					struct attrib {
						enum _type {
							SHORT = GL_SHORT,
							INT = GL_INT,
							FLOAT = GL_FLOAT,
							DOUBLE = GL_DOUBLE
						};

						bool on;
						_type type;
						unsigned buffer_binding;
						int components, stride;
						void* ptr;

						attrib();
						attrib(_type type, int components);
						int get_bytes();
						bool operator==(const attrib&);
					};

					std::vector<attrib> attribs;
					vertex_spec(const char* format); // example: 3f2f3f

					void update();
					int get_size();
				private:
					int size;
				};
			

				class vbo {
					unsigned id, id_dyn, id_str, id_i, bytes, bytes_dyn, bytes_str;
					bool built;
					static vertex_spec::attrib bindings[32];
					static unsigned current_array;
					static unsigned current_element;
				public:
					static bool vbo_supported;
					vbo(), ~vbo();
					
//					std::vector<mesh*> meshes;
					
					void open(), organize(), // sort by vertex types 
						build(), close();
					unsigned get_bytes(), get_bytes_dynamic();
				};

			}
		}
	}
}
