#include "display.h"

BOOL CALLBACK monitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);

Display::Display(LPRECT lprcMonitor) {
	rect.left = lprcMonitor->left;
	rect.right = lprcMonitor->right;
	rect.top = lprcMonitor->top;
	rect.bottom = lprcMonitor->bottom;
}

const RECT& Display::getRect() const {
	return rect;
}

DisplayList::DisplayList() {
	EnumDisplayMonitors(NULL, NULL, monitorEnumProc, (LPARAM)this);
}

void DisplayList::addDisplay(LPRECT lprcMonitor) {    
	display.push_back(Display(lprcMonitor));
}

const Display& DisplayList::getDisplay(int no) const {
	return display[no];
}

void Hibernation() {
	SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM) 2);
}

void ScreenSaver() {
	SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_SCREENSAVE, 0);
}

BOOL CALLBACK monitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    DisplayList *displayList = (DisplayList*)dwData;
	
	displayList->addDisplay(lprcMonitor);
	    
	return TRUE;
}