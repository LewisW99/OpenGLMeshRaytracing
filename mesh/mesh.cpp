#include "stdafx.h"
#include <assimp/Importer.hpp>
#include <assimp/PostProcess.h>
#include <assimp/Scene.h>
#include "mesh.h"
#include "raytracer.h"
#include "image.h"

const double LARGE_NUM = 10000.0;

double & Vector3D::operator[] (unsigned i) {
	if (i == 0) return mx;
	if (i == 1) return my;
	return mz;
}

void Vector3D::operator= (const Vector3D& v) {
	mx = v.mx;
	my = v.my;
	mz = v.mz;
}

Vector3D Vector3D::operator* (double k) {
	return Vector3D(mx * k, my * k, mz * k);
}

Vector3D Vector3D::operator *  (const Vector3D& v) {
	return Vector3D(mx * v.mx, my * v.my, mz * v.mz);
}

void Vector3D::operator*= (double k) {
	mx *= k;
	my *= k;
	mz *= k;
}

void Vector3D::operator*= (const Vector3D& v) {
	mx *= v.mx;
	my *= v.my;
	mz *= v.mz;
}

void Vector3D::operator -= (const Vector3D& v) {
	mx -= v.mx;
	my -= v.my;
	mz -= v.mz;
}

void Vector3D::operator += (const Vector3D& v) {
	mx += v.mx;
	my += v.my;
	mz += v.mz;
}

double Vector3D::length() {
	return sqrt(mx * mx + my * my + mz * mz);
}

void Vector3D::normalization() {
	double d = 1.0 / length();  // suppose len != 0
	mx *= d;
	my *= d;
	mz *= d;
}

double dot(const Vector3D& v1, const Vector3D& v2) {
	return v1.mx * v2.mx + v1.my * v2.my + v1.mz * v2.mz;
}

Vector3D cross(const Vector3D& v1, const Vector3D& v2) {
	double x = v1.my * v2.mz - v1.mz * v2.my;
	double y = v1.mz * v2.mx - v1.mx * v2.mz;
	double z = v1.mx * v2.my - v1.my * v2.mx;
	return Vector3D(x, y, z);
}

Vector3D operator+ (const Vector3D& v1, const Vector3D& v2) {
	return Vector3D(v1.mx + v2.mx, v1.my + v2.my, v1.mz + v2.mz);
}

Vector3D operator- (const Vector3D& v1, const Vector3D& v2) {
	return Vector3D(v1.mx - v2.mx, v1.my - v2.my, v1.mz - v2.mz);
}

Vector3D Mesh::faceNormal(uint faceInd) {
	uint ind0 = mFaces[faceInd].mIndices[0];
	uint ind1 = mFaces[faceInd].mIndices[1];
	uint ind2 = mFaces[faceInd].mIndices[2];
	Vector3D v01 = mVertices[ind1] - mVertices[ind0];
	Vector3D v02 = mVertices[ind2] - mVertices[ind0];
	Vector3D n = cross(v01, v02); // indices in anti-clockwise order, v01 x v02 pointing to outer space
	n.normalization();  
	return n;
}

Vector3D Mesh::getTexel(double u, double v) {
	return mTexImage->texture(u, v);
}

bool Mesh::rayTriangleIntersection(const Ray& ray, intersectionAttr& trianglePoint, SHADINGMODE sm) { 
	uint faceInd = trianglePoint.faceInd;

	uint ind0 = mFaces[faceInd].mIndices[0];
	uint ind1 = mFaces[faceInd].mIndices[1];
	uint ind2 = mFaces[faceInd].mIndices[2];
	Vector3D a = mVertices[ind0];
	Vector3D b = mVertices[ind1];
	Vector3D c = mVertices[ind2];

	Vector3D N = faceNormal(faceInd);
	Vector3D e = ray.mP2 - ray.mP1;
	e.normalization();

	// ray parallel to the triangle 
	if (abs(dot(N, e)) <= 0.0000001) {
		return false; 
	}

	double t = (dot(N, a) - dot(N, ray.mP1)) / dot (N, e);

	// intersected at the origion of ray or behind it
	if (t <= 0.01) {
		return false;
	}

	Vector3D p = ray.mP1 + e * t;

	// Compute vectors        
	Vector3D v0 = c - a;
	Vector3D v1 = b - a;
	Vector3D v2 = p - a;

	// Compute dot products
	double dot00 = dot(v0, v0); 
	double dot01 = dot(v0, v1); 
	double dot02 = dot(v0, v2);
	double dot11 = dot(v1, v1);
	double dot12 = dot(v1, v2);

	double invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
	double v = (dot11 * dot02 - dot01 * dot12) * invDenom;
	double u = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	bool isIntersected = (u >= 0) && (v >= 0) && (u + v < 1); 

	// interpolate vertices normal vectors to obtain normal for intersection point
	double m = 1.0 - u - v;
	if (isIntersected) {
		if (sm == SMOOTHED) {
			Vector3D Na = mNormals[ind0];
			Vector3D Nc = mNormals[ind1];
			Vector3D Nb = mNormals[ind2];
			trianglePoint.normal = Na * m + Nb * v + Nc * u; 
			trianglePoint.normal.normalization();
		}
		else
			trianglePoint.normal = N;

		if (hasTexture()) {
			Vector3D Ta = mUVCoords[ind0];
			Vector3D Tb = mUVCoords[ind1];
			Vector3D Tc = mUVCoords[ind2];
			trianglePoint.u = m * Ta[0] + u * Tb[0] + v * Tc[0];
			trianglePoint.v = m * Ta[1] + u * Tb[1] + v * Tc[1];
		}

		trianglePoint.closestP = p;
		trianglePoint.closestT = t;
	}
	return isIntersected; 
}

