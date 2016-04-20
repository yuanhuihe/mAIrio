#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

#include "Entity.h"

// The top left point of the rectangle is where the sprite starts based off of its template
void Entity::setBoundingBox() {
	switch (type) {
	case EntityType::GOOMBA: bbox = cv::Rect(-3, -5, 16, 16); break;
	case EntityType::KOOPA_L: bbox = cv::Rect(-5, -13, 16, 24); break;
	case EntityType::KOOPA_R: bbox = cv::Rect(-2, -13, 16, 24); break;
	//case EntityType::KOOPA_RED_L: bbox = cv::Rect(-5, -13, 16, 24); break;
	//case EntityType::KOOPA_RED_R: bbox = cv::Rect(-2, -13, 16, 24); break;
	case EntityType::SHELL: bbox = cv::Rect(-4, -3, 16, 14); break;
	/*case EntityType::SHELL_RED: bbox = cv::Rect(-4, -3, 16, 14); break;
	case EntityType::PIRANHA: bbox = cv::Rect(); break;
	case EntityType::BRICK: bbox = cv::Rect(); break;
	case EntityType::QUESTION: bbox = cv::Rect(); break;
	case EntityType::ROCK: bbox = cv::Rect(); break;
	case EntityType::FLAGPOLE: bbox = cv::Rect(); break;
	case EntityType::BEAM: bbox = cv::Rect(); break;
	case EntityType::PIPE: bbox = cv::Rect(); break;
	case EntityType::MUSHROOM: bbox = cv::Rect(); break;
	case EntityType::FIREFLOWER: bbox = cv::Rect(); break;*/
	}
}

int Entity::getDetThresh(EntityType type) {
	int detThresh = 0;
	switch (type) {
	case EntityType::GOOMBA: detThresh = 780000; break;
	case EntityType::KOOPA_L: detThresh = 150000; break;
	case EntityType::KOOPA_R: detThresh = 150000; break;
	//case EntityType::KOOPA_RED_L: detThresh = 150000; break;
	//case EntityType::KOOPA_RED_R: detThresh = 150000; break;
	case EntityType::SHELL: detThresh = 150000; break;
	/*case EntityType::SHELL_RED: detThresh = 150000; break;
	case EntityType::PIRANHA: detThresh = 150000; break;
	case EntityType::BRICK: detThresh = 150000; break;
	case EntityType::QUESTION: detThresh = 150000; break;
	case EntityType::ROCK: detThresh = 150000; break;
	case EntityType::FLAGPOLE: detThresh = 150000; break;
	case EntityType::BEAM: detThresh = 150000; break;
	case EntityType::PIPE: detThresh = 150000; break;
	case EntityType::MUSHROOM: detThresh = 150000; break;
	case EntityType::FIREFLOWER: detThresh = 150000; break;*/
	}
	return detThresh;
}

void Entity::fillSpriteTable(WorldType world) {
	std::string worldStr;
	if (world == WorldType::OVERWORLD) {
		worldStr = "overworld";
	}
	else {
		worldStr = "underworld";
	}

	// Fill spriteTable
	Entity::spriteTable[EntityType::GOOMBA] = cv::imread("sprites/enemies/" + worldStr + "/goomba-template.png", CV_LOAD_IMAGE_COLOR);
	Entity::spriteTable[EntityType::KOOPA_L] = cv::imread("sprites/enemies/" + worldStr + "/koopa-l-template.png", CV_LOAD_IMAGE_COLOR);
	Entity::spriteTable[EntityType::KOOPA_R] = cv::imread("sprites/enemies/" + worldStr + "/koopa-r-template.png", CV_LOAD_IMAGE_COLOR);
	//Entity::spriteTable[EntityType::KOOPA_RED_L] = cv::imread("sprites/enemies/shared/koopa-l-template.png", CV_LOAD_IMAGE_COLOR);
	//Entity::spriteTable[EntityType::KOOPA_RED_R] = cv::imread("sprites/enemies/shared/koopa-r-template.png", CV_LOAD_IMAGE_COLOR);
	Entity::spriteTable[EntityType::SHELL] = cv::imread("sprites/enemies/" + worldStr + "/shell-template.png", CV_LOAD_IMAGE_COLOR);
	//Entity::spriteTable[EntityType::SHELL_RED] = cv::imread("sprites/enemies/shared/shell-template.png", CV_LOAD_IMAGE_COLOR);
	/*spriteTable[EntityType::PIRANHA] = cv::imread("sprites/enemies/" + worldStr + "/goomba-template.png", CV_LOAD_IMAGE_COLOR);
	spriteTable[EntityType::BRICK] = cv::imread("sprites/misc/" + worldStr + "/brick-template.png", CV_LOAD_IMAGE_COLOR);
	spriteTable[EntityType::QUESTION]
	spriteTable[EntityType::ROCK]
	spriteTable[EntityType::FLAGPOLE]
	spriteTable[EntityType::BEAM]
	spriteTable[EntityType::PIPE]
	spriteTable[EntityType::MUSHROOM]
	spriteTable[EntityType::FIREFLOWER]*/
}

Entity::Entity(cv::Point loc, EntityType type, int timeMS) {
	this->loc = loc;
	this->type = type;
	msLastSeen = timeMS;

	setBoundingBox();
}

