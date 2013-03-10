#pragma once
#include "pixel.h"
db::graphics::pixel_8::pixel_8(color brightness) : r(brightness) {}
db::graphics::pixel_8::operator unsigned char() {return r;}
db::graphics::pixel_8& db::graphics::pixel_8::operator=(color brightness) { r = brightness; return *this; } 

db::graphics::pixel_24::pixel_24(color red, color green, color blue) : r(red), g(green), b(blue) {}
db::graphics::pixel_32::pixel_32(color red, color green, color blue, color alpha) : r(red), g(green), b(blue), a(alpha) {}