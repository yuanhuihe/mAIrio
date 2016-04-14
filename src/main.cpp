#include <iostream>
#include <algorithm>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>

#include "Entity.h"
#include "Mario.h"
#include "ScreenCapture.h"

std::vector<cv::Mat> getSpriteList(WorldType world);
cv::Mat getHue(std::string loc);
void findEnemyTemplateInFrame(cv::Mat image, cv::Mat enemyTemplate, std::vector<cv::Rect> boundingBoxes, cv::Scalar drawColor, int match_method, double threshold);
cv::Rect findMarioInFrame(cv::Mat image, cv::Mat enemyTemplate, cv::Rect marioBoundingBox, int match_method, double threshold);
std::vector<cv::Mat> loadMarioTemplates();

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
	cv::Rect marioBoundingRect;
	bool foundMario = false;
	int marioState = 0;

	std::vector<cv::Mat> marioTemplates = loadMarioTemplates();
	int marioThresholds[] = {150000, 150000, 150000, 150000, 150000, 150000};
	// std::vector<cv::Mat> spriteList; // = getSpriteList(WorldType::OVERWORLD);
	Entity::fillSpriteTable(WorldType::OVERWORLD);

	cv::namedWindow("Image", cv::WINDOW_AUTOSIZE);

	/* cap.open(argv[1]);

	if (!cap.isOpened()) {
		std::cout << "Cannot open " << argv[1] << std::endl;
		return -1;
	} */

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (stricmp(entry.szExeFile, "fceux.exe") == 0)
			{
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
				DWORD id = GetProcessId(hProcess);
				EnumWindows((WNDENUMPROC)EnumWindowsProcMy, id);
				CloseHandle(hProcess);
			}
		}
	}

	while (1) {
		foundMario = false;
		/*if (newWorldType) {
			// Determine type of world
			spriteList = getSpriteList(world);
		}*/

		// cap >> input;
		input = hwnd2mat(emulator_window);
		if (input.empty()) {
			std::cout << "Empty input" << std::endl;
			break;
		}

		// Remove alpha component for template matching - I think this is why it works?
		cv::cvtColor(input, input, CV_RGBA2RGB);

		std::vector<cv::Rect> enemyBoundingBoxes;
		/* for (int i = 1; i < EntityType::PIRANHA; i++) {
			// 780000
			findEnemyTemplateInFrame(input, Entity::spriteTable[i], enemyBoundingBoxes, cv::Scalar(0, 255, i*10), CV_TM_SQDIFF, Entity::getDetThresh((EntityType) i));
		} */

		std::vector<int> marioAttempts;

		while (foundMario != true) {
			marioAttempts.push_back(marioState);
			marioBoundingRect = findMarioInFrame(input, marioTemplates[marioState], cv::Rect(0, 0, 0, 0), CV_TM_SQDIFF, marioThresholds[marioState]);
			if (marioBoundingRect.area() == 0) {

				if (marioAttempts.size() > 5) {
					// We couldn't find mario at all, give up on this frame
					break;
				}

				if (marioState == 0 && std::find(marioAttempts.begin(), marioAttempts.end(), 1) == marioAttempts.end()) {
					// We couldn't find small mario to the right, look for small mario to the left
					marioState = 1;
				} else if (marioState == 0 && std::find(marioAttempts.begin(), marioAttempts.end(), 1) != marioAttempts.end()) {
					// We couldn't find small mario to the right or left, look for big mario to the right
					marioState = 2;
				} else if (marioState == 1 && std::find(marioAttempts.begin(), marioAttempts.end(), 0) == marioAttempts.end()) {
					// We couldn't find small mario to the left, look for small mario to the left
					marioState = 0;
				} else if (marioState == 1 && std::find(marioAttempts.begin(), marioAttempts.end(), 0) != marioAttempts.end()) {
					// We couldn't find small mario to the right or left, look for big mario to the right
					marioState = 2;
				} else if (marioState == 2 && std::find(marioAttempts.begin(), marioAttempts.end(), 3) == marioAttempts.end()) {
					// We couldn't find big mario to the right, look for big mario to the left
					marioState = 3;
				} else if (marioState == 2 && std::find(marioAttempts.begin(), marioAttempts.end(), 3) != marioAttempts.end()) {
					// We couldn't find big mario to the right or left
					if (std::find(marioAttempts.begin(), marioAttempts.end(), 0) != marioAttempts.end()) {
						marioState = 4;
					}
					else {
						marioState = 0;
					}

				} else if (marioState == 3 && std::find(marioAttempts.begin(), marioAttempts.end(), 2) == marioAttempts.end()) {
					// We couldn't find big mario to the left, look for big mario to the right
					marioState = 2;
				} else if (marioState == 3 && std::find(marioAttempts.begin(), marioAttempts.end(), 2) != marioAttempts.end()) {
					// We couldn't find big mario to the left or right
					if (std::find(marioAttempts.begin(), marioAttempts.end(), 1) != marioAttempts.end()) {
						marioState = 5;
					}
					else {
						marioState = 1;
					}

				} else if (marioState == 4 && std::find(marioAttempts.begin(), marioAttempts.end(), 5) == marioAttempts.end()) {
					// We couldn't find fire mario to the right, look for fire mario to the left
					marioState = 3;
				} else if (marioState == 4 && std::find(marioAttempts.begin(), marioAttempts.end(), 5) != marioAttempts.end()) {
					// We couldn't find fire mario to the right or left, look for big mario to the right
					marioState = 2;
				} else if (marioState == 5 && std::find(marioAttempts.begin(), marioAttempts.end(), 4) == marioAttempts.end()) {
					// We couldn't find fire mario to the left, look for big mario to the right
					marioState = 4;
				} else if (marioState == 5 && std::find(marioAttempts.begin(), marioAttempts.end(), 4) != marioAttempts.end()) {
					// We couldn't find fire mario to the left or right, look for big mario to the left
					marioState = 3;
				}

			}
			else {
				cv::rectangle(input, marioBoundingRect, cv::Scalar(255,255,0), 2, 8, 0);
				foundMario = true;
			}
		}

		std::cout << std::endl;

		for (int i = 0; i < enemyBoundingBoxes.size(); i++) {
			cv::rectangle(recon, enemyBoundingBoxes[i], cv::Scalar(127,127,127));
		}

		// cv::cvtColor(input, input, CV_8UC4);

		cv::imshow("Image", input);

		if (cv::waitKey(1) == 27) {
			break;
		}
	}

	return 0;
}

