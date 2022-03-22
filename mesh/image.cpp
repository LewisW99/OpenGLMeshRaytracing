#include "stdafx.h"
#include <IL\il.h>
#include <IL\ilu.h>
#include <IL\ilut.h>
#include "image.h"
#include "mesh.h"

Image2D::Image2D(const char* filename) {
	mdata = NULL;
	load(filename);
}

void Image2D::load(const char* filename) {
	uint imageID; 
	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	if (ilLoadImage((const wchar_t*)filename)) {
		//ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		mheight = ilGetInteger(IL_IMAGE_HEIGHT);
		mwidth  = ilGetInteger(IL_IMAGE_WIDTH);
		mdata   = new uchar[mwidth * mheight * 3 * sizeof(unsigned char)]; // multiply by 3 because we want 3 components per pixel
		ilCopyPixels(0, 0, 0, mwidth, mheight, 1, IL_RGB, IL_UNSIGNED_BYTE, mdata);
	} else {
		std::cout << "Image load failed." << std::endl;
		exit(-1);
	}

	ilDeleteImages(1, &imageID); 
}

Vector3D Image2D::texture(double u, double v) {
// to-do:
// by multiplying u, v with the corresponding width/height of an image
// we get the coordinate of the texel in the image.
// i.e., texel (i, j) = (u * w, v * h)

// by doing linear interpolation on the 4 texels sorrounding (i, j) we obtain the final colour of (i, j), 
// this is so called linear filtering

	// i-th col, j-th row
	uint i = (uint) (u * mwidth);
	uint j = (uint) ((1.0 - v) * mheight);
	uint k = j * mwidth + i;

	double r = mdata[3 * k] / 255.0;
	double g = mdata[3 * k + 1] / 255.0;
	double b = mdata[3 * k + 2] / 255.0;

	return Vector3D(r, g, b);
}