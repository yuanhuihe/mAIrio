#include "Entity.h"

enum MarioType {
	SMALL_R = 0,
	SMALL_L,
	BIG_R,
	BIG_L,
	FIRE_R,
	FIRE_L,
	DEAD,
	SIZE_MARIO_TYPE
};

class Mario : public Entity {
protected:
	// cv::Point loc;
	// cv::Rect bbox;
	MarioType type;
	void setBoundingBox();
	static void fillSpriteTable();
	// bool isInFrame;
	// int msLastSeen;

	// static const bool transTable[MarioType::SIZE_MARIO_TYPE][MarioType::SIZE_MARIO_TYPE];

public:
	Mario::Mario(cv::Point loc, MarioType type, int timeMS);
	Mario::Mario();
	static int getDetThresh(MarioType type);
	// static cv::Mat spriteTable[MarioType::SIZE_MARIO_TYPE];
	static Mario watch(cv::Mat image, int timeMS);

	// cv::Point getLoc();
	// cv::Rect getBBox();
	// cv::Mat getSprite();
	MarioType getType();
	bool updateState(cv::Mat frame, int timeMS);

	// void setLoc(cv::Point loc);
	// void setType(MarioType type);

	// bool isPassable();
	// bool isHostile();
	// bool inFrame();
	int timeLastSeen();
};