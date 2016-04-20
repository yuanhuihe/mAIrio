#include "Mario.h"
#include <iostream>
#include <algorithm>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

	Mario::Mario() {
		loadMarioTemplates();

		return;
	}

	void Mario::loadMarioTemplates() {
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
		}*/
				return marioBoundingRect;
			}
	}
}

const bool transTable[MarioType::SIZE_MARIO_TYPE][MarioType::SIZE_MARIO_TYPE] = {
		       // SMALL_L, SMALL_R, BIG_L, BIG_R, FIRE_L, FIRE_R, DEAD
	/*SMALL_L*/ { true,    true,    true,  false, false,  false,  true },
	/*SMALL_R*/ { true,    true,    false, true,  false,  false,  true },
	/*BIG_L*/   { true,    false,   true,  true,  true,   false,  true },
	/*BIG_R*/   { false,   true,    true,  true,  false,  true,   true },
	/*FIRE_L*/  { false,   false,   true,  false, true,   true,   true },
	/*FIRE_R*/  { false,   true,    false, true,  true,   true,   true },
	/*DEAD*/    { false,   true,    false, false, false,  false,  true }
};