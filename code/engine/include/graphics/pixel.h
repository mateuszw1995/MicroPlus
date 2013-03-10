#pragma once
namespace db {
	namespace graphics {
		typedef unsigned char color;

		struct pixel_8 {
			color r;
			pixel_8(color brightness = 255);
			operator color ();
			pixel_8& operator=(color);
		};

		struct pixel_24 {
			color r, g, b;
			pixel_24(color red = 255, color green = 255, color blue = 255);
		};

		struct pixel_32 {
			color r, g, b, a;
			pixel_32(color red = 255, color green = 255, color blue = 255, color alpha = 255);

		};
	}
}