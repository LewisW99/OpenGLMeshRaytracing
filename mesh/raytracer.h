#ifndef RAYTRACER_H
#define RAYTRACER_H
#include <vector>
#include "mesh.h"

struct Ray {
	Ray() {}
	Ray(const Vector3D& origin, const Vector3D& point) {
		mP1 = origin;
		mP2 = point;
	}
	Vector3D mP1;
	Vector3D mP2;
};

struct Light {
	Vector3D mPos;
	Vector3D mI;
	Vector3D mAmbient;
};

struct Screen {
	float mz;
	float mWidth;
	float mHeight;
	int   mNumPixels;
	int   mNumScanLines;
};

struct intersectionAttr {
	uint   objInd, meshInd, faceInd;
	double closestT;
	double u, v;      // (u,v) texture coord
	Vector3D normal;  // interpolated normal vector
	Vector3D closestP;
};

class Scene {
public:
	Scene()  {}
	~Scene() {}

	void     addObj(const Object& obj);
	Ray      genRay(const Vector3D& pixel);
	Ray      genReflectedRay(const Ray& ray, const intersectionAttr& attr);
	bool     intersection(const Ray& ray, intersectionAttr& attr);
	void     init();
	Vector3D rayTracer(int depth, const Ray& ray, intersectionAttr& attr);
	Vector3D renderer(const intersectionAttr& attr);

	std::vector<Object> mObjects;
	Light     mLight;
	Vector3D  mCamera;
	Screen    mScreen;
};

#endif
