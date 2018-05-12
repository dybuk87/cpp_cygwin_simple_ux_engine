#ifndef __DISPLAY__H__
#define __DISPLAY__H__

#include <vector>
#include <windows.h>

class Display {
public:
	Display(LPRECT rect);
	const RECT& getRect() const;
private:
	RECT rect;
};

class DisplayList {
public:
	DisplayList();
	void addDisplay(LPRECT lprcMonitor);
	const Display& getDisplay(int no) const;
private:
	std::vector<Display> display;
};

void Hibernation() ;

#endif