#pragma once

#include <vector>

#include <opencv2/core.hpp>

enum WorldType {
	OVERWORLD,
	UNDERWORLD
};

enum EntityType {
	MARIO = 0,
	GOOMBA,
	KOOPA_L,
	KOOPA_R,
	KOOPA_RED_L,
	KOOPA_RED_R,
	SHELL,
	SHELL_RED,
	PIRANHA, // We're going to check color above pipes to check for Piranhas, not template matching
	BRICK,
	QUESTION,
	ROCK, // Includes chiseled block and used block
	FLAGPOLE,
	BEAM,
	PIPE,
	MUSHROOM,
	FIREFLOWER,
	SIZE_ENTITY_TYPE
};

static cv::Mat spriteTable[EntityType::SIZE_ENTITY_TYPE]; // Global for now
void fillSpriteTable(WorldType world);

class Entity {
protected:
	cv::Point loc;
	cv::Rect bbox;
	EntityType type;
	int detThresh;
	void setDetThresh();
	void setBoundingBox();

public:
	Entity(cv::Point loc, EntityType type);

	cv::Point getLoc();
	cv::Rect getBBox();
	cv::Mat getSprite();
	int getDetThresh();
	EntityType getType();

	void setLoc(cv::Point loc);
	void setType(EntityType type);

	bool isPassable();
	bool isHostile();
};