void Object::load(const std::string& filename) {
	Assimp::Importer aiImporter;
	const aiScene* aiScene = aiImporter.ReadFile(filename, 
		aiProcessPreset_TargetRealtime_Quality);
	if(!aiScene) {
		std::cout << aiImporter.GetErrorString() << std::endl;
		exit(-1);
	}

	mNumMeshes = aiScene->mNumMeshes;
	mMeshes = new Mesh*[mNumMeshes];

	for (uint i = 0; i < mNumMeshes; i++) {
		mMeshes[i] = new Mesh;
		aiMesh* 
			aiMesh = aiScene->mMeshes[i]; 

		mMeshes[i]->mNumFaces    = aiMesh->mNumFaces;
		mMeshes[i]->mNumVertices = aiMesh->mNumVertices;

		mMeshes[i]->mFaces = new Face[mMeshes[i]->mNumFaces];
		uint s = 0;
		for (uint j = 0; j < aiMesh->mNumFaces; j++) {
			if (aiMesh->mFaces[j].mNumIndices == 3) {
				for (uint k = 0; k < 3; k++) 
					mMeshes[i]->mFaces[s].mIndices[k] = aiMesh->mFaces[j].mIndices[k];
				s++;
			}
			else {
				mMeshes[i]->mNumFaces--;
				continue;
			}
		}

		mMeshes[i]->mVertices = new Vector3D[mMeshes[i]->mNumVertices];
		mMeshes[i]->mNormals  = new Vector3D[mMeshes[i]->mNumVertices];
		for (uint j = 0; j < aiMesh->mNumVertices; j++) {
			for (uint k = 0; k < 3; k++) {
				mMeshes[i]->mVertices[j][k] = aiMesh->mVertices[j][k]; 
				mMeshes[i]->mNormals[j][k] = aiMesh->mNormals[j][k];
			}
		}
		if (aiMesh->HasTextureCoords(0)) {
			//std::cout << "mesh has texture coords" << std::endl;
			mMeshes[i]->mUVCoords = new Vector3D[mMeshes[i]->mNumVertices];
			for (uint j = 0; j < aiMesh->mNumVertices; j++) {
				for (uint k = 0; k < 2; k++) {
					mMeshes[i]->mUVCoords[j][k] = aiMesh->mTextureCoords[0][j][k];
				}
			}
		}
	}
	std::cout << " ------------------------------------------------------------------" << std::endl;
	std::cout << "The geometry object " << filename << " is successfully loaded" << std::endl;
	std::cout << "The object has " << mNumMeshes << " mesh(es)" << std::endl;
	//for (uint i = 0; i < mNumMeshes; i++) {
	//	std::cout << "Mesh[" << i << "]:- " << std::endl;
	//	for (uint j = 0; j < mMeshes[i]->mNumVertices; j++) {
	//		std::cout << "         vertex " << j << " :" << mMeshes[i]->mVertices[j].mx << ", " << mMeshes[i]->mVertices[j].my << ", " << mMeshes[i]->mVertices[j].mz << std::endl;
	//		std::cout << "       texCoord " << j << " :" << mMeshes[i]->mUVCoords[j].mx << ", " << mMeshes[i]->mUVCoords[j].my << std::endl;

	//	}
	//	for (uint j = 0; j < mMeshes[i]->mNumFaces; j++) {
	//		std::cout << "           face " << j << " :" << mMeshes[i]->mFaces[j].mIndices[0] << ", " << mMeshes[i]->mFaces[j].mIndices[1] << ", " << mMeshes[i]->mFaces[j].mIndices[2] << std::endl; 

	//	}
	//}
}

