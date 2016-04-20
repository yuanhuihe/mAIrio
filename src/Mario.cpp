#include <iostream>
#include <algorithm>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "Mario.h"

// The top left point of the rectangle is where the sprite starts based off of its template
// TODO - Update bounding box sizes
void Mario::setBoundingBox() {
	switch (type) {
	case MarioType::SMALL_L: bbox = cv::Rect(-3, -5, 16, 16); break;
	case MarioType::SMALL_R: bbox = cv::Rect(-5, -13, 16, 24); break;
	case MarioType::BIG_L: bbox = cv::Rect(-2, -13, 16, 24); break;
	case MarioType::BIG_R: bbox = cv::Rect(-3, -5, 16, 16); break;
	case MarioType::FIRE_L: bbox = cv::Rect(-5, -13, 16, 24); break;
	case MarioType::FIRE_R: bbox = cv::Rect(-2, -13, 16, 24); break;
	}
}

int Mario::getDetThresh(MarioType type) {
	int detThresh = 0;
	switch (type) {
	case MarioType::SMALL_L: detThresh = 150000; break;
	case MarioType::SMALL_R: detThresh = 150000; break;
	case MarioType::BIG_L: detThresh = 150000; break;
	case MarioType::BIG_R: detThresh = 150000; break;
	case MarioType::FIRE_L: detThresh = 150000; break;
	case MarioType::FIRE_R: detThresh = 150000; break;
	}
	return detThresh;
}

void Mario::fillSpriteTable() {
	// Fill spriteTable
	Mario::spriteTable[MarioType::SMALL_L] = cv::imread("sprites/mario/small-mario-template-left.png", CV_LOAD_IMAGE_COLOR);
	Mario::spriteTable[MarioType::SMALL_R] = cv::imread("sprites/mario/small-mario-template.png", CV_LOAD_IMAGE_COLOR);
	Mario::spriteTable[MarioType::BIG_L] = cv::imread("sprites/mario/big-mario-normal-template-left.png", CV_LOAD_IMAGE_COLOR);
	Mario::spriteTable[MarioType::BIG_R] = cv::imread("sprites/mario/big-mario-normal-template.png", CV_LOAD_IMAGE_COLOR);;
	Mario::spriteTable[MarioType::FIRE_L] = cv::imread("sprites/mario/fire-mario-left.png", CV_LOAD_IMAGE_COLOR);
	Mario::spriteTable[MarioType::FIRE_R] = cv::imread("sprites/mario/fire-mario-normal-template.png", CV_LOAD_IMAGE_COLOR);
}

Mario::Mario(cv::Point loc, MarioType type, int timeMS) {
	this->loc = loc;
	this->type = type;
	msLastSeen = timeMS;
	isInFrame = true;

	setBoundingBox();
}

Mario::Mario() {
	this->loc = cv::Point(0, 0);
	this->type = MarioType::SIZE_MARIO_TYPE;
	isInFrame = true;

	setBoundingBox();
}

bool Mario::updateState(cv::Mat image, int timeMS) {
	cv::Mat result;
	cv::Point minLoc;
	cv::Point maxLoc;
	cv::Point matchLoc;

	double minVal;
	double maxVal;
	int method = cv::TM_SQDIFF;
	MarioType tmpType = type;

	const int MARGIN = 5;
	int x = std::max(0, bbox.x + loc.x - MARGIN);
	int y = std::max(0, bbox.y + loc.y - MARGIN);
	int width = std::min(bbox.width + MARGIN * 2, image.cols - x);
	int height = std::min(bbox.height + MARGIN * 2, image.rows - y);
	cv::Mat roi = image(cv::Rect(x, y, width, height));
	// imshow("ROI", roi);

	while (true) {
		// Create the result matrix
		int result_cols = roi.cols - spriteTable[tmpType].cols + 1;
		int result_rows = roi.rows - spriteTable[tmpType].rows + 1;
		result.create(result_rows, result_cols, CV_32FC1);

		// Do the Matching and Normalize
		cv::matchTemplate(roi, spriteTable[tmpType], result, method);

		// Try and find the first enemy template
		cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

		if (minVal < getDetThresh(tmpType)) { // We found it
			loc = cv::Point(minLoc.x + loc.x + bbox.x - MARGIN, minLoc.y + loc.y + bbox.y - MARGIN);
			type = tmpType;

			setBoundingBox();
			isInFrame = true;
			msLastSeen = timeMS;

			return true;
		}

		// Essentially tmpType++
		if (tmpType == type) {
			// Reset to first type if tmpType is what we thought the Entity was
			tmpType = MarioType::SMALL_R;
		}
		else {
			tmpType = static_cast<MarioType>(tmpType + 1);
		}
		MarioType t;
		for (t = tmpType; t != MarioType::SIZE_MARIO_TYPE; t = static_cast<MarioType>(t + 1)) {
			if (transTable[type][t] && t != type) {
				tmpType = t;
				break;
			}
		}
		if (t == MarioType::SIZE_MARIO_TYPE) {
			isInFrame = false;
			return false; // We lost the Entity
		}
	}
}

