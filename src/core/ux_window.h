#ifndef __UX_WINSOW__H__
#define __UX_WINSOW__H__

#include "environment.h"
#include "ux_bitmap.h"

struct pixel {
	union {
		struct {
			unsigned char b, g, r, a;
		};
		int val;
	};
	pixel() {
		val = 0;
	}
};

class UxWindow;

typedef void (*ON_FRAME)(UxWindow* window);

class UxWindow {
	public:
		UxWindow(Environment& environment, const char *name, int _width, int _height, ON_FRAME _onFrame);
		
		void show() const ;
		
		void create();
		void paint();
		void close();
		
		pixel *getPixels() const;
		HBITMAP getHBitmap() const;		
		UxBitmap& getBitmap();
		
		~UxWindow();
		
		const HWND getHwnd() const;
		
		void setHandlers(HDC _hdc, HDC _hdcMem, HBITMAP _hbmOld);
		
		void run();
		
		int getWidth() const;
		int getHeight() const;
		
	private:
		void makeSurface();
		
		int width, height;
		
		ON_FRAME onFrame;

		HWND hwnd;
		HBITMAP hbmp;
		pixel* pixels;
		UxBitmap *bitmap;
		HDC hdcMem;
		HDC hdc;
		HBITMAP hbmOld;
		HANDLE hTickThread;
		
		Environment& environment;
};

#endif