#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

class Keyboard {
private:
#ifdef _WIN32
	// Needed for Windows implementation of Keyboard
	// Find key codes at https://msdn.microsoft.com/en-us/library/ms927178.aspx?f=255&MSPPError=-2147217396
	INPUT _buffer[1];
	HWND window; // The emulator
#endif

public:
	Keyboard(HWND window);
	Keyboard();
	void keyUp(char key);
	void keyDown(char key);
	void keyClick(char key);
};