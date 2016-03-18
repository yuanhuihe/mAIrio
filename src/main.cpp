#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "Entity.h"

std::vector<cv::Mat> getSpriteList(WorldType world);
cv::Mat getHue(std::string loc);
void findEnemyTemplateInFrame(cv::Mat image, cv::Mat enemyTemplate, std::vector<cv::Rect> boundingBoxes, cv::Scalar drawColor, int match_method, double threshold);

int main(int argc, char** argv) {
	cv::VideoCapture cap;
	cv::Mat input;
	cv::Mat inputCh[3];
	cv::Mat recon;
	std::vector<cv::Mat> spriteList = getSpriteList(WorldType::OVERWORLD);

	cv::namedWindow("Input", cv::WINDOW_AUTOSIZE);
	// cv::namedWindow("Recon", cv::WINDOW_AUTOSIZE);

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

		// Redeclare the enemy bounding boxes for each frame
		std::vector<cv::Rect> enemyBoundingBoxes;
		findEnemyTemplateInFrame(input, spriteList.front(), enemyBoundingBoxes, cv::Scalar(0, 255, 255), CV_TM_SQDIFF, 780000);

		// cv::cvtColor(input, input, cv::COLOR_BGR2HSV);
		// cv::split(input, inputCh);
		
		// Match
		//for (int i = 0; i < spriteList.size(); i++) {
		//	cv::Mat tmp(input.rows - spriteList[0].rows + 1, input.cols - spriteList[0].cols + 1, CV_32FC1);
		//	cv::matchTemplate(inputCh[1], spriteList[0], tmp, cv::TM_CCOEFF_NORMED);
			//cv::threshold(tmp, tmp, 0.45, 255, cv::THRESH_BINARY);
			// If match, place on recon
		//}

		cv::imshow("Image", input);
		// cv::imshow("Template Tracking", tmp);
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
		cv::floodFill(result, matchLoc, cv::Scalar(780000), 0);

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
	list.push_back(cv::imread("sprites/enemies/" + worldStr + "/goomba-template.png"));
	list.push_back(cv::imread("sprites/enemies/" + worldStr + "/goomba1.png"));
	list.push_back(cv::imread("sprites/enemies/" + worldStr + "/goomba2.png"));
	list.push_back(cv::imread("sprites/enemies/" + worldStr + "/goomba-flat.png"));
	list.push_back(cv::imread("sprites/enemies/" + worldStr + "/koopa1.png"));
	list.push_back(cv::imread("sprites/enemies/" + worldStr + "/koopa2.png"));
	list.push_back(cv::imread("sprites/enemies/" + worldStr + "/koopa3.png"));
	list.push_back(cv::imread("sprites/enemies/" + worldStr + "/koopa4.png"));
	list.push_back(cv::imread("sprites/enemies/" + worldStr + "/koopa-shell.png"));
	list.push_back(cv::imread("sprites/enemies/shared/koopa1.png"));
	list.push_back(cv::imread("sprites/enemies/shared/koopa2.png"));
	list.push_back(cv::imread("sprites/enemies/shared/koopa3.png"));
	list.push_back(cv::imread("sprites/enemies/shared/koopa4.png"));
	list.push_back(cv::imread("sprites/enemies/shared/koopa-shell.png"));
	list.push_back(cv::imread("sprites/enemies/" + worldStr + "/shell.png"));
	list.push_back(cv::imread("sprites/enemies/shared/shell.png"));
	list.push_back(cv::imread("sprites/enemies/" + worldStr + "/piranha1.png"));
	list.push_back(cv::imread("sprites/enemies/" + worldStr + "/piranha2.png"));
	list.push_back(cv::imread("sprites/misc/" + worldStr + "/brick1.png"));
	list.push_back(cv::imread("sprites/misc/" + worldStr + "/brick2.png"));
	list.push_back(cv::imread("sprites/misc/" + worldStr + "/question1.png"));
	list.push_back(cv::imread("sprites/misc/" + worldStr + "/question2.png"));
	list.push_back(cv::imread("sprites/misc/" + worldStr + "/question3.png"));
	list.push_back(cv::imread("sprites/misc/" + worldStr + "/rock.png"));
	list.push_back(cv::imread("sprites/misc/" + worldStr + "/block-chiseled.png"));
	list.push_back(cv::imread("sprites/misc/" + worldStr + "/used-block.png"));
	list.push_back(cv::imread("sprites/misc/" + worldStr + "/flagpole.png"));
	list.push_back(cv::imread("sprites/misc/shared/beam-short.png"));
	list.push_back(cv::imread("sprites/misc/shared/beam-medium.png"));
	list.push_back(cv::imread("sprites/misc/shared/beam-long.png"));
	list.push_back(cv::imread("sprites/misc/shared/pipe-up.png"));
	list.push_back(cv::imread("sprites/misc/shared/pipe-down.png"));
	list.push_back(cv::imread("sprites/misc/shared/pipe-left.png"));
	list.push_back(cv::imread("sprites/misc/shared/pipe-t.png"));
	list.push_back(cv::imread("sprites/misc/shared/pipe-tess.png"));
	list.push_back(cv::imread("sprites/powerups/shared/mushroom.png"));
	if (world == WorldType::OVERWORLD) {
		list.push_back(getHue("sprites/misc/overworld/flagpole.png"));
	}

	return list;
}