bool Object::isIntersected(const Ray& ray, intersectionAttr& at) {
	bool isIntersected = false;

	// test against the bounding sphere
	Vector3D e = ray.mP2 - ray.mP1;
	e.normalization();

	Vector3D d = (ray.mP1 - mCentre) + 
		e * dot(e, mCentre - ray.mP1);
	if (d.length() > mR) 
		return isIntersected;

	intersectionAttr trianglePoint;
	for (uint i = 0; i < mNumMeshes; i++) {
		for (uint j = 0; j < mMeshes[i]->mNumFaces; j++) {
			trianglePoint.objInd  = at.objInd;
			trianglePoint.faceInd = j;
			trianglePoint.meshInd = i;
			if (mMeshes[i]->rayTriangleIntersection(ray, trianglePoint, shadingMode())) {
				isIntersected = true;
				if (trianglePoint.closestT < at.closestT) 
					at = trianglePoint;
			}
		}
	}
	return isIntersected;
}

void Object::initMaterial(const Vector3D& ka, const Vector3D& kd, const Vector3D& ks) {
	mMaterialAttr.ka = ka;
	mMaterialAttr.kd = kd;
	mMaterialAttr.ks = ks;
}

void Object::normalize() {
	double max_x = -LARGE_NUM; 
	double min_x = +LARGE_NUM; 
	double max_y = -LARGE_NUM;
	double min_y = +LARGE_NUM; 
	double max_z = -LARGE_NUM; 
	double min_z = +LARGE_NUM;

	for (uint i = 0; i < mNumMeshes; i++) {
		for (uint j = 0; j < mMeshes[i]->mNumVertices; j++) {
			if (mMeshes[i]->mVertices[j].mx > max_x) max_x = mMeshes[i]->mVertices[j].mx;
			if (mMeshes[i]->mVertices[j].mx < min_x) min_x = mMeshes[i]->mVertices[j].mx;
			if (mMeshes[i]->mVertices[j].my > max_y) max_y = mMeshes[i]->mVertices[j].my;
			if (mMeshes[i]->mVertices[j].my < min_y) min_y = mMeshes[i]->mVertices[j].my;
			if (mMeshes[i]->mVertices[j].mz > max_z) max_z = mMeshes[i]->mVertices[j].mz;
			if (mMeshes[i]->mVertices[j].mz < min_z) min_z = mMeshes[i]->mVertices[j].mz;
		}
	}

	double width  = max_x - min_x;
	double height = max_y - min_y;
	double depth  = max_z - min_z;
	double cx = (max_x + min_x) / 2.0;
	double cy = (max_y + min_y) / 2.0;
	double cz = (max_z + min_z) / 2.0;

	if (width  == 0.0) width  = 1.0;
	if (height == 0.0) height = 1.0;
	if (depth  == 0.0) depth  = 1.0;

	double sx = 1.0 / width;
	double sy = 1.0 / height;
	double sz = 1.0 / depth;

	for (uint i = 0; i < mNumMeshes; i++) {
		for (uint j = 0; j < mMeshes[i]->mNumVertices; j++) {
			mMeshes[i]->mVertices[j] -= Vector3D(cx, cy, cz);
			mMeshes[i]->mVertices[j] *= Vector3D(sx, sy, sz);
		}
	}
	mR = 0.5 * sqrt(3.0);
	mCentre = Vector3D(0.0, 0.0, 0.0);
}

void Object::translate(const Vector3D& t) {
	for (uint i = 0; i < mNumMeshes; i++) {
		for (uint j = 0; j < mMeshes[i]->mNumVertices; j++) {
			mMeshes[i]->mVertices[j] += t; 
		}
	}
	mCentre += t;
}

void Object::rotate(double angle, const Vector3D& axis) {
	// to-do
}

void Object::scale(const Vector3D& s) {
	for (uint i = 0; i < mNumMeshes; i++) {
		for (uint j = 0; j < mMeshes[i]->mNumVertices; j++) {
			mMeshes[i]->mVertices[j] *= s; 
		}
	}
	mR *= s.mx;
}

void Object::addTexture(Image2D* img) {
	for (uint i = 0; i <mNumMeshes; i++) {
		mMeshes[i]->mTextured = true;
		mMeshes[i]->mTexImage  = img;
	}
}

void Object::removeTexture() {
	for (uint i = 0; i <mNumMeshes; i++) {
		mMeshes[i]->mTextured = false;
		mMeshes[i]->mTexImage  = NULL;
	}
}

