#pragma once
#include "vbo.h"
#include "mesh.h"
#include <algorithm>

namespace db {
	namespace graphics {
		namespace io {
			namespace input {
				bool vbo::vbo_supported = false;

				unsigned vbo::current_array = 0, vbo::current_element = 0;
				
				vertex_spec::attrib::attrib() : buffer_binding(0), on(false), components(4), stride(0), type(FLOAT) {}

				vertex_spec::attrib::attrib(_type type, int components) : buffer_binding(0), on(false), stride(0), type(type), components(components) {}

				int vertex_spec::attrib::get_bytes() {
					switch(type) {
					case attrib::SHORT: return components * sizeof(short);
					case attrib::INT: return components * sizeof(int);
					case attrib::FLOAT: return components * sizeof(float);
					case attrib::DOUBLE: return components * sizeof(double);
					}
					return 0;
				}

				bool vertex_spec::attrib::operator==(const attrib& b) {
					return stride == b.stride && components == b.components && type == b.type && ptr == b.ptr;
				}

				vertex_spec::vertex_spec(const char* format) {
					int len = strlen(format), i = 0;
					attrib::_type type;
					while(i < len) {
						switch(format[i+1]) {
						case 's': type = attrib::SHORT; break;
						case 'i': type = attrib::INT; break;
						case 'f': type = attrib::FLOAT; break;
						case 'd': type = attrib::DOUBLE; break;
						default:  type = attrib::FLOAT; break;
						}
						attribs.push_back(attrib(type, (format[i] - '0')));
						i += 2;
					}

					update();
				}

				void vertex_spec::update() {
					size = 0;
					size_t i;
					for(i = 0; i < attribs.size(); ++i) {
						attribs[i].ptr = i ? (((char*)(attribs[i-1].ptr)) + attribs[i-1].get_bytes()) : NULL; 
						size += attribs[i].get_bytes(); 
					}
					
					for(i = 0; i < attribs.size(); ++i)
						attribs[i].stride = size;
				}

				int vertex_spec::get_size() {
					return size;
				}


				vbo::vbo() : built(false), bytes(0), bytes_dyn(0), bytes_str(0) {}
				
				vbo::~vbo() {
					close();
				}
				
				void vbo::open() {
					close();
					glGenBuffers(1, &id);
					glGenBuffers(1, &id_dyn);
					glGenBuffers(1, &id_str);
					glGenBuffers(1, &id_i);
					built = true;
				}
				
				void vbo::organize() {
					bytes = bytes_dyn = bytes_str = 0;
					
//					for(int i = 0; i < meshes.size(); ++i) {
						//bytes += meshes[i]->b_static.get_bytes();
						//bytes_dyn += meshes[i]->b_dynamic.get_bytes();
						//bytes_str += meshes[i]->b_stream.get_bytes();
						//meshes[i]->b_static.pt = meshes[i]->b_dynamic.pt = meshes[i]->b_stream.pt = this;
				//	}
					
					if(bytes > 0) {
						current_array = id;
						glBindBuffer(GL_ARRAY_BUFFER, id);
						glBufferData(GL_ARRAY_BUFFER, bytes, NULL, GL_STATIC_DRAW);
					}
					
					if(bytes_dyn > 0) {
						current_array = id_dyn;
						glBindBuffer(GL_ARRAY_BUFFER, id_dyn);
						glBufferData(GL_ARRAY_BUFFER, bytes_dyn, NULL, GL_DYNAMIC_DRAW);
					}

					if(bytes_str > 0) {
						current_array = id_str;
						glBindBuffer(GL_ARRAY_BUFFER, id_str);
						glBufferData(GL_ARRAY_BUFFER, bytes_str, NULL, GL_STREAM_DRAW);
					}

					//std::sort(meshes.begin(), meshes.end());
				}

				void vbo::close() {
					if(built) {
						glDeleteBuffers(1, &id);
						glDeleteBuffers(1, &id_dyn);
						glDeleteBuffers(1, &id_str);
						glDeleteBuffers(1, &id_i);
						built = false;
					}
				}

				/*
				vertex_buffer::component::component(int* ptr, int size, int stride) {set(ptr, size, stride); }
				vertex_buffer::component::component(short* ptr, int size, int stride) {set(ptr, size, stride); }
				vertex_buffer::component::component(float* ptr, int size, int stride) {set(ptr, size, stride); }
				vertex_buffer::component::component(double* ptr, int size, int stride) {set(ptr, size, stride); }
				
				void vertex_buffer::component::set(int* p, int s, int st) {
					pointer = (int*)p; size = s; stride = st; type = GL_INT;
				}

				void vertex_buffer::component::set(short* p, int s, int st) {
					pointer = (short*)p; size = s; stride = st; type = GL_SHORT;
				}

				void vertex_buffer::component::set(float* p, int s, int st) {
					pointer = (float*)p; size = s; stride = st; type = GL_FLOAT;
				}

				void vertex_buffer::component::set(double* p, int s, int st) {
					pointer = (double*)p; size = s; stride = st; type = GL_DOUBLE;
				}

				std::vector<vertex_buffer::component>& vertex_buffer::get_components() {
					return comps;
				}*/

			}
		}
	}
}