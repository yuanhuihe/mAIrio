#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "Entity.h"

Entity::Entity(cv::Point loc, cv::Size size, EntityType type) {
	this->loc = loc;
	this->size = size;
	this->type = type;
}

cv::Point Entity::getLoc() {
	return loc;
}

cv::Size Entity::getSize() {
	return size;
}

cv::Rect Entity::getRect() {
	return cv::Rect(loc.x, loc.y, size.width, size.height);
}

std::vector<cv::Mat> Entity::getSprites() {
	return sprites;
}

EntityType Entity::getType() {
	return type;
}

void Entity::setLoc(cv::Point loc) {
	this->loc = loc;
}

void Entity::setSize(cv::Size size) {
	this->size = size;
}

void Entity::setType(EntityType type) {
	this->type = type;
}

bool Entity::isPassable() { // How to handle breakable bricks?
	switch (type) {
	case EntityType::MARIO:
	case EntityType::SHELL:
	case EntityType::FLAGPOLE:
	case EntityType::MUSHROOM:
	case EntityType::FIREFLOWER:
		return true;
	default:
		return false;
	}
}

bool Entity::isHostile() { // Technically should also return true for a moving shell
	switch (type) {
	case EntityType::GOOMBA:
	case EntityType::KOOPA:
	case EntityType::PIRANHA:
		return true;
	default:
		return false;
	}
}