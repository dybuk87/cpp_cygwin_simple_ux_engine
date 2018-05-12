#ifndef __ENVIRONMENT__H__
#define __ENVIRONMENT__H__

#include <windows.h>
#include "display.h"
#include <vector>

class UxWindow;

class Environment {
	public:
		void add(UxWindow *uxWindow); 
		void remove(UxWindow *uxWindow);			
		
		const UxWindow* search(HWND hwnd) const;
	
		const DisplayList& getDisplayList() const;
		
		void setHInstance(HINSTANCE _hIstance);
		
		HINSTANCE getHInstance() const;
	
	private:
		std::vector<UxWindow*> uxWindows;
		DisplayList displayList;
		HINSTANCE hIstance;
};

extern Environment mainEnvironment;

#define START_PROC(startup_func) int WINAPI WinMain( \
      HINSTANCE hInstance2,\
      HINSTANCE hPrevInstance,\
      LPSTR lpCmdLine,\
      int nShowCmd)\
{\
	mainEnvironment.setHInstance(hInstance2);\
	MSG msg;\
	startup_func;\
	\
	while ( GetMessage(&msg, 0, 0, 0) > 0 ) {\
		TranslateMessage( &msg );\
		DispatchMessage( &msg );\
	}\
	return 0;\
}

#endif