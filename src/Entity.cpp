#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

#include "Entity.h"

cv::Vec3b Entity::holeColor;

// The top left point of the rectangle is where the sprite starts based off of its template
void Entity::setBoundingBox() {
	switch (type) {
	case EntityType::MARIO_SMALL_L: bbox = cv::Rect(-3, -1, 13, 16); break;
	case EntityType::MARIO_SMALL_R: bbox = cv::Rect(-3, -1, 13, 16); break;
	case EntityType::MARIO_BIG_L: bbox = cv::Rect(-4, -3, 16, 32); break;
	case EntityType::MARIO_BIG_R: bbox = cv::Rect(-3, -3, 16, 32); break;
	case EntityType::MARIO_FIRE_L: bbox = cv::Rect(-4, -3, 16, 32); break;
	case EntityType::MARIO_FIRE_R: bbox = cv::Rect(-3, -3, 16, 32); break;
	case EntityType::GOOMBA: bbox = cv::Rect(-3, -5, 16, 16); break;
	case EntityType::KOOPA_L: bbox = cv::Rect(-5, -13, 16, 24); break;
	case EntityType::KOOPA_R: bbox = cv::Rect(-2, -13, 16, 24); break;
	case EntityType::PIPE: bbox = cv::Rect(-5, -6, 33, 32); break;
	case EntityType::QUESTION_Y: bbox = cv::Rect(-3, -3, 16, 16); break;
	case EntityType::QUESTION_O: bbox = cv::Rect(-3, -3, 16, 16); break;
	case EntityType::QUESTION_B: bbox = cv::Rect(-3, -3, 16, 16); break;
	case EntityType::BRICK: bbox = cv::Rect(0, -1, 8, 9); break;
	case EntityType::KOOPA_RED_L: bbox = cv::Rect(-5, -13, 16, 24); break;
	case EntityType::KOOPA_RED_R: bbox = cv::Rect(-2, -13, 16, 24); break;
	case EntityType::SHELL: bbox = cv::Rect(-4, -3, 16, 14); break;
	case EntityType::CHISELED: bbox = cv::Rect(-3, -3, 16, 16); break;
	case EntityType::BEAM: bbox = cv::Rect(-3, -3, 48, 8); break;
	case EntityType::SHELL_RED: bbox = cv::Rect(-4, -3, 16, 14); break;
	/*case EntityType::PIRANHA: bbox = cv::Rect(); break;
	case EntityType::BRICK: bbox = cv::Rect(0, 0, 16, 16); break;
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
	case EntityType::MARIO_SMALL_L: detThresh = 150000; break;
	case EntityType::MARIO_SMALL_R: detThresh = 150000; break;
	case EntityType::MARIO_BIG_L: detThresh = 150000; break;
	case EntityType::MARIO_BIG_R: detThresh = 150000; break;
	case EntityType::MARIO_FIRE_L: detThresh = 150000; break;
	case EntityType::MARIO_FIRE_R: detThresh = 150000; break;
	case EntityType::GOOMBA: detThresh = 780000; break;
	case EntityType::KOOPA_L: detThresh = 150000; break;
	case EntityType::KOOPA_R: detThresh = 150000; break;
	case EntityType::PIPE: detThresh = 250000; break;
	case EntityType::QUESTION_Y: detThresh = 150000; break;
	case EntityType::QUESTION_O: detThresh = 150000; break;
	case EntityType::QUESTION_B: detThresh = 150000; break;
	case EntityType::BRICK: detThresh = 10000; break;
	case EntityType::KOOPA_RED_L: detThresh = 150000; break;
	case EntityType::KOOPA_RED_R: detThresh = 150000; break;
	case EntityType::SHELL: detThresh = 150000; break;
	case EntityType::CHISELED: detThresh = 150000; break;
	case EntityType::BEAM: detThresh = 150000; break;
	case EntityType::SHELL_RED: detThresh = 150000; break;
	/*case EntityType::PIRANHA: detThresh = 150000; break;
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
		holeColor = cv::Vec3b(252, 148, 92);
	}
	else {
		worldStr = "underworld";
		holeColor = cv::Vec3b(0, 0, 0);
	}

	std::cout << worldStr << std::endl;

	// Fill spriteTable
	spriteTable[EntityType::MARIO_SMALL_L] = cv::imread("sprites/mario/mario-small-left.png");
	spriteTable[EntityType::MARIO_SMALL_R] = cv::imread("sprites/mario/mario-small-right.png");
	spriteTable[EntityType::MARIO_BIG_L] = cv::imread("sprites/mario/mario-big-left.png");
	spriteTable[EntityType::MARIO_BIG_R] = cv::imread("sprites/mario/mario-big-right.png");;
	spriteTable[EntityType::MARIO_FIRE_L] = cv::imread("sprites/mario/mario-fire-left.png");
	spriteTable[EntityType::MARIO_FIRE_R] = cv::imread("sprites/mario/mario-fire-right.png");
	Entity::spriteTable[EntityType::GOOMBA] = cv::imread("sprites/enemies/" + worldStr + "/goomba-template.png");
	Entity::spriteTable[EntityType::KOOPA_L] = cv::imread("sprites/enemies/" + worldStr + "/koopa-l-template.png");
	Entity::spriteTable[EntityType::KOOPA_R] = cv::imread("sprites/enemies/" + worldStr + "/koopa-r-template.png");
	Entity::spriteTable[EntityType::KOOPA_RED_L] = cv::imread("sprites/enemies/shared/koopa-l-template.png");
	Entity::spriteTable[EntityType::KOOPA_RED_R] = cv::imread("sprites/enemies/shared/koopa-r-template.png");
	Entity::spriteTable[EntityType::SHELL] = cv::imread("sprites/enemies/" + worldStr + "/shell-template.png");
	Entity::spriteTable[EntityType::PIPE] = cv::imread("sprites/misc/shared/pipe-cropped.png");
	Entity::spriteTable[EntityType::CHISELED] = cv::imread("sprites/misc/" + worldStr + "/block-chiseled.png");
	Entity::spriteTable[EntityType::QUESTION_Y] = cv::imread("sprites/misc/" + worldStr + "/question1.png");
	Entity::spriteTable[EntityType::QUESTION_O] = cv::imread("sprites/misc/" + worldStr + "/question2.png");
	Entity::spriteTable[EntityType::QUESTION_B] = cv::imread("sprites/misc/" + worldStr + "/question3.png");
	Entity::spriteTable[EntityType::BRICK] = cv::imread("sprites/misc/" + worldStr + "/brick_smaller.png");
	spriteTable[EntityType::BEAM] = cv::imread("sprites/misc/shared/beam_cropped.png");
	Entity::spriteTable[EntityType::SHELL_RED] = cv::imread("sprites/enemies/shared/shell-template.png");
	/*spriteTable[EntityType::PIRANHA] = cv::imread("sprites/enemies/" + worldStr + "/goomba-template.png");
	spriteTable[EntityType::BRICK] = cv::imread("sprites/misc/" + worldStr + "/brick1.png");
	spriteTable[EntityType::QUESTION]
	spriteTable[EntityType::ROCK]
	spriteTable[EntityType::FLAGPOLE]
	
	spriteTable[EntityType::PIPE]
	spriteTable[EntityType::MUSHROOM]
	spriteTable[EntityType::FIREFLOWER]*/
}

Entity::Entity(cv::Point loc, EntityType type, int timeMS) {
	this->loc = loc;
	this->type = type;
	msLastSeen = timeMS;
	isInFrame = true;

	setBoundingBox();
}

Entity::Entity(cv::Rect box, EntityType type, int timeMS) {
	this->loc = cv::Point(box.x, box.y);
	this->type = type;
	msLastSeen = timeMS;
	isInFrame = true;

	bbox = cv::Rect(0, 0, box.width, box.height);
}

Entity::Entity(EntityType type) {
	this->loc = cv::Point(0, 0);
	this->type = type;
	msLastSeen = 0;
	isInFrame = false;

	setBoundingBox();
}

cv::Point Entity::getLoc() {
	return loc;
}

cv::Point Entity::getCenter() {
	return cv::Point(loc.x + bbox.x + bbox.width / 2, loc.y + bbox.y + bbox.height / 2);
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
	case EntityType::KOOPA_RED_R:
	case EntityType::KOOPA_RED_L:
		return true;
	default:
		return false;
	}
}