Mario Mario::watch(cv::Mat image, int timeMS) {
	// Only look at the right
	int origWidth = image.size().width;
	image = image(cv::Rect(0, 0, image.size().width - 40, image.size().height));

	cv::Mat result;
	cv::Point minLoc;
	cv::Point maxLoc;
	cv::Point matchLoc;

	double minVal;
	double maxVal;
	int method = cv::TM_SQDIFF;

	for (MarioType t = MarioType::SMALL_R; t != MarioType::SIZE_MARIO_TYPE; t = static_cast<MarioType>(t + 1)) {
		// Create the result matrix
		int result_cols = image.cols - spriteTable[t].cols + 1;
		int result_rows = image.rows - spriteTable[t].rows + 1;
		result.create(result_rows, result_cols, CV_32FC1);

		// Do the Matching and Normalize
		cv::matchTemplate(image, spriteTable[t], result, method);

		// Try and find the first enemy template
		cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

		if (minVal < getDetThresh(t)) { // We found mario
			return Mario(minLoc, t, timeMS);
		}
	}

	return Mario();
}

int Mario::timeLastSeen() {
	return msLastSeen;
}

	/* void Mario::fillSpriteTable() {
		marioTemplates.push_back(cv::imread("sprites/mario/small-mario-template.png"));
		marioTemplates.push_back(cv::imread("sprites/mario/small-mario-template-left.png"));
		marioTemplates.push_back(cv::imread("sprites/mario/big-mario-normal-template.png"));
		marioTemplates.push_back(cv::imread("sprites/mario/big-mario-normal-template-left.png"));
		marioTemplates.push_back(cv::imread("sprites/mario/fire-mario-normal-template.png"));
		marioTemplates.push_back(cv::imread("sprites/mario/fire-mario-left.png"));
	}

	cv::Rect Mario::findMarioInFrame(cv::Mat image, int match_method) {

		cv::Mat result;
		cv::Point minLoc;
		cv::Point maxLoc;
		cv::Point matchLoc;

		double minVal;
		double maxVal;

		// Create the result matrix
		int result_cols = image.cols - marioTemplates[m].cols + 1;
		int result_rows = image.rows - marioTemplates[m].rows + 1;

		result.create(result_rows, result_cols, CV_32FC1);
		// cv::imshow("Template", marioTemplate);

		/// Do the Matching and Normalize
		cv::matchTemplate(image, marioTemplates[m], result, match_method);

		// Try and find the first enemy template
		cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
		std::cout << minVal << ", ";

		// TODO - allow for max match technique
		if (minVal < marioThresholds[m]) {
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
			return cv::Rect(matchLoc, cv::Point(matchLoc.x + marioTemplates[m].cols, matchLoc.y + marioTemplates[m].rows));
		}
		else {
			return cv::Rect(0, 0, 0, 0);
		}
	}

	cv::Rect Mario::findMario(cv::Mat input) {
		std::vector<int> marioAttempts;
		bool foundMario = false;
		cv::Rect marioBoundingRect;

		while (foundMario != true) {
			marioAttempts.push_back(m);
			marioBoundingRect = findMarioInFrame(input, CV_TM_SQDIFF);
			if (marioBoundingRect.area() == 0) {
				if (marioAttempts.size() > SIZE_MARIO_TYPE) {
					// We couldn't find mario at all, give up on this frame
					break;
				}
				/*
				if (m == 0 && std::find(marioAttempts.begin(), marioAttempts.end(), 1) == marioAttempts.end()) {
					// We couldn't find small mario to the right, look for small mario to the left
					marioState = 1;
				}
				else if (marioState == 0 && std::find(marioAttempts.begin(), marioAttempts.end(), 1) != marioAttempts.end()) {
					// We couldn't find small mario to the right or left, look for big mario to the right
					marioState = 2;
				}
				else if (marioState == 1 && std::find(marioAttempts.begin(), marioAttempts.end(), 0) == marioAttempts.end()) {
					// We couldn't find small mario to the left, look for small mario to the left
					marioState = 0;
				}
				else if (marioState == 1 && std::find(marioAttempts.begin(), marioAttempts.end(), 0) != marioAttempts.end()) {
					// We couldn't find small mario to the right or left, look for big mario to the right
					marioState = 2;
				}
				else if (marioState == 2 && std::find(marioAttempts.begin(), marioAttempts.end(), 3) == marioAttempts.end()) {
					// We couldn't find big mario to the right, look for big mario to the left
					marioState = 3;
				}
				else if (marioState == 2 && std::find(marioAttempts.begin(), marioAttempts.end(), 3) != marioAttempts.end()) {
					// We couldn't find big mario to the right or left
					if (std::find(marioAttempts.begin(), marioAttempts.end(), 0) != marioAttempts.end()) {
						marioState = 4;
					}
					else {
						marioState = 0;
					}

				}
				else if (marioState == 3 && std::find(marioAttempts.begin(), marioAttempts.end(), 2) == marioAttempts.end()) {
					// We couldn't find big mario to the left, look for big mario to the right
					marioState = 2;
				}
				else if (marioState == 3 && std::find(marioAttempts.begin(), marioAttempts.end(), 2) != marioAttempts.end()) {
					// We couldn't find big mario to the left or right
					if (std::find(marioAttempts.begin(), marioAttempts.end(), 1) != marioAttempts.end()) {
						marioState = 5;
					}
					else {
						marioState = 1;
					}

				}
				else if (marioState == 4 && std::find(marioAttempts.begin(), marioAttempts.end(), 5) == marioAttempts.end()) {
					// We couldn't find fire mario to the right, look for fire mario to the left
					marioState = 3;
				}
				else if (marioState == 4 && std::find(marioAttempts.begin(), marioAttempts.end(), 5) != marioAttempts.end()) {
					// We couldn't find fire mario to the right or left, look for big mario to the right
					marioState = 2;
				}
				else if (marioState == 5 && std::find(marioAttempts.begin(), marioAttempts.end(), 4) == marioAttempts.end()) {
					// We couldn't find fire mario to the left, look for big mario to the right
					marioState = 4;
				}
				else if (marioState == 5 && std::find(marioAttempts.begin(), marioAttempts.end(), 4) != marioAttempts.end()) {
					// We couldn't find fire mario to the left or right, look for big mario to the left
					marioState = 3;
				}

			}
			else {
				foundMario = true;
				cv::rectangle(input, marioBoundingRect, cv::Scalar(255, 255, 0), 2, 8, 0);
			}
		}
				return marioBoundingRect;
			}
	}
}
*/

const bool spriteTable[MarioType::SIZE_MARIO_TYPE];

const bool transTable[MarioType::SIZE_MARIO_TYPE][MarioType::SIZE_MARIO_TYPE] = {
		       // SMALL_L, SMALL_R, BIG_L, BIG_R, FIRE_L, FIRE_R, DEAD
	/*SMALL_R*/{ true,    true,    false, true,  false,  false,  true },
	/*SMALL_L*/ { true,    true,    true,  false, false,  false,  true },
	/*BIG_R*/{ false,   true,    true,  true,  false,  true,   true },
	/*BIG_L*/   { true,    false,   true,  true,  true,   false,  true },
	/*FIRE_R*/{ false,   true,    false, true,  true,   true,   true },
	/*FIRE_L*/  { false,   false,   true,  false, true,   true,   true },
	/*DEAD*/    { false,   true,    false, false, false,  false,  true }
};