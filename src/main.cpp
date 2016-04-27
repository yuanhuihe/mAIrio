#include <iostream>
#include <algorithm>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <time.h>

#include "Entity.h"
#include "ScreenCapture.h"
#include "Keyboard.h"
#include "Controller.h"

cv::Mat getHue(std::string loc);

// Global Window handle
HWND emulator_window;

BOOL CALLBACK EnumWindowsProcMy(HWND hwnd, LPARAM lParam)
{
	DWORD lpdwProcessId;
	GetWindowThreadProcessId(hwnd, &lpdwProcessId);
	if (lpdwProcessId == lParam)
	{
		emulator_window = hwnd;
		return FALSE;
	}
	return TRUE;
}

int main(int argc, char** argv) {
	cv::VideoCapture cap;
	cv::Mat input;
	cv::Mat inputCh[3];
	cv::Mat recon;
	cv::Mat output;
	cv::Rect marioBoundingRect;
	Entity mario(EntityType::MARIO_SMALL_R);
	EntityType lostMarioAttempt = EntityType::MARIO_SMALL_L;
	DWORD start, end;
	int fps;
	std::vector<Entity> known;
	const int PURGE_TIME = 500;
	cv::Mat blockImage;
	cv::Mat blockMask(224, 256, CV_8U);
	cv::Mat connComp;
	WorldType world = WorldType::OVERWORLD;

	for (int i = 0; i < 224; i++) {
		for (int j = 0; j < 256; j++) {
			if (i < 200) {
				blockMask.at<uchar>(i, j) = 255;
			}
			else {
				blockMask.at<uchar>(i, j) = 0;
			}
		} 
	}

	Entity::fillSpriteTable(world);
	
	cv::namedWindow("Image", cv::WINDOW_AUTOSIZE);

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (strcmp(entry.szExeFile, "fceux.exe") == 0)
			{
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
				DWORD id = GetProcessId(hProcess);
				EnumWindows((WNDENUMPROC)EnumWindowsProcMy, id);
				CloseHandle(hProcess);
				GetFocus();
			}
		}
	}
	Keyboard kb(emulator_window);
	Controller control(kb);

	while (1) {
		start = GetTickCount();

		// cap >> input;
		if (emulator_window != NULL) {
			input = hwnd2mat(emulator_window);
			if (input.empty()) {
				std::cout << "Unable to extract frame" << std::endl;
				return -1;
			}
		}
		else {
			std::cout << "Lost FCEUX" << std::endl;
			return -1;
		}

		// Get rid of alpha
		cv::cvtColor(input, input, CV_RGBA2RGB);

		if (input.at<cv::Vec3b>(0, 0)[0] == 252 && input.at<cv::Vec3b>(0, 0)[1] == 148 && input.at<cv::Vec3b>(0, 0)[2] == 92 && world != WorldType::OVERWORLD) {
			world = WorldType::OVERWORLD;
			Entity::fillSpriteTable(world);
		}
		else if (input.at<cv::Vec3b>(0, 0)[0] == 0 && input.at<cv::Vec3b>(0, 0)[1] == 0 && input.at<cv::Vec3b>(0, 0)[2] == 0 && world != WorldType::UNDERWORLD) {
			world = WorldType::UNDERWORLD;
			Entity::fillSpriteTable(world);
		}

		// Find Mario
		mario.updateState(input, start);
		cv::rectangle(input, mario.getBBox(), cv::Scalar::all(255), 2);

		// If we've lost Mario, cycle through a new type each frame
		if (start - mario.timeLastSeen() > 5000) {
			mario.setType(lostMarioAttempt);
			lostMarioAttempt = static_cast<EntityType>(lostMarioAttempt + 1);
			if (lostMarioAttempt > EntityType::MARIO_FIRE_R) {
				lostMarioAttempt = EntityType::MARIO_SMALL_L;
			}
		}

		// Find known sprites
		for (int i = 0; i < known.size(); i++) {
			if (known[i].getType() != EntityType::HOLE && known[i].getType() != EntityType::BRICK) {
				known[i].updateState(input, start);
			}
		}

		// Detect new sprites
		std::vector<Entity> newEntities = Entity::watch(input, known, start);
		std::vector<Entity> holes;
		std::vector<Entity> bricks;

		// Add newEntities to known
		for (int i = 0; i < newEntities.size(); i++) {
			if (newEntities[i].getType() != EntityType::HOLE && newEntities[i].getType() != EntityType::BRICK) {
				known.push_back(newEntities[i]);
			}
			else if (newEntities[i].getType() == EntityType::HOLE) {
				holes.push_back(newEntities[i]);
			}
			else if (newEntities[i].getType() == EntityType::BRICK) {
				bricks.push_back(newEntities[i]);
			}
		}

		// Draw all known sprites and delete old ones
		for (int i = 0; i < known.size(); i++) {
			if (known[i].inFrame()) {
				cv::rectangle(input, known[i].getBBox(), cv::Scalar::all(255), 2);
			}
			else if (start - known[i].timeLastSeen() > PURGE_TIME) {
				known.erase(known.begin() + i);
			}
		}
		for (int i = 0; i < holes.size(); i++) {
			cv::rectangle(input, holes[i].getBBox(), cv::Scalar::all(255), 2);
		}
		for (int i = 0; i < bricks.size(); i++) {
			cv::rectangle(input, bricks[i].getBBox(), cv::Scalar::all(255), 2);
		}

		bool farEnemy = false;
		bool closeEnemy = false;
		bool smallPipe = false;
		bool largePipe = false;
		bool stairs = false;
		bool stairGap = false;
		int holeWidth = 0;
		bool forwardStairs = false;
		bool overStairs = false;
		bool beneathStairs = false;
		bool beneathPillar = false;
		bool veryFarEnemy = false;
		bool brickAbove = false;

		cv::Scalar marioFarRightAboveCenterColor = cv::Scalar(0, 255, 0);
		cv::Point marioFarRightAboveCenter = mario.getCenter();
		marioFarRightAboveCenter.y -= 16;
		marioFarRightAboveCenter.x += 48;

		cv::Scalar marioExtremeForwardCenterColor = cv::Scalar(0, 255, 0);
		cv::Point marioExtremeForwardCenter = mario.getCenter();
		marioExtremeForwardCenter.x += 64;

		cv::Scalar marioStraightAboveCenterColor = cv::Scalar(0, 255, 0);
		cv::Point marioStraightAboveCenter = mario.getCenter();
		marioStraightAboveCenter.y -= 16;

		cv::Scalar marioForwardCenterColor = cv::Scalar(0, 255, 0);
		cv::Point marioForwardCenter = mario.getCenter();
		marioForwardCenter.x += 20;

		cv::Scalar marioFarForwardCenterColor = cv::Scalar(0, 255, 0);
		cv::Point marioFarForwardCenter = mario.getCenter();
		marioFarForwardCenter.x += 48;

		cv::Scalar marioUnderCenterColor = cv::Scalar(0, 255, 0);
		cv::Point marioUnderCenter = mario.getCenter();
		marioUnderCenter.y += 16;

		cv::Scalar marioAboveCenterColor = cv::Scalar(0, 255, 0);
		cv::Point marioAboveCenter = mario.getCenter();
		marioAboveCenter.x += 16;
		marioAboveCenter.y -= 16;

		cv::Scalar marioFarAboveCenterColor = cv::Scalar(0, 255, 0);
		cv::Point marioFarAboveCenter = mario.getCenter();
		marioFarAboveCenter.x += 16;
		marioFarAboveCenter.y -= 32;

		for (Entity b : bricks) {
			if (b.getBBox().contains(marioFarAboveCenter)) {
				brickAbove = true;
				marioFarAboveCenterColor = cv::Scalar(0, 255, 255);
			}
			else if (b.getBBox().contains(marioStraightAboveCenter)) {
				brickAbove = true;
				marioStraightAboveCenterColor = cv::Scalar(0, 255, 255);
			}
			else if (b.getBBox().contains(marioAboveCenter)) {
				brickAbove = true;
				marioAboveCenterColor = cv::Scalar(0, 255, 255);
			}
			else if (b.getBBox().contains(marioFarRightAboveCenter)) {
				brickAbove = true;
				marioFarRightAboveCenterColor = cv::Scalar(0, 255, 255);
			}
		}

		// Should Mario Jump?
		for (Entity e : known) {
			forwardStairs |= e.getType() == EntityType::CHISELED && e.getBBox().contains(marioForwardCenter);
			overStairs |= e.getType() == EntityType::CHISELED && e.getBBox().contains(marioUnderCenter);
			beneathStairs |= e.getType() == EntityType::CHISELED && e.getBBox().contains(marioAboveCenter);

			if (forwardStairs) {
				marioForwardCenterColor = cv::Scalar(0, 255, 255);
			}

			if (overStairs) {
				marioUnderCenterColor = cv::Scalar(0, 255, 255);
			}

			if (beneathStairs) {
				marioAboveCenterColor = cv::Scalar(0, 255, 255);
			}

			// If Mario needs to jump over an enemy
			if (e.isHostile() &&
				e.inFrame() &&
				e.getBBox().contains(marioForwardCenter)) {
				closeEnemy = true;
				marioForwardCenterColor = cv::Scalar(0, 255, 255);
			}
			else if (e.isHostile() &&
				e.inFrame() &&
				e.getBBox().contains(marioExtremeForwardCenter)) {
				veryFarEnemy = true;
				marioExtremeForwardCenterColor = cv::Scalar(0, 255, 255);
			}
			// If Mario needs to jump over two enemies
			else if (e.isHostile() &&
				e.inFrame() &&
				e.getBBox().contains(marioFarForwardCenter)) {
				farEnemy = true;
				marioFarForwardCenterColor = cv::Scalar(0, 255, 255);
			}
			// If Mario needs to jump over a pipe
			else if (e.getType() == EntityType::PIPE && e.getBBox().contains(marioAboveCenter)) {
				smallPipe = true;
				marioAboveCenterColor = cv::Scalar(0, 255, 255);
			}
			// If Mario needs to jump over a pipe
			else if (e.getType() == EntityType::PIPE && e.getBBox().contains(marioFarAboveCenter)) {
				largePipe = true;
				marioFarAboveCenterColor = cv::Scalar(0, 255, 255);
			}
			// If Mario needs to climb a staircase
			else if (forwardStairs) {
				stairs = true;
			}
			// If Mario needs to jump the gap between staircase
			else if ((overStairs && !forwardStairs)) {
				stairGap = true;
			}
			else if (beneathStairs) {
				beneathPillar = true;
			}
		}
		// If mario needs to jump over holes on the ground
		for (Entity e : holes) {
			if (e.getLoc().x - mario.getLoc().x < 16 &&
				e.getLoc().x - mario.getLoc().x > 0) {
				holeWidth = e.getBBox().width;
				break;
			}
		}

		cv::circle(input, marioForwardCenter, 1, marioForwardCenterColor, 2);
		cv::circle(input, marioUnderCenter, 1, marioUnderCenterColor, 2);
		cv::circle(input, marioAboveCenter, 1, marioAboveCenterColor, 2);
		cv::circle(input, marioFarAboveCenter, 1, marioFarAboveCenterColor, 2);
		cv::circle(input, marioFarForwardCenter, 1, marioFarForwardCenterColor, 2);
		cv::circle(input, marioStraightAboveCenter, 1, marioStraightAboveCenterColor, 2);
		cv::circle(input, marioFarRightAboveCenter, 1, marioFarRightAboveCenterColor, 2);
		cv::circle(input, marioExtremeForwardCenter, 1, marioExtremeForwardCenterColor, 2);

		if (brickAbove && (closeEnemy || farEnemy || veryFarEnemy)) {
			control.runLeft();
			std::cout << "Left!" << std::endl;
		} else{
			control.runRight();
		}

		// Handle movement
		if (closeEnemy && !farEnemy) {
			control.smallJump();
		}
		else if (closeEnemy && farEnemy) {
			control.mediumJump();
		}
		else if (largePipe) {
			control.largeJump();
		}
		else if (smallPipe) {
			control.mediumJump();
		}
		else if (beneathStairs) {
			control.largeJump();
		}
		else if (stairGap && !stairs) {
			control.largeJump();
		}
		else if (stairs) {
			control.stop();
			control.smallJump();
		}
		else if (holeWidth > 30) {
			control.mediumJump();
		}
		else if (holeWidth > 0) {
			control.smallJump();
		}

		// std::cout << " Forward: " << forwardStairs << " Over: " << overStairs << " Under: " << beneathStairs << std::endl;
		//  "Far: " << farEnemy << " Close: " << closeEnemy << " Pipe: " << pipeHeight << " Hole: " << holeWidth 

		// std::cout << known.size() << std::endl;

		end = GetTickCount();

		if (end != start) {
			fps = 1000 / (end - start);
		}

		// Put fps on the screen. Maybe make it a toggle option
		std::ostringstream strs;
		strs << fps;
		std::string str = strs.str();
		cv::putText(input, str, cv::Point(15, 30), FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 255), 2);

		cv::imshow("Image", input);

		int key_press = cv::waitKey(1);

		if (key_press == 27) {
			break;
		}
		else if (key_press == ' ') {
			std::cout << "Enter" << std::endl;
			// control.enter();
		}
	}

	return 0;
}

cv::Mat getHue(std::string loc) {
	cv::Mat tmp = cv::imread(loc, CV_LOAD_IMAGE_COLOR);
	cv::cvtColor(tmp, tmp, cv::COLOR_BGR2HSV);
	cv::Mat ch[3];
	cv::split(tmp, ch);
	return ch[0];
}
