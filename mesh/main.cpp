// mesh.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <IL\il.h>
#include <IL\ilu.h>
#include <IL\ilut.h>
#include "glut.h"
#include "mesh.h"
#include "raytracer.h"
#include "image.h"

using namespace std;

void display();
void reshape(int, int);
void ogl();
void img();

Scene scene;
Image2D image, image2, image3, image4;

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitWindowPosition(10,10);
	glutInitWindowSize(800, 600);
	glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE| GLUT_DEPTH);
	glutCreateWindow("Ray Tracer");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	ilInit();
	scene.init();

	image.load("marble2.jpg");
	image2.load("marble.bmp");
	image3.load("marble3.jpg");
	image4.load("leather.jpg");

	Object* obj1 = new Object;
	obj1->initMaterial(Vector3D(0.5, 0.5, 0.5), Vector3D(0.9, 0.9, 0.9), Vector3D(0.9, 0.9, 0.9));
	obj1->load("ball.obj");
	obj1->normalize();
	obj1->scale(Vector3D(1.0, 1.0, 1.0));
	obj1->translate(Vector3D(-1.0, -0.8, -1.0));
	obj1->addTexture(&image);
	obj1->setShadingMode(SMOOTHED);
	scene.addObj(*obj1);

	Object* obj4 = new Object;
	obj4->initMaterial(Vector3D(0.5, 0.5, 0.5), Vector3D(0.9, 0.9, 0.9), Vector3D(0.9, 0.9, 0.9));
	obj4->load("ball.obj");
	obj4->normalize();
	obj4->scale(Vector3D(1.0, 1.0, 1.0));
	obj4->translate(Vector3D(0.0, -0.8, -1.0));
	obj4->addTexture(&image2);
	obj4->setShadingMode(SMOOTHED);
	scene.addObj(*obj4);

	Object* obj2 = new Object;
	obj2->initMaterial(Vector3D(0.3, 0.3, 0.3), Vector3D(0.7, 0.7, 0.7), Vector3D(0.9, 0.9, 0.9));
	obj2->load("ball.obj");
	obj2->normalize();
	obj2->scale(Vector3D(1.0, 1.0, 1.0));
	obj2->translate(Vector3D(1.0, -0.8, -1.0));
	obj2->addTexture(&image3);
	obj2->setShadingMode(SMOOTHED);
	scene.addObj(*obj2);

	Object* obj3 = new Object;
	obj3->initMaterial(Vector3D(0.3, 0.3, 0.3), Vector3D(0.7, 0.7, 0.7), Vector3D(0.9, 0.9, 0.9));
	obj3->load("ball.obj");
	obj3->normalize();
	obj3->scale(Vector3D(1.0, 1.0, 1.0));
	obj3->translate(Vector3D(0.0, 0.3, -1.0));
	obj3->addTexture(&image4);
	obj3->setShadingMode(SMOOTHED);
	scene.addObj(*obj3);

	glutMainLoop();
	return 0;
}

void img() {
	uint h = image.mheight;
	uint w = image.mwidth;

	glBegin(GL_POINTS);
	for (uint i = 0; i < h; i++) {
		for (uint j = 0; j < w; j++) {
			uint k = i * w + j;  // k-th pixel on the image 
			glColor3f(image.mdata[3 * k] / 255.0, image.mdata[3 * k + 1] / 255.0, image.mdata[3 * k + 2] / 255.0);
			glVertex3f(j * 1.0 / w, 1.0 - i * 1.0 / h, 0.0);
		}
	}
	glEnd();
	//glFlush();
}

