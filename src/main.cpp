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
	const int PURGE_TIME = 1000;

	Entity::fillSpriteTable(WorldType::OVERWORLD);
	
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
		control.runRight();
		start = GetTickCount();

		/*if (newWorldType) {
			// Determine type of world
			spriteList = getSpriteList(world);
		}*/

		// cap >> input;
		if (emulator_window != NULL) {
			input = hwnd2mat(emulator_window);
			if (input.empty()) {
				std::cout << "Lost FCEUX" << std::endl;
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
			known[i].updateState(input, start);
		}

		// Detect new sprites
		std::vector<Entity> newEntities = Entity::watch(input, known, start);

		// Add newEntities to known
		for (int i = 0; i < newEntities.size(); i++) {
			known.push_back(newEntities[i]);
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

		// Find holes in the ground
		int startX = -1;
		int endX = -1;
		for (int i = 0; i < input.cols; i++) {
			while (input.at<Vec3b>(205, i)[0] == 252 && input.at<Vec3b>(205, i)[1] == 148 && input.at<Vec3b>(205, i)[2] == 92) {
				if (startX > -1) {
					endX = i;
				}
				else {
					startX = i;
				}

				if (++i >= input.cols) {
					break;
				}
			}

			if (startX > -1) {
				cv::rectangle(input, cv::Rect(startX, 205, endX - startX, 5), cv::Scalar(255, 0, 0), 2);
				startX = -1;
				endX = -1;
			}
		}

		// Should Mario Jump?
		for (Entity e : known) {
			if (e.isHostile() &&
				e.getLoc().x - mario.getLoc().x < 32 &&
				e.getLoc().x - mario.getLoc().x > 0 &&
				abs(e.getLoc().y - mario.getLoc().y) < 32) {
				control.smallJump();
				break;
			}
		}

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

		if (cv::waitKey(1) == 27) {
			break;
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