std::vector<cv::Mat> loadMarioTemplates() {
	std::vector<cv::Mat> list;
	list.push_back(cv::imread("sprites/mario/small-mario-template.png"));
	list.push_back(cv::imread("sprites/mario/small-mario-template-left.png"));
	list.push_back(cv::imread("sprites/mario/big-mario-normal-template.png"));
	list.push_back(cv::imread("sprites/mario/big-mario-normal-template-left.png"));
	list.push_back(cv::imread("sprites/mario/fire-mario-normal-template.png"));
	list.push_back(cv::imread("sprites/mario/fire-mario-left.png"));
	return list;
}

cv::Rect findMarioInFrame(cv::Mat image, cv::Mat marioTemplate, cv::Rect marioBoundingBox, int match_method, double threshold) {

	cv::Mat result;
	cv::Point minLoc;
	cv::Point maxLoc;
	cv::Point matchLoc;

	double minVal;
	double maxVal;

	// Create the result matrix
	int result_cols = image.cols - marioTemplate.cols + 1;
	int result_rows = image.rows - marioTemplate.rows + 1;

	result.create(result_rows, result_cols, CV_32FC1);
	// cv::imshow("Template", marioTemplate);

	/// Do the Matching and Normalize
	cv::matchTemplate(image, marioTemplate, result, match_method);

	// Try and find the first enemy template
	cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
	std::cout << minVal << ", ";

	// TODO - allow for max match technique
	if (minVal < threshold) {
		if (match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED)
		{
			matchLoc = minLoc;
		}
		else
		{
			matchLoc = maxLoc;
		}

		// Populate our enemy bounding boxes
		// TODO - Extrapolate enemy size

		// Return rectangle
		return cv::Rect(matchLoc, cv::Point(matchLoc.x + marioTemplate.cols, matchLoc.y + marioTemplate.rows));
	}
	else {
		return cv::Rect(0,0,0,0);
	}
}

cv::Mat getHue(std::string loc) {
	cv::Mat tmp = cv::imread(loc, CV_LOAD_IMAGE_COLOR);
	cv::cvtColor(tmp, tmp, cv::COLOR_BGR2HSV);
	cv::Mat ch[3];
	cv::split(tmp, ch);
	return ch[0];
}

// CV_TM_SQDIFF for match method presently.
// TODO - Optimization
void findEnemyTemplateInFrame(cv::Mat image, cv::Mat enemyTemplate, std::vector<cv::Rect> boundingBoxes, cv::Scalar drawColor, int match_method, double threshold) {
	
	cv::Mat result;
	cv::Point minLoc;
	cv::Point maxLoc;
	cv::Point matchLoc;

	double minVal;
	double maxVal;

	// Create the result matrix
	int result_cols = image.cols - enemyTemplate.cols + 1;
	int result_rows = image.rows - enemyTemplate.rows + 1;

	result.create(result_rows, result_cols, CV_32FC1);

	/// Do the Matching and Normalize
	cv::matchTemplate(image, enemyTemplate, result, match_method);

	// Try and find the first enemy template
	cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
	std::cout << minVal << std::endl;

	// TODO - allow for max match technique
	while (minVal < threshold) {
		if (match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED)
		{
			matchLoc = minLoc;
		}
		else
		{
			matchLoc = maxLoc;
		}

		// Populate our enemy bounding boxes
		// TODO - Extrapolate enemy size
		boundingBoxes.push_back(cv::Rect(matchLoc, cv::Point(matchLoc.x + enemyTemplate.cols, matchLoc.y + enemyTemplate.rows)));

		// Draw what we see
		cv::rectangle(image, matchLoc, cv::Point(matchLoc.x + enemyTemplate.cols, matchLoc.y + enemyTemplate.rows), drawColor, 2, 8, 0);

		// Fill in our result to remove previously tracked objects
		cv::floodFill(result, matchLoc, cv::Scalar(threshold), 0);

		// Find the next template
		cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
	}
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