Entity::Entity() {
	this->loc = cv::Point(0,0);
	this->type = SIZE_ENTITY_TYPE;
	isInFrame = true;

	setBoundingBox();
}

cv::Point Entity::getLoc() {
	return loc;
}

cv::Rect Entity::getBBox() {
	return cv::Rect(loc.x + bbox.x, loc.y + bbox.y, bbox.width, bbox.height);
}

cv::Mat Entity::getSprite() {
	return spriteTable[type];
}

EntityType Entity::getType() {
	return type;
}

void Entity::setLoc(cv::Point loc) {
	this->loc = loc;
}

void Entity::setType(EntityType type) {
	this->type = type;
}

bool Entity::isPassable() { // How to handle breakable bricks?
	switch (type) {
	//case EntityType::MARIO:
	case EntityType::SHELL:
	/*case EntityType::FLAGPOLE:
	case EntityType::MUSHROOM:
	case EntityType::FIREFLOWER:*/
		return true;
	default:
		return false;
	}
}

bool Entity::isHostile() { // Technically should also return true for a moving shell
	switch (type) {
	case EntityType::GOOMBA:
	case EntityType::KOOPA_R:
	case EntityType::KOOPA_L:
	//case EntityType::PIRANHA:
		return true;
	default:
		return false;
	}
}

bool Entity::updateState(cv::Mat image, int timeMS) {
	cv::Mat result;
	cv::Point minLoc;
	cv::Point maxLoc;
	cv::Point matchLoc;

	double minVal;
	double maxVal;
	int method = cv::TM_SQDIFF;
	EntityType tmpType = type;

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
			tmpType = EntityType::GOOMBA;
		}
		else {
			tmpType = static_cast<EntityType>(tmpType + 1);
		}
		EntityType t;
		for (t = tmpType; t != EntityType::SIZE_ENTITY_TYPE; t = static_cast<EntityType>(t + 1)) {
			if (transTable[type][t] && t != type) {
				tmpType = t;
				break;
			}
		}
		if (t == EntityType::SIZE_ENTITY_TYPE) {
			isInFrame = false;
			return false; // We lost the Entity
		}
	}
}

bool Entity::inFrame() {
	return isInFrame;
}

std::vector<Entity> Entity::watch(cv::Mat image, std::vector<Entity> known, int timeMS) {
	std::vector<Entity> ret;
	
	// Only look at the right
	int origWidth = image.size().width;
	image = image(cv::Rect(image.size().width - 40, 0, 40, image.size().height));
	
	cv::Mat result;
	cv::Point minLoc;
	cv::Point maxLoc;
	cv::Point matchLoc;

	double minVal;
	double maxVal;
	int method = cv::TM_SQDIFF;

	for (EntityType t = EntityType::GOOMBA; t != EntityType::SIZE_ENTITY_TYPE; t = static_cast<EntityType>(t + 1)) {
		// Create the result matrix
		int result_cols = image.cols - spriteTable[t].cols + 1;
		int result_rows = image.rows - spriteTable[t].rows + 1;
		result.create(result_rows, result_cols, CV_32FC1);

		cv::Mat maskedImage = image.clone();
		for (int i = 0; i < known.size(); i++) {
			if (known[i].getType() == t) {
				cv::Rect bbox = known[i].getBBox();
				bbox.x -= (origWidth - maskedImage.cols);
				std::cout << bbox.x << std::endl;
				cv::rectangle(maskedImage, bbox, cv::Scalar::all(255), CV_FILLED);
			}
		}

		cv::imshow("Masked Watch", maskedImage);

		// Do the Matching and Normalize
		cv::matchTemplate(maskedImage, spriteTable[t], result, method);

		// White out the Entities we know about of this type
		/* for (int i = 0; i < known.size(); i++) {
			if (known[i].getType() == t) {
				for (int m = std::max(0, bbox.y); m < std::min(result.size().height, bbox.y + bbox.height); m++) {
					for (int n = std::max(0, bbox.x); m < std::min(result.size().width, bbox.x + bbox.width); n++) {
						result.at<float>(cv::Point(n, m)) = std::numeric_limits<float>::max();
					}
				}
			}
		} */

		// Try and find the first enemy template
		cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
		// minLoc = cv::Point(minLoc.x + spriteTable[t].cols - 1, minLoc.y + spriteTable[t].rows - 1);

		if (minVal < getDetThresh(t)) { // We found one
			minLoc.x += (origWidth - maskedImage.cols);
			ret.push_back(Entity(minLoc, t, timeMS));
		}
	}

	return ret;
}

int Entity::timeLastSeen() {
	return msLastSeen;
}

cv::Mat Entity::spriteTable[EntityType::SIZE_ENTITY_TYPE];

const bool Entity::transTable[EntityType::SIZE_ENTITY_TYPE][EntityType::SIZE_ENTITY_TYPE] = {
		       // GOOMBA, KOOPA_L, KOOPA_R, SHELL
	/*GOOMBA*/  { true,   false,   false,   false },
	/*KOOPA_L*/ { false,  true,    true,    true },
	/*KOOPA_R*/ { false,  true,    true,    true },
	/*SHELL*/   { false,  false,   false,    true },
};