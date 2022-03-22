#ifndef MESH_H
#define MESH_H

#include <iostream>

/* 
* Assimp libray is used here to load geomtry models, hence classes defined here are very similar to
* Assimp, and could be viewed as simplified version of Assimp classes.
*/

typedef unsigned int uint;
struct Ray;
struct intersectionAttr;
class  Image2D;

enum SHADINGMODE {SMOOTHED = 0, FLAT};

class Vector3D {
public:
	Vector3D() {}
	Vector3D(double x, double y, double z) 
		: mx (x), my (y), mz (z) 
	{}
	Vector3D(const Vector3D& v) {
		mx = v.mx;
		my = v.my;
		mz = v.mz;
	}
	~Vector3D() {}

	double   length();
	void     normalization();
	void     operator =  (const Vector3D& v);
	double&  operator [] (unsigned i);
	Vector3D operator *  (double k);
	Vector3D operator *  (const Vector3D& v);
	void     operator *= (double k);
	void     operator *= (const Vector3D& v);
	void     operator -= (const Vector3D& v);
	void     operator += (const Vector3D& v);

	double mx, my, mz;
};

// non-member functions related to Vector3D
double   dot  (const Vector3D& v1, const Vector3D& v2);
Vector3D cross(const Vector3D& v1, const Vector3D& v2);
Vector3D operator+ (const Vector3D& v1, const Vector3D& v2);
Vector3D operator- (const Vector3D& v1, const Vector3D& v2);

class Face {
public:
	Face()  {}
	~Face() {}

	uint mIndices[3]; // indice array for 3 vertices
};

class Mesh {
public:
	Mesh()  {
		mVertices = NULL;
		mNormals  = NULL;
		mFaces    = NULL;
		mTexImage = NULL;
		mTextured = false;
	}
	~Mesh() {}

	Vector3D faceNormal(uint faceInd); 
	Vector3D getTexel(double u, double v);
	bool     rayTriangleIntersection(const Ray& ray, intersectionAttr& trianglePoint, SHADINGMODE sm); 

	bool hasNormals() { 
		if (mNormals == NULL) return false;
		return true;
	}
	bool hasTexture() {
		return mTextured;
	}
	uint mNumVertices;
	uint mNumFaces;

	Vector3D *mVertices;  // vertex array
	Vector3D *mNormals;   // vertex normal array by assimp 
	Vector3D *mUVCoords;  // vertex UV array
	Face     *mFaces;     // triangle array
	Image2D  *mTexImage;  // pointer to image/texture
	bool      mTextured;
};

class Object {
public:
	Object()  {
		mMeshes = NULL;
		mShadingMode = SMOOTHED; 
	}
	~Object() {}

	struct materialAttr {
		Vector3D ka;
		Vector3D kd;
		Vector3D ks;
	};

	void setShadingMode(SHADINGMODE sm) {
		mShadingMode = sm;
	}
	SHADINGMODE shadingMode() {
		return mShadingMode;
	}

	void load(const std::string& filename);
	bool isIntersected(const Ray& ray, intersectionAttr& at);
	void initMaterial(const Vector3D& ka, const Vector3D& kd, const Vector3D& ks);
	void addTexture(Image2D* img);
	void removeTexture();
	void normalize();
	void translate(const Vector3D& t);
	void rotate(double angle, const Vector3D& axis);
	void scale(const Vector3D& s);

	materialAttr mMaterialAttr;
	uint mNumMeshes;  
	Mesh **mMeshes; 
	double mR;          // radius of the bounding sphere
	Vector3D mCentre;

private:
	SHADINGMODE mShadingMode;   // flat or smoothed Vector3D mCentre;   // centre of the bounding shpere
};

#endif
