#include "Keyboard.h"
#include <iostream>

// Windows implementation of Keyboard
#ifdef _WIN32
Keyboard::Keyboard(HWND window) {
	_buffer->type = INPUT_KEYBOARD;
	_buffer->ki.wScan = 0;
	_buffer->ki.time = 0;
	_buffer->ki.dwExtraInfo = 0;
	this->window = window;
}

Keyboard::Keyboard() {
	_buffer->type = INPUT_KEYBOARD;
	_buffer->ki.wScan = 0;
	_buffer->ki.time = 0;
	_buffer->ki.dwExtraInfo = 0;
	this->window = 0;
}

void Keyboard::keyUp(char key) {
	if (GetForegroundWindow() == window) {
		_buffer->ki.wVk = key;
		_buffer->ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, _buffer, sizeof(INPUT));
	}
}

void Keyboard::keyDown(char key) {
	if (GetForegroundWindow() == window) {
		_buffer->ki.wVk = key;
		_buffer->ki.dwFlags = 0;
		SendInput(1, _buffer, sizeof(INPUT));
	}
}

void Keyboard::keyClick(char key) {
	if (GetForegroundWindow() == window) {
		keyDown(key);
		Sleep(10);
		keyUp(key);
	}
}
#endif