#pragma once

#include <thread>

#include "Keyboard.h"

class Controller {
private:
	Keyboard kb;
	void jumpWatcher(unsigned int expiration);
	char right;
	char left;
	char start;
	char jump;
	bool alreadyJumping;

public:
	Controller(Keyboard kb);
	void runRight();
	void runLeft();
	void enter();
	void stop();
	void smallJump();
	void mediumJump();
	void largeJump();
};