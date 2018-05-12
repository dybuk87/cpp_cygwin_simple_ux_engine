#include <windows.h>
#include <vector>
#include <algorithm>
#include "ux_window.h"
#include "environment.h"

const int fps = 60;

template<class T>
class HasHwnd {
    HWND _hwnd;
public:
    HasHwnd(HWND hwnd) : _hwnd(hwnd) {}
    bool operator()(const T & o) const {		
        return o->getHwnd() == _hwnd;
    }
};

LRESULT CALLBACK WndProc2(
      HWND hwnd,
      UINT msg,
      WPARAM wParam,
      LPARAM lParam)
{
	UxWindow* window = const_cast<UxWindow*>(mainEnvironment.search(hwnd));
	
	if (window != 0) {
		switch ( msg ) {
			case WM_CREATE: {
				
				//return 0;
			} break;
			
			case WM_PAINT: {
				window->paint();
			//	return 0;
			} break;
			
			case WM_CLOSE: {
				window->close();
			//	return 0;
			} break;
			
			case WM_DESTROY: {								
			//	return 0;
			} break;
		}
	}

	return DefWindowProc( hwnd, msg, wParam, lParam );
}

DWORD WINAPI tickThreadProc(HANDLE handle) {
	UxWindow *uxWindow = reinterpret_cast<UxWindow*>(handle);

	// Give plenty of time for main thread to finish setting up
	Sleep( 50 );	
	HDC hdc = GetDC( uxWindow->getHwnd() );	
	HDC hdcMem = CreateCompatibleDC( hdc );
	HBITMAP hbmOld = (HBITMAP)SelectObject( hdcMem, uxWindow->getHBitmap());
	uxWindow->setHandlers(hdc, hdcMem, hbmOld);
	
	int delay = 1000 / fps;
	for ( ;; ) {
		uxWindow->run();		
		BitBlt( hdc, 0, 0, uxWindow->getWidth(), uxWindow->getHeight(), hdcMem, 0, 0, SRCCOPY );
		Sleep( delay );
	}
	
}

HWND createWindow(Environment& environment, const char *name, int width, int height,  LRESULT CALLBACK (*proc)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) ) {
	WNDCLASSEX wc;
		// Init wc
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.cbSize = sizeof( WNDCLASSEX );
	wc.hbrBackground = CreateSolidBrush( 0 );
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	wc.hIconSm = LoadIcon( NULL, IDI_APPLICATION );
	wc.hInstance = environment.getHInstance();
	wc.lpfnWndProc = proc;
	wc.lpszClassName = name;
	wc.lpszMenuName = NULL;
	wc.style = 0;
	// Register wc
	if ( !RegisterClassEx(&wc) ) {
		MessageBox( NULL, "Failed to register window class.", "Error", MB_OK );
		return 0;
	}
	
	// Make window
	return CreateWindowEx(
		WS_EX_APPWINDOW,
		name,
		name,
		WS_MINIMIZEBOX | WS_SYSMENU | WS_POPUP | WS_CAPTION,
		0, 0, width, height,
		NULL, NULL, environment.getHInstance(), NULL );
	
}



void UxWindow::setHandlers(HDC _hdc, HDC _hdcMem, HBITMAP _hbmOld) {
	this->hdc = _hdc;
	this->hdcMem = _hdcMem;
	this->hbmOld = _hbmOld;
}

HBITMAP UxWindow::getHBitmap() const {
	return hbmp;
}

UxBitmap& UxWindow::getBitmap()  {
	return *bitmap;
}

pixel *UxWindow::getPixels() const {
	return pixels;
}

void UxWindow::run() {
	onFrame(this);
}


UxWindow::UxWindow(Environment& _environment, const char *name, int _width, int _height, ON_FRAME _onFrame) : 
environment(_environment), width(_width), height(_height), onFrame(_onFrame) {
	this->hwnd = createWindow(environment, name, width, height, WndProc2);
	
	RECT rcClient, rcWindow;	
	POINT ptDiff;
	// Get window and client sizes
	GetClientRect( this->hwnd, &rcClient );
	GetWindowRect( this->hwnd, &rcWindow );
	// Find offset between window size and client size
	ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
	ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
	
	const RECT& displayRect = _environment.getDisplayList().getDisplay(0).getRect();
	
	int c_x = (displayRect.left + displayRect.right)/2;
	int c_y = (displayRect.top + displayRect.bottom)/2;
	
	c_x -= (rcWindow.left + width + ptDiff.x)/2;
	c_y -= (rcWindow.top + height + ptDiff.y)/2;
	
	// Resize client
	MoveWindow( this->hwnd, c_x, c_y, width + ptDiff.x, height + ptDiff.y, false);
	UpdateWindow( this->hwnd );
	
	environment.add(this);
	
	create();	

	bitmap = UxBitmap::wrap(pixels, width, height, ARGB_8888);
}

const HWND UxWindow::getHwnd() const {
	return hwnd;
}

void UxWindow::show() const {
	ShowWindow(this->hwnd, SW_SHOW);
}

void UxWindow::create() {
	makeSurface();
}

void UxWindow::paint() {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint( hwnd, &ps );
	// Draw pixels to window when window needs repainting
	BitBlt( hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY );
	EndPaint( hwnd, &ps );
}

void UxWindow::close() {
	DestroyWindow( hwnd );
	environment.remove(this);		
	TerminateThread(hTickThread, 0);
	WaitForSingleObject(hTickThread, INFINITE);
	SelectObject( hdcMem, hbmOld );
	DeleteDC( hdc );
}

UxWindow::~UxWindow() {	
	environment.remove(this);
	delete bitmap;
}

void UxWindow::makeSurface() {
	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight =  -height; // Order pixels from top to bottom
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32; // last byte not used, 32 bit for alignment
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;
	bmi.bmiHeader.biXPelsPerMeter = 0;
	bmi.bmiHeader.biYPelsPerMeter = 0;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biClrImportant = 0;
	bmi.bmiColors[0].rgbBlue = 0;
	bmi.bmiColors[0].rgbGreen = 0;
	bmi.bmiColors[0].rgbRed = 0;
	bmi.bmiColors[0].rgbReserved = 0;
	HDC hdc = GetDC( hwnd );
	// Create DIB section to always give direct access to pixels
	hbmp = CreateDIBSection( hdc, &bmi, DIB_RGB_COLORS, (void**)&pixels, NULL, 0 );
	DeleteDC( hdc );
		
	hTickThread = CreateThread( NULL, 0, &tickThreadProc, this, 0, NULL );
}

int UxWindow::getWidth() const {
	return width;
}

int UxWindow::getHeight() const {
	return height;
}

const UxWindow* Environment::search(HWND hwnd) const {
	std::vector<UxWindow*>::const_iterator it = std::find_if (uxWindows.begin(), uxWindows.end(), HasHwnd<UxWindow*>(hwnd));
	if (it == uxWindows.end()) {	
		return 0;
	} else {		
		return *it;
	}
}
