#pragma once
#include "../Graphics/vertex.h"

struct RGBA_256{
    unsigned char r,g,b,alpha;
    RGBA_256(unsigned char, unsigned char, unsigned char, unsigned char aa = 255);
};

struct RGBA_1
{
    float r,g,b,alpha;

    RGBA_1(float rr = 1.0, float gg = 1.0, float bb = 1.0, float aa = 1.0);

    RGBA_1(RGBA_256 aa);

    int operator!=(RGBA_1 c);
    operator RGBA_256();
    int operator!=(RGBA_256 d);
	operator vec4();
};
