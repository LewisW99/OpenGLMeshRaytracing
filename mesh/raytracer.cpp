#include "stdafx.h"
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "raytracer.h"
#include "image.h"
#include "mesh.h"

const double LARGE_NUM = 100000.0;

void Scene::addObj(const Object& obj) {
	mObjects.push_back(obj);
}

Ray Scene::genRay(const Vector3D& pixel) {
	return Ray(mCamera, pixel);
}

Vector3D Scene::renderer(const intersectionAttr& attr) {
	Vector3D normal;
	if (mObjects[attr.objInd].shadingMode() == FLAT) 
		normal = mObjects[attr.objInd].mMeshes[attr.meshInd]->faceNormal(attr.faceInd);
	else
		normal = attr.normal;

	Vector3D L = mLight.mPos - attr.closestP;
	L.normalization();

	Vector3D V = mCamera - attr.closestP;
	V.normalization();

	Vector3D H = L + V;
	H.normalization();

	double LdotN = dot(L, normal);
	double HdotN = max(dot(H, normal), 0.0);
	if (HdotN == 0.0) HdotN = 1.0;
	double HdotN_n = pow(HdotN, (float)250.0);

	Vector3D c(0.0, 0.0, 0.0);

	c = mLight.mAmbient * mObjects[attr.objInd].mMaterialAttr.ka; 
	if (LdotN > 0.0) {
		c += mLight.mI * mObjects[attr.objInd].mMaterialAttr.kd * LdotN + 
			 mLight.mI * mObjects[attr.objInd].mMaterialAttr.ks * HdotN_n;
	}

	if(mObjects[attr.objInd].mMeshes[attr.meshInd]->hasTexture()) {
		Vector3D textureColour = this->mObjects[attr.objInd].mMeshes[attr.meshInd]->getTexel(attr.u, attr.v); 
		c *= textureColour;
	}

	return c;
}

Vector3D Scene::rayTracer(int depth, const Ray& ray, intersectionAttr& attr) {
	const int maxDepth = 2;
	if (depth > maxDepth)
		return Vector3D(0.0, 0.0, 0.0);
	else {
		attr.closestT = LARGE_NUM;
		if (intersection(ray, attr)) {
			Vector3D localColour = renderer(attr);
			Ray reflectedRay = genReflectedRay(ray, attr);

			Vector3D envirColour = rayTracer(depth + 1, reflectedRay, attr); 
			return Vector3D(localColour + envirColour * 0.6); 
		}
		else {
			return Vector3D(0.0, 0.0, 0.0);
		}
	}
}

Ray Scene::genReflectedRay(const Ray& ray, const intersectionAttr& attr) {
	//Vector3D N = mObjects[attr.objInd].mMeshes[attr.meshInd]->faceNormal(attr.faceInd); 
	Vector3D N = attr.normal;

	Vector3D R = ray.mP1 - ray.mP2;
	R.normalization();

	double   R_dot_N = dot(R, N);
	Vector3D knormal = N * (2.0 * R_dot_N);
	Vector3D F = knormal - R;

	Ray reflectedRay;
	reflectedRay.mP2 = F + attr.closestP;
	reflectedRay.mP1 = attr.closestP;

	return reflectedRay; 
}

bool Scene::intersection(const Ray& ray, intersectionAttr& attr) {
	bool isIntersected = false;
	uint size = mObjects.size();

	intersectionAttr tmpAttr;
	tmpAttr.closestT = LARGE_NUM;

	for (uint index = 0; index < size; index++) {
		tmpAttr.objInd = index;
		if (mObjects[index].isIntersected(ray, tmpAttr)) {
			isIntersected = true;
			if(tmpAttr.closestT < attr.closestT) {
				attr = tmpAttr;
				//attr.objInd = index;
			}
		}
	}
	return isIntersected;
}

void Scene::init() {
	mLight.mI       = Vector3D(1.0, 1.0, 1.0);
	mLight.mPos     = Vector3D(0.0, 1.0, 9.0);
	mLight.mAmbient = Vector3D(0.3, 0.3, 0.3);

	mCamera.mx = 0.0;
	mCamera.my = 0.0;
	mCamera.mz = 5.0;

	mScreen.mHeight = 1.2f;
	mScreen.mWidth  = 1.2f;
	mScreen.mz      = 3.5;

	mScreen.mNumPixels    = 800;
	mScreen.mNumScanLines = 800;
}

