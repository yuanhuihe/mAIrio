#pragma once

#include <vector>

#include <opencv2/core.hpp>

enum WorldType {
	OVERWORLD,
	UNDERWORLD
};

enum EntityType {
	GOOMBA = 0,
	KOOPA_L,
	KOOPA_R,
	//KOOPA_RED_L,
	//KOOPA_RED_R,
	SHELL,
	/*SHELL_RED,
	PIRANHA, // We're going to check color above pipes to check for Piranhas, not template matching
	BRICK,
	QUESTION,
	ROCK, // Includes chiseled block and used block
	FLAGPOLE,
	BEAM,
	PIPE,
	MUSHROOM,
	FIREFLOWER,*/
	SIZE_ENTITY_TYPE
};

class Entity {
protected:
	cv::Point loc;
	cv::Rect bbox;
	EntityType type;
	void setBoundingBox();

	static const bool transTable[EntityType::SIZE_ENTITY_TYPE][EntityType::SIZE_ENTITY_TYPE];

public:
	Entity(cv::Point loc, EntityType type);
	Entity::Entity();
	static int getDetThresh(EntityType type);
	static cv::Mat spriteTable[EntityType::SIZE_ENTITY_TYPE];
	static void fillSpriteTable(WorldType world);

	cv::Point getLoc();
	cv::Rect getBBox();
	cv::Mat getSprite();
	EntityType getType();
	bool updateState(cv::Mat frame);

	void setLoc(cv::Point loc);
	void setType(EntityType type);

	bool isPassable();
	bool isHostile();
};