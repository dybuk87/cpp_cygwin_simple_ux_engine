# cpp_cygwin_simple_ux_engine
Simple light weight engine for app compiled in cygwin environment. 

Example source code in main.cpp 
![Alt text](/screen.png?raw=true "Screen")

Example app create 3 windows with animation.

# How To use It 

## 1. Define startpoint 

```
  START_PROC(init())
```
This will start winapi mainloop and execute code from init() function


## 2. Define init() function

```
void init() {
  ...
  UxWindow* uxWindow1 = new UxWindow(mainEnvironment, "Test 1", w_width, w_height, onFrame);
	uxWindow1->show();  
  ...
}
```
This will create new window with dimension w_width x w_height. onFrame is method that will execute every frame

## 3. Define onFrame method:

```
void onFrame(UxWindow* uxWindow) {
	pixel *pixels = (pixel*)uxWindow->getBitmap().asUInt32();
	pixel *p;	
	static float frameOffset = 0; // +0.005 each frame
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
```
This code will be executed on each frame, pixels is direct access to screen bitmap.

## 4. Bmp loading and bitmaps

You can load 24 bti color bmp with simple one line:
uxBitmap = UxBitmap::loadBmp("test.bmp");

You can also create bitmap in memory using this code:
uxBitmap = new UxBitmap(800, 600, ARGB_8888); // create bitmap size 800x600 x 32 bits

To access raw pixels : 
pixel *pixels = uxBitmap.asUInt32();

draw bitmap to other bitmap :
destinationBitmap.drawBitmap(sourceBitmap, destinationX, destinationY);

## More function

There is few more method and classes for support monitor access, threads and bitmaps, explore source code






