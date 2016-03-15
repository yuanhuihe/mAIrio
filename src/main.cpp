#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "Entity.h"

std::vector<cv::Mat> getSpriteList(WorldType world);
cv::Mat getHue(std::string loc);

int main(int argc, char** argv) {
	cv::VideoCapture cap;
	cv::Mat input;
	cv::Mat inputCh[3];
	cv::Mat recon;
	std::vector<cv::Mat> spriteList = getSpriteList(WorldType::OVERWORLD);

	cv::namedWindow("Input", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Recon", cv::WINDOW_AUTOSIZE);

	cap.open(argv[1]);
	if (!cap.isOpened()) {
		std::cout << "Cannot open " << argv[1] << std::endl;
		return -1;
	}

	while (1) {
		/*if (newWorldType) {
			// Determine type of world
			spriteList = getSpriteList(world);
		}*/

		cap >> input;
		if (input.empty()) {
			break;
		}
		cv::cvtColor(input, input, cv::COLOR_BGR2HSV);
		cv::split(input, inputCh);
		
		// Match
		//for (int i = 0; i < spriteList.size(); i++) {
			cv::Mat tmp(input.rows - spriteList[0].rows + 1, input.cols - spriteList[0].cols + 1, CV_32FC1);
			cv::matchTemplate(inputCh[1], spriteList[0], tmp, cv::TM_CCOEFF_NORMED);
			//cv::threshold(tmp, tmp, 0.45, 255, cv::THRESH_BINARY);
			// If match, place on recon
		//}

		cv::imshow("Input", inputCh[0]);
		cv::imshow("Recon", tmp);
		if (cv::waitKey(30) == 27) {
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
	list.push_back(getHue("sprites/enemies/" + worldStr + "/goomba1.png"));
	list.push_back(getHue("sprites/enemies/" + worldStr + "/goomba2.png"));
	list.push_back(getHue("sprites/enemies/" + worldStr + "/goomba-flat.png"));
	list.push_back(getHue("sprites/enemies/" + worldStr + "/koopa1.png"));
	list.push_back(getHue("sprites/enemies/" + worldStr + "/koopa2.png"));
	list.push_back(getHue("sprites/enemies/" + worldStr + "/koopa3.png"));
	list.push_back(getHue("sprites/enemies/" + worldStr + "/koopa4.png"));
	list.push_back(getHue("sprites/enemies/" + worldStr + "/koopa-shell.png"));
	list.push_back(getHue("sprites/enemies/shared/koopa1.png"));
	list.push_back(getHue("sprites/enemies/shared/koopa2.png"));
	list.push_back(getHue("sprites/enemies/shared/koopa3.png"));
	list.push_back(getHue("sprites/enemies/shared/koopa4.png"));
	list.push_back(getHue("sprites/enemies/shared/koopa-shell.png"));
	list.push_back(getHue("sprites/enemies/" + worldStr + "/shell.png"));
	list.push_back(getHue("sprites/enemies/shared/shell.png"));
	list.push_back(getHue("sprites/enemies/" + worldStr + "/piranha1.png"));
	list.push_back(getHue("sprites/enemies/" + worldStr + "/piranha2.png"));
	list.push_back(getHue("sprites/misc/" + worldStr + "/brick1.png"));
	list.push_back(getHue("sprites/misc/" + worldStr + "/brick2.png"));
	list.push_back(getHue("sprites/misc/" + worldStr + "/question1.png"));
	list.push_back(getHue("sprites/misc/" + worldStr + "/question2.png"));
	list.push_back(getHue("sprites/misc/" + worldStr + "/question3.png"));
	list.push_back(getHue("sprites/misc/" + worldStr + "/rock.png"));
	list.push_back(getHue("sprites/misc/" + worldStr + "/block-chiseled.png"));
	list.push_back(getHue("sprites/misc/" + worldStr + "/used-block.png"));
	list.push_back(getHue("sprites/misc/" + worldStr + "/flagpole.png"));
	list.push_back(getHue("sprites/misc/shared/beam-short.png"));
	list.push_back(getHue("sprites/misc/shared/beam-medium.png"));
	list.push_back(getHue("sprites/misc/shared/beam-long.png"));
	list.push_back(getHue("sprites/misc/shared/pipe-up.png"));
	list.push_back(getHue("sprites/misc/shared/pipe-down.png"));
	list.push_back(getHue("sprites/misc/shared/pipe-left.png"));
	list.push_back(getHue("sprites/misc/shared/pipe-t.png"));
	list.push_back(getHue("sprites/misc/shared/pipe-tess.png"));
	list.push_back(getHue("sprites/powerups/shared/mushroom.png"));
	if (world == WorldType::OVERWORLD) {
		list.push_back(getHue("sprites/misc/overworld/flagpole.png"));
	}

	return list;
}
