#ifndef TEXTURE_H
#define TEXTURE_H
#include <iostream>

typedef unsigned int  uint;
typedef unsigned char uchar;

class Vector3D;

class Image2D {
public:
	Image2D(const char* filename); 
	Image2D() {}

	void     load(const char* filename);
	Vector3D texture(double u, double v); // return (r,g,b) at (u,v)

	uchar* mdata;
	uint   mheight;
	uint   mwidth;
};
#endif