bool Entity::updateState(cv::Mat image, int timeMS) {
	cv::Mat result;
	cv::Point minLoc;
	cv::Point maxLoc;

	double minVal;
	double maxVal;
	int method = cv::TM_SQDIFF;

	int x_margin, y_margin;
	int x, y, width, height;

	if (type == EntityType::MARIO_SMALL_L ||
		type == EntityType::MARIO_SMALL_R ||
		type == EntityType::MARIO_BIG_L ||
		type == EntityType::MARIO_BIG_R ||
		type == EntityType::MARIO_FIRE_L ||
		type == EntityType::MARIO_FIRE_R) {
		x_margin = 10;
		y_margin = 10;
	}
	else if (type == EntityType::GOOMBA || type == EntityType::BEAM) {
		x_margin = 6;
		y_margin = 10;
	}
	else {
		x_margin = 5;
		y_margin = 5;
	}

	if (!isInFrame && (
		type == EntityType::MARIO_SMALL_L ||
		type == EntityType::MARIO_SMALL_R ||
		type == EntityType::MARIO_BIG_L   ||
		type == EntityType::MARIO_BIG_R   ||
		type == EntityType::MARIO_FIRE_L  ||
		type == EntityType::MARIO_FIRE_R)) {
		x = 0;
		y = 0;
		width = image.cols;
		height = image.rows;
	}
	else {
		x = std::max(0, bbox.x + loc.x - x_margin);
		width = std::min(bbox.width + x_margin * 2, image.cols - x);
		y = std::max(0, bbox.y + loc.y - y_margin);
		height = std::min(bbox.height + y_margin * 2, image.rows - y);
	}
	cv::Mat roi = image(cv::Rect(x, y, width, height));
	// cv::Mat roi = image.clone();
	if (type == EntityType::MARIO_SMALL_L ||
		type == EntityType::MARIO_SMALL_R ||
		type == EntityType::MARIO_BIG_L ||
		type == EntityType::MARIO_BIG_R ||
		type == EntityType::MARIO_FIRE_L ||
		type == EntityType::MARIO_FIRE_R) {
	}
	std::vector<EntityType> possStates = nextStates();

	for (EntityType tmpType : possStates) {
		// Create the result matrix
		int result_cols = roi.cols - spriteTable[tmpType].cols + 1;
		int result_rows = roi.rows - spriteTable[tmpType].rows + 1; 
		result.create(result_rows, result_cols, CV_32FC1);
		cv::Mat resultImg = result.clone();

		// Do the Matching and Normalize
		cv::matchTemplate(roi, spriteTable[tmpType], result, method);

		// Try and find the first enemy template
		cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

		if (minVal < getDetThresh(tmpType)) { // We found it
			if (!isInFrame && (
				type == EntityType::MARIO_SMALL_L ||
				type == EntityType::MARIO_SMALL_R ||
				type == EntityType::MARIO_BIG_L ||
				type == EntityType::MARIO_BIG_R ||
				type == EntityType::MARIO_FIRE_L ||
				type == EntityType::MARIO_FIRE_R)) {
				loc = minLoc;
			}
			else {
				loc = cv::Point(minLoc.x + loc.x + bbox.x - x_margin, minLoc.y + loc.y + bbox.y - y_margin);
			}
			type = tmpType;

			setBoundingBox();
			isInFrame = true;
			msLastSeen = timeMS;

			return true;
		}
	}

	isInFrame = false;
	return false;
}


