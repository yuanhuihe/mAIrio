#include "Controller.h"
#include "Keyboard.h"

Controller::Controller(Keyboard kb) {
	this->kb = kb;
	right = 'D';
	left = 'A';
	jump = 'K';
	alreadyJumping = false;
}

void Controller::jumpWatcher(unsigned int expiration) {
	while (1) {
		if (GetTickCount() >= expiration) {
			kb.keyUp('K');
			alreadyJumping = false;
			break;
		}
	}
}

void Controller::runRight() {
	kb.keyUp(left);
	kb.keyDown(right);
}

void Controller::runLeft() {
	kb.keyUp(right);
	kb.keyDown(left);
}

void Controller::stop() {
	kb.keyUp(right);
	kb.keyUp(left);
}

void Controller::smallJump() {
	if (!alreadyJumping) {
		alreadyJumping = true;
		kb.keyDown(jump);
		std::thread thr(&Controller::jumpWatcher, this, GetTickCount() + 50);
		thr.detach();
	}
}

void Controller::mediumJump() {
	if (!alreadyJumping) {
		kb.keyDown(jump);
		std::thread thr(&Controller::jumpWatcher, this, GetTickCount() + 250);
		thr.detach();
	}
}

void Controller::largeJump() {
	if (!alreadyJumping) {
		kb.keyDown(jump);
		std::thread thr(&Controller::jumpWatcher, this, GetTickCount() + 1000);
		thr.detach();
	}
}