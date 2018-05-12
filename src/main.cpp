#include <windows.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdio>
#include "threads/ux_threads.h"
#include "core/display.h"
#include "core/environment.h"
#include "core/ux_window.h"
#include "core/ux_bitmap.h"

const int w_width = 375;
const int w_height = 375;

UxBitmap* xBitmap;
int xPos = 50;
int yPos = 50;

int xSpeed = 2;
int ySpeed = 3;


void onFrame(UxWindow* uxWindow) {
	pixel *pixels = (pixel*)uxWindow->getBitmap().asUInt32();
	pixel *p;
	// +0.005 each frame
	static float frameOffset = 0;
	float px; // % of the way across the bitmap
	float py; // % of the way down the bitmap
	int width  = uxWindow->getWidth();
	int height = uxWindow->getHeight();
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			p = &pixels[y * width + x];
			px = float(x) / float(width);
			py = float(y) / float(height);
			p->r = (unsigned char)(((cos(px + frameOffset * 10) / sin(py + frameOffset)) * cos(frameOffset * 3) * 10) * 127 + 127);
			p->g = ~p->r;
			p->b = 255;
		}
	}
	frameOffset += 0.005f;
	
	xPos += xSpeed;
	yPos += ySpeed;
	
	if (xPos + xBitmap->getWidth() >= uxWindow->getWidth() || xPos<0) {
		xSpeed = -xSpeed;
		xPos += xSpeed;
	}
	
	if (yPos + xBitmap->getHeight() >= uxWindow->getHeight() || yPos<0) {
		ySpeed = -ySpeed;
		yPos += ySpeed;
	}
	
	uxWindow->getBitmap().drawBitmap(xBitmap, xPos, yPos);
}


void onFrame2(UxWindow* uxWindow) {
	pixel *pixels = (pixel*)uxWindow->getBitmap().asUInt32();
	pixel *p;
	// +0.005 each frame
	static float frameOffset = 0;
	float px; // % of the way across the bitmap
	float py; // % of the way down the bitmap
	int width  = uxWindow->getWidth();
	int height = uxWindow->getHeight();
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			p = &pixels[y * width + x];
			px = float(x) / float(width);
			py = float(y) / float(height);
			p->b = (unsigned char)(((cos(px + frameOffset * 10) / sin(py + frameOffset)) * cos(frameOffset * 3) * 10) * 127 + 127);
			p->g = ~p->r;
			p->r = 255;
		}
	}
	frameOffset += 0.005f;
}


class TestTask : public Runnable {
public:
	virtual void run() {
		std::cout<<"TASK RUNNING"<<std::endl;			
		std::cout<<"TASK DONE"<<std::endl;
	}
};

void init() {
	TestTask testTask1;
	TestTask testTask2;
	TestTask testTask3;
	
	ThreadPool thPool(20);	
	thPool.pushTask(&testTask1);
	thPool.pushTask(&testTask2);
	thPool.pushTask(&testTask3);
	
	testTask1.wait();
	testTask2.wait();
	testTask3.wait();
	
	xBitmap = UxBitmap::loadBmp("test.bmp");

	UxWindow* uxWindow1 = new UxWindow(mainEnvironment, "Test 1", w_width, w_height, onFrame);
	uxWindow1->show();
	
	UxWindow* uxWindow2 = new UxWindow(mainEnvironment, "Test 2", w_width, w_height, onFrame);
	uxWindow2->show();

	UxWindow* uxWindow3 = new UxWindow(mainEnvironment, "Test 3", w_width, w_height, onFrame2);
	uxWindow3->show();
	
	UxBitmap *uxBitmap = new UxBitmap(800, 600, ARGB_8888);
	delete uxBitmap;
	
}

START_PROC(init())