bool Entity::inFrame() {
	return isInFrame;
}

std::vector<Entity> Entity::watch(cv::Mat image, std::vector<Entity> known, int timeMS, bool newWorldType) {
	std::vector<Entity> ret;

	// Find holes in the ground
	int startX = -1;
	int endX = -1;
	for (int i = 0; i < image.cols; i++) {
		while (image.at<cv::Vec3b>(205, i) == holeColor) {
			if (startX > -1) {
				endX = i;
			}
			else {
				startX = i;
			}

			if (++i >= image.cols) {
				break;
			}
		}

		if (startX > -1) {
			if (endX - startX > 16) {
				ret.push_back(Entity(cv::Rect(startX, 205, endX - startX, 5), EntityType::HOLE, timeMS));
			}
			startX = -1;
			endX = -1;
		}
	}

	cv::Mat result;
	cv::Point minLoc;
	cv::Point maxLoc;
	double minVal;
	double maxVal;
	int method = cv::TM_SQDIFF;
	int origWidth = image.size().width;
	int result_cols;
	int result_rows;

	// Search the entire image (other than top 40) for bricks
	cv::Mat brickImage = image(cv::Rect(0, 40, image.size().width, image.size().height - 40));

	// Create the result matrix
	result_cols = brickImage.cols - spriteTable[EntityType::BRICK].cols + 1;
	result_rows = brickImage.rows - spriteTable[EntityType::BRICK].rows + 1;
	result.create(result_rows, result_cols, CV_32FC1);

	// Do the Matching and Normalize
	cv::matchTemplate(brickImage, spriteTable[EntityType::BRICK], result, method);
	// Try and find the first enemy template
	while (true) {
		cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
		// minLoc = cv::Point(minLoc.x + spriteTable[t].cols - 1, minLoc.y + spriteTable[t].rows - 1);

		if (minVal < getDetThresh(EntityType::BRICK)) {
			cv::Point originLoc = minLoc;
			originLoc.y += 40;
			ret.push_back(Entity(originLoc, EntityType::BRICK, timeMS));
		}
		else {
			break;
		}

		result.at<float>(minLoc) = getDetThresh(EntityType::BRICK);
	}

	// Search the bottom and the top for beams
	cv::Mat beamSearchTop = image(cv::Rect(0, 24, image.size().width, 40)).clone();
	cv::Mat beamSearchBot = image(cv::Rect(0, image.size().height - 40, image.size().width, 40)).clone();
	
	// Create the result matrix
	result_cols = beamSearchTop.cols - spriteTable[EntityType::BEAM].cols + 1;
	result_rows = beamSearchTop.rows - spriteTable[EntityType::BEAM].rows + 1;
	result.create(result_rows, result_cols, CV_32FC1);

	// Remove currently known beams
	for (int i = 0; i < known.size(); i++) {
		if (known[i].getType() == EntityType::BEAM) {
			cv::Rect bbox = known[i].getBBox();
			bbox.y -= 24;
			cv::rectangle(beamSearchTop, bbox, cv::Scalar::all(255), cv::FILLED);
			bbox.y -= (image.size().height - 64);
			cv::rectangle(beamSearchBot, bbox, cv::Scalar::all(255), cv::FILLED);
		}
	}

	// Do the Matching and Normalize
	cv::Mat resultTop, resultBot;
	cv::Point minLocTop, minLocBot;
	cv::Point maxLocTop, maxLocBot;
	double minValTop, minValBot;
	double maxValTop, maxValBot;
	cv::matchTemplate(beamSearchTop, spriteTable[EntityType::BEAM], resultTop, method);
	cv::matchTemplate(beamSearchBot, spriteTable[EntityType::BEAM], resultBot, method);

	// Try and find the first beam template
	cv::minMaxLoc(resultTop, &minValTop, &maxValTop, &minLocTop, &maxLocTop, cv::Mat());
	cv::minMaxLoc(resultBot, &minValBot, &maxValBot, &minLocBot, &maxLocBot, cv::Mat());
	// minLoc = cv::Point(minLoc.x + spriteTable[t].cols - 1, minLoc.y + spriteTable[t].rows - 1);

	if (minValTop < getDetThresh(EntityType::BEAM)) { // We found one
		minLocTop.y += 24;
		ret.push_back(Entity(minLocTop, EntityType::BEAM, timeMS));
	}
	if (minValBot < getDetThresh(EntityType::BEAM)) { // We found one
		minLocBot.y += (image.size().height - 40);
		ret.push_back(Entity(minLocBot, EntityType::BEAM, timeMS));
	}

	// Now only look at the right (regular entities) unless we are in a new world
	int watchWidth = 40;
	if (newWorldType) {
		watchWidth = image.size().width;
	}
	image = image(cv::Rect(image.size().width - watchWidth, 40, watchWidth, image.size().height - 40));

	for (EntityType t = EntityType::GOOMBA; t != EntityType::SIZE_ENTITY_TYPE; t = static_cast<EntityType>(t + 1)) {
		if (t == EntityType::HOLE || t == EntityType::BEAM || t == EntityType::BRICK || t == EntityType::BEAM) {
			continue;
		}
		
		// Create the result matrix
		int result_cols = image.cols - spriteTable[t].cols + 1;
		int result_rows = image.rows - spriteTable[t].rows + 1;
		result.create(result_rows, result_cols, CV_32FC1);

		cv::Mat maskedImage = image.clone();
		for (int i = 0; i < known.size(); i++) {
			if (known[i].getType() == t) {
				cv::Rect bbox = known[i].getBBox();
				bbox.x -= (origWidth - maskedImage.cols);
				bbox.y -= 40;
				// std::cout << bbox.x << std::endl;
				cv::rectangle(maskedImage, bbox, cv::Scalar::all(255), cv::FILLED);
			}
		}

		// Do the Matching and Normalize
		cv::matchTemplate(maskedImage, spriteTable[t], result, method);

		while (true) {
			cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
			// minLoc = cv::Point(minLoc.x + spriteTable[t].cols - 1, minLoc.y + spriteTable[t].rows - 1);

			if (minVal < getDetThresh(t)) {
				cv::Point originLoc = minLoc;
				originLoc.x += (origWidth - maskedImage.cols);
				originLoc.y += 40;
				ret.push_back(Entity(originLoc, t, timeMS));
			}
			else {
				break;
			}

			result.at<float>(minLoc) = getDetThresh(t);
		}
	}

	return ret;
}

