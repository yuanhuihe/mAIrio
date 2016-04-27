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

std::vector<cv::Mat> getSpriteList(WorldType world);
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

	Entity::fillSpriteTable(WorldType::UNDERWORLD);
	
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
		// control.runRight();
		start = GetTickCount();

		/*if (newWorldType) {
			// Determine type of world
			spriteList = getSpriteList(world);
		}*/

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
		int pipeHeight = 0;
		bool stairs = false;
		bool stairGap = false;
		int holeWidth = 0;

		// Should Mario Jump?
		for (Entity e : known) {

			bool forwardStairs = e.getType() == EntityType::CHISELED && e.getLoc().x - mario.getLoc().x < 16 &&
				e.getLoc().x - mario.getLoc().x > 0 && abs(mario.getLoc().y - e.getLoc().y) < 8;
			bool overStairs = e.getType() == EntityType::CHISELED && abs(e.getLoc().x - mario.getLoc().x) < mario.getBBox().width / 2
				&& mario.getLoc().y < e.getLoc().y && abs(mario.getLoc().y - e.getLoc().y) < 32;
			// If Mario needs to jump over an enemy
			if (e.isHostile() &&
				e.inFrame() &&
				e.getLoc().x - mario.getLoc().x < 32 &&
				e.getLoc().x - mario.getLoc().x > 0 &&
				abs(e.getLoc().y - mario.getLoc().y) < 32) {
				closeEnemy = true;
			}
			// If Mario needs to jump over two enemies
			else if (e.isHostile() &&
				e.inFrame() &&
				e.getLoc().x - mario.getLoc().x < 64 &&
				e.getLoc().x - mario.getLoc().x >= 32 &&
				abs(e.getLoc().y - mario.getLoc().y) < 32) {
				farEnemy = true;
			}
			// If Mario needs to jump over a pipe
			else if (e.getType() == EntityType::PIPE && e.getLoc().x - mario.getLoc().x < 16 &&
				e.getLoc().x - mario.getLoc().x > 0) {
				pipeHeight = mario.getLoc().y - e.getLoc().y;
				break;
			}
			// If Mario needs to jump the gap between staircase
			else if (overStairs) {
				stairGap = true;
			}
			// If Mario needs to climb a staircase
			else if (forwardStairs) {
				stairs = true;
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

		// Handle movement
		if (closeEnemy && !farEnemy) {
			// smallJump();
		}
		else if (closeEnemy && farEnemy) {
			// control.mediumJump();
		}
		else if (pipeHeight > 36) {
			// control.largeJump();
		}
		else if (pipeHeight > 0) {
			// control.mediumJump();
		}
		else if (stairGap) {
			// control.largeJump();
		}
		else if (stairs) {
			// control.smallJump();
		}
		else if (holeWidth > 30) {
			// control.mediumJump();
		}
		else if (holeWidth > 0) {
			// control.smallJump();
		}

		std::cout << "Far: " << farEnemy << " Close: " << closeEnemy << " Pipe: " << pipeHeight << " Stairs: " << stairs << " Stair Gap: " << stairGap << " Hole: " << holeWidth << std::endl;

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

std::vector<cv::Mat> getSpriteList(WorldType world) {
	std::vector<cv::Mat> list;

	std::string worldStr;
	if (world == WorldType::OVERWORLD) {
		worldStr = "overworld";
	}
	else {
		worldStr = "underworld";
	}

	// It works for overworld
	// list.push_back(cv::imread("sprites/enemies/" + worldStr + "/goomba-template.png"));
	// list.push_back(cv::imread("sprites/enemies/" + worldStr + "/koopa-left-template.png"));
	// list.push_back(cv::imread("sprites/enemies/" + worldStr + "/goomba1.png"));
	// list.push_back(cv::imread("sprites/enemies/" + worldStr + "/goomba2.png"));
	// list.push_back(cv::imread("sprites/enemies/" + worldStr + "/goomba-flat.png"));
	// list.push_back(cv::imread("sprites/enemies/" + worldStr + "/koopa1.png"));
	// list.push_back(cv::imread("sprites/enemies/" + worldStr + "/koopa2.png"));
	// list.push_back(cv::imread("sprites/enemies/" + worldStr + "/koopa3.png"));
	// list.push_back(cv::imread("sprites/enemies/" + worldStr + "/koopa4.png"));
	// list.push_back(cv::imread("sprites/enemies/" + worldStr + "/koopa-shell.png"));
	// list.push_back(cv::imread("sprites/enemies/shared/koopa1.png"));
	// list.push_back(cv::imread("sprites/enemies/shared/koopa2.png"));
	// list.push_back(cv::imread("sprites/enemies/shared/koopa3.png"));
	// list.push_back(cv::imread("sprites/enemies/shared/koopa4.png"));
	// list.push_back(cv::imread("sprites/enemies/shared/koopa-shell.png"));
	// list.push_back(cv::imread("sprites/enemies/" + worldStr + "/shell.png"));
	// list.push_back(cv::imread("sprites/enemies/shared/shell.png"));
	// list.push_back(cv::imread("sprites/enemies/" + worldStr + "/piranha1.png"));
	// list.push_back(cv::imread("sprites/enemies/" + worldStr + "/piranha2.png"));
	// list.push_back(cv::imread("sprites/misc/" + worldStr + "/brick1.png"));
	// list.push_back(cv::imread("sprites/misc/" + worldStr + "/brick2.png"));
	// list.push_back(cv::imread("sprites/misc/" + worldStr + "/question1.png"));
	// list.push_back(cv::imread("sprites/misc/" + worldStr + "/question2.png"));
	// list.push_back(cv::imread("sprites/misc/" + worldStr + "/question3.png"));
	// list.push_back(cv::imread("sprites/misc/" + worldStr + "/rock.png"));
	// list.push_back(cv::imread("sprites/misc/" + worldStr + "/block-chiseled.png"));
	/* list.push_back(cv::imread("sprites/misc/" + worldStr + "/used-block.png")); */
	// list.push_back(cv::imread("sprites/misc/" + worldStr + "/flagpole.png"));
	// list.push_back(cv::imread("sprites/misc/shared/beam-short.png"));
	// list.push_back(cv::imread("sprites/misc/shared/beam-medium.png"));
	// list.push_back(cv::imread("sprites/misc/shared/beam-long.png"));
	// list.push_back(cv::imread("sprites/misc/shared/pipe-up.png"));
	// list.push_back(cv::imread("sprites/misc/shared/pipe-down.png"));
	// list.push_back(cv::imread("sprites/misc/shared/pipe-left.png"));
	// list.push_back(cv::imread("sprites/misc/shared/pipe-t.png"));
	// list.push_back(cv::imread("sprites/misc/shared/pipe-tess.png"));
	// list.push_back(cv::imread("sprites/powerups/shared/mushroom.png"));
	if (world == WorldType::OVERWORLD) {
		// list.push_back(getHue("sprites/misc/overworld/flagpole.png"));
	}

	return list;
}
