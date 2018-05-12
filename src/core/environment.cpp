#include <vector>
#include <algorithm>
#include "environment.h"

Environment mainEnvironment;


const DisplayList& Environment::getDisplayList() const {
	return displayList;
}

void Environment::setHInstance(HINSTANCE _hIstance) {
	this->hIstance = _hIstance;
}
		
HINSTANCE Environment::getHInstance() const {
	return hIstance;
}

void Environment::add(UxWindow *uxWindow) {
	uxWindows.push_back(uxWindow);
}

void Environment::remove(UxWindow *uxWindow) {
	uxWindows.erase(std::remove(uxWindows.begin(), uxWindows.end(), uxWindow), uxWindows.end());	
	
	if (uxWindows.size() == 0) {
		PostQuitMessage( 0 );
	}
}



