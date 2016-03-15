#pragma once

#include <vector>

#include <opencv2/core.hpp>

enum WorldType {
	OVERWORLD,
	UNDERWORLD
};

enum EntityType {
	MARIO,
	GOOMBA,
	KOOPA,
	SHELL,
	PIRANHA,
	BRICK,
	QUESTION,
	ROCK, // Includes chiseled block and used block
	FLAGPOLE,
	BEAM,
	PIPE,
	MUSHROOM,
	FIREFLOWER
};

class Entity {
protected:
	cv::Point loc;
	cv::Size size;
	std::vector<cv::Mat> sprites;
	EntityType type;

public:
	Entity(cv::Point loc, cv::Size size, EntityType type);

	cv::Point getLoc();
	cv::Size getSize();
	cv::Rect getRect();
	std::vector<cv::Mat> getSprites();
	EntityType getType();

	void setLoc(cv::Point loc);
	void setSize(cv::Size size);
	void setType(EntityType type);

	bool isPassable();
	bool isHostile();
};