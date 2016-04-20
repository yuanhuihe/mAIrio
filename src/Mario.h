#include "Entity.h"

enum MarioType {
	SMALL_L = 0,
	SMALL_R,
	BIG_L,
	BIG_R,
	FIRE_L,
	FIRE_R,
	DEAD,
	SIZE_MARIO_TYPE
};

class Mario : public Entity {
private:
	cv::Rect findMarioInFrame(cv::Mat image, int match_method);
	void loadMarioTemplates();
	MarioType m;
	std::vector<cv::Mat> marioTemplates;
	std::vector<cv::Rect> marioBoundingBoxes;
	std::vector<float> marioThresholds;

	static const bool transTable[MarioType::SIZE_MARIO_TYPE][MarioType::SIZE_MARIO_TYPE];
public:
	cv::Rect findMario(cv::Mat input);
	Mario();
};