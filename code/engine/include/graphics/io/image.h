#pragma once
#include "../../math/math.h"
/*
todo:
* ostateczne rozwiazanie kwestii rejestru, podzielic po prostu na link/object ale na razie to nie moj concern
* jakies tam secure blity
*/

namespace db {
	namespace graphics {
		namespace io {
			class image {
				friend class image_file;
				unsigned char* data;
				vector<unsigned char> v;
				math::rect_wh size;
				bool allocated, file;
				int channels;
				void _zero();
			public:
				image();
				virtual ~image();

				// set
				image(unsigned char* ptr, int channels, const math::rect_wh& size);
				
				image(const image&);
				image& operator=(const image&);
				
				void set(unsigned char* ptr, int channels, const math::rect_wh& size);
				void set(const image&);

				void create	  (int w, int h, int channels);
			    bool from_file(const char* filename, unsigned channels = 0),
				     from_clipboard();

				void fill(unsigned char val), 
					 fill(unsigned char* channel_vals),
					 fill_channel(int channel, unsigned char val);

				void copy(const db::graphics::io::image&);
				void copy(unsigned char* ptr, int channels, int pitch, const math::rect_wh& size);
				
				bool secure_blit (const image&, int x, int y, const math::rect_xywhf& src); // not implemented
				void blit		 (const image&, int x, int y, const math::rect_xywhf& src, bool luminance_to_alpha = false);
				void blit_channel(const image&, int x, int y, const math::rect_xywhf& src, int channel, int src_channel);
				bool secure_blit_channel(const image&, int x, int y, const math::rect_xywhf& src, int channel, int src_channel); // not implemented

				unsigned char* operator()(int x, int y, int channel = 0); // get pixel
				unsigned char* ptr(int x = 0, int y = 0, int channel = 0);
				unsigned char  pix(int x = 0, int y = 0, int channel = 0) const;

				int get_channels() const, get_bytes() const, get_num_pixels() const;
				const math::rect_wh& get_size() const;

				void destroy(bool free_memory = false);
			};

			//class image_file : public image, public db::misc::container::registry<image_file> {
			//	const char* filename;
			//	int force_channels;
			//public:
			//	bool opened, operator==(const image_file&);
			//	image_file&  operator= (const image_file&); // set

			//	image_file();
			//	void open(const char* filename, unsigned force_channels = 0), destroy();
			//	bool is_open();
			//	
			//	~image_file();
			//};

		}
	}
}