int Entity::timeLastSeen() {
	return msLastSeen;
}

cv::Mat Entity::spriteTable[EntityType::SIZE_ENTITY_TYPE];

std::vector<EntityType> Entity::nextStates() {
	std::vector<EntityType> ret;
	switch (type) {
	case EntityType::MARIO_SMALL_L:
		ret.push_back(EntityType::MARIO_SMALL_L);
		ret.push_back(EntityType::MARIO_SMALL_R);
		ret.push_back(EntityType::MARIO_BIG_L);
		break;
	case EntityType::MARIO_SMALL_R:
		ret.push_back(EntityType::MARIO_SMALL_R);
		ret.push_back(EntityType::MARIO_SMALL_L);
		ret.push_back(EntityType::MARIO_BIG_R);
		break;
	case EntityType::MARIO_BIG_L:
		ret.push_back(EntityType::MARIO_BIG_L);
		ret.push_back(EntityType::MARIO_BIG_R);
		ret.push_back(EntityType::MARIO_SMALL_L);
		ret.push_back(EntityType::MARIO_FIRE_L);
		break;
	case EntityType::MARIO_BIG_R:
		ret.push_back(EntityType::MARIO_BIG_R);
		ret.push_back(EntityType::MARIO_BIG_L);
		ret.push_back(EntityType::MARIO_SMALL_R);
		ret.push_back(EntityType::MARIO_FIRE_R);
		break;
	case EntityType::MARIO_FIRE_L:
		ret.push_back(EntityType::MARIO_FIRE_L);
		ret.push_back(EntityType::MARIO_FIRE_R);
		ret.push_back(EntityType::MARIO_SMALL_L);
		break;
	case EntityType::MARIO_FIRE_R:
		ret.push_back(EntityType::MARIO_FIRE_R);
		ret.push_back(EntityType::MARIO_FIRE_L);
		ret.push_back(EntityType::MARIO_SMALL_R);
		break;
	case EntityType::GOOMBA:
		ret.push_back(EntityType::GOOMBA);
		break;
	case EntityType::KOOPA_L:
		ret.push_back(EntityType::KOOPA_L);
		ret.push_back(EntityType::KOOPA_R);
		ret.push_back(EntityType::SHELL);
		break;
	case EntityType::KOOPA_R:
		ret.push_back(EntityType::KOOPA_R);
		ret.push_back(EntityType::KOOPA_L);
		ret.push_back(EntityType::SHELL);
		break;
	case EntityType::KOOPA_RED_L:
		ret.push_back(EntityType::KOOPA_RED_L);
		ret.push_back(EntityType::KOOPA_RED_R);
		ret.push_back(EntityType::SHELL_RED);
		break;
	case EntityType::KOOPA_RED_R:
		ret.push_back(EntityType::KOOPA_RED_R);
		ret.push_back(EntityType::KOOPA_RED_L);
		ret.push_back(EntityType::SHELL_RED);
		break;
	case EntityType::SHELL:
		ret.push_back(EntityType::SHELL);
		break;
	case EntityType::SHELL_RED:
		ret.push_back(EntityType::SHELL_RED);
		break;
	case EntityType::PIPE:
		ret.push_back(EntityType::PIPE);
		break;
	case EntityType::CHISELED:
		ret.push_back(EntityType::CHISELED);
		break;
	case EntityType::QUESTION_Y:
		ret.push_back(EntityType::QUESTION_Y);
		ret.push_back(EntityType::QUESTION_O);
		break;
	case EntityType::QUESTION_O:
		ret.push_back(EntityType::QUESTION_O);
		ret.push_back(EntityType::QUESTION_B);
		ret.push_back(EntityType::QUESTION_Y);
		break;
	case EntityType::QUESTION_B:
		ret.push_back(EntityType::QUESTION_B);
		ret.push_back(EntityType::QUESTION_O);
		break;
	case EntityType::BRICK:
		ret.push_back(EntityType::BRICK);
		break;
	case EntityType::BEAM:
		ret.push_back(EntityType::BEAM);
		break;
	}
	return ret;
}