void ogl() {
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	for (uint i = 0; i < scene.mObjects.size(); i++) {
		for (uint j = 0; j < scene.mObjects[i].mNumMeshes; j++) {
			for (uint k = 0; k < scene.mObjects[i].mMeshes[j]->mNumFaces; k++) {
				glVertex3f(scene.mObjects[i].mMeshes[j]->mVertices[scene.mObjects[i].mMeshes[j]->mFaces[k].mIndices[0]].mx, 
						   scene.mObjects[i].mMeshes[j]->mVertices[scene.mObjects[i].mMeshes[j]->mFaces[k].mIndices[0]].my, 
						   scene.mObjects[i].mMeshes[j]->mVertices[scene.mObjects[i].mMeshes[j]->mFaces[k].mIndices[0]].mz); 

				glVertex3f(scene.mObjects[i].mMeshes[j]->mVertices[scene.mObjects[i].mMeshes[j]->mFaces[k].mIndices[1]].mx, 
						   scene.mObjects[i].mMeshes[j]->mVertices[scene.mObjects[i].mMeshes[j]->mFaces[k].mIndices[1]].my, 
						   scene.mObjects[i].mMeshes[j]->mVertices[scene.mObjects[i].mMeshes[j]->mFaces[k].mIndices[1]].mz); 

				glVertex3f(scene.mObjects[i].mMeshes[j]->mVertices[scene.mObjects[i].mMeshes[j]->mFaces[k].mIndices[1]].mx, 
						   scene.mObjects[i].mMeshes[j]->mVertices[scene.mObjects[i].mMeshes[j]->mFaces[k].mIndices[1]].my, 
						   scene.mObjects[i].mMeshes[j]->mVertices[scene.mObjects[i].mMeshes[j]->mFaces[k].mIndices[1]].mz);

				glVertex3f(scene.mObjects[i].mMeshes[j]->mVertices[scene.mObjects[i].mMeshes[j]->mFaces[k].mIndices[2]].mx, 
						   scene.mObjects[i].mMeshes[j]->mVertices[scene.mObjects[i].mMeshes[j]->mFaces[k].mIndices[2]].my, 
						   scene.mObjects[i].mMeshes[j]->mVertices[scene.mObjects[i].mMeshes[j]->mFaces[k].mIndices[2]].mz); 

				glVertex3f(scene.mObjects[i].mMeshes[j]->mVertices[scene.mObjects[i].mMeshes[j]->mFaces[k].mIndices[2]].mx, 
					       scene.mObjects[i].mMeshes[j]->mVertices[scene.mObjects[i].mMeshes[j]->mFaces[k].mIndices[2]].my, 
						   scene.mObjects[i].mMeshes[j]->mVertices[scene.mObjects[i].mMeshes[j]->mFaces[k].mIndices[2]].mz); 

				glVertex3f(scene.mObjects[i].mMeshes[j]->mVertices[scene.mObjects[i].mMeshes[j]->mFaces[k].mIndices[0]].mx, 
						   scene.mObjects[i].mMeshes[j]->mVertices[scene.mObjects[i].mMeshes[j]->mFaces[k].mIndices[0]].my, 
						   scene.mObjects[i].mMeshes[j]->mVertices[scene.mObjects[i].mMeshes[j]->mFaces[k].mIndices[0]].mz); 
			}
		}
	}
	glEnd();
}

void display() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	float delt_x = scene.mScreen.mWidth / scene.mScreen.mNumPixels;
	float delt_y = scene.mScreen.mHeight / scene.mScreen.mNumScanLines;

	int numPixels = scene.mScreen.mNumPixels;
	int numScanLines = scene.mScreen.mNumScanLines;

	float xLeft = -0.5 * scene.mScreen.mWidth;
	float yTop  =  0.5 * scene.mScreen.mHeight;

	Vector3D global;
	Vector3D pixel;
	Ray      ray;
	intersectionAttr attr;

	for (int i = 0; i < numScanLines; i++) {
		for (int j = 0; j < numPixels; j++) {
			pixel = Vector3D(xLeft + j * delt_x, yTop - i * delt_y, scene.mScreen.mz);
			ray = scene.genRay(pixel);
			global = scene.rayTracer(0, ray, attr);

			glColor3f(global.mx, global.my, global.mz);
			glBegin(GL_POINTS);
			glVertex3f(pixel.mx, pixel.my, pixel.mz);
			glEnd();
		}
	}
	//ogl();
	//img();
	glFlush();
}

void reshape (int w, int h) {
	if(h == 0) h = 1;
	float ratio = 1.0 * w / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glViewport(0, 0, w, h);

	gluPerspective(35, ratio, 1.0, 8.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(scene.mCamera.mx, scene.mCamera.my, scene.mCamera.mz,
		      0.0, 0.0, 0.0,     
			  0.0f,1.0f,0.0f);
}

