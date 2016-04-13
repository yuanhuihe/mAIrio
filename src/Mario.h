#include "Entity.h"

enum MarioType {
	SMALL,
	BIG,
	FIRE,
	DEAD
};

class Mario : public Entity {
protected:
	MarioType m;
public:
	cv::Rect update();
};