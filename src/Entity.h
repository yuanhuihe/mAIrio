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

class Entity {
protected:
	cv::Point loc;
	cv::Rect bbox;
	EntityType type;
	void setBoundingBox();

public:
	Entity(cv::Point loc, EntityType type);
	static int getDetThresh(EntityType type);
	static cv::Mat spriteTable[EntityType::SIZE_ENTITY_TYPE]; // Global for now
	static void fillSpriteTable(WorldType world);

	cv::Point getLoc();
	cv::Rect getBBox();
	cv::Mat getSprite();
	EntityType getType();

	void setLoc(cv::Point loc);
	void setType(EntityType type);

	bool isPassable();
	bool isHostile();
};