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

	bool transTable[MarioType::SIZE_MARIO_TYPE][MarioType::SIZE_MARIO_TYPE] = {
		// SMALL_L, SMALL_R, BIG_L, BIG_R, FIRE_L, FIRE_R, DEAD
		/*SMALL_L*/{ true,    true,    true,  false, false,  false,  true },
		/*SMALL_R*/{ true,    true,    false, true,  false,  false,  true },
		/*BIG_L*/{ true,    false,   true,  true,  true,   false,  true },
		/*BIG_R*/{ false,   true,    true,  true,  false,  true,   true },
		/*FIRE_L*/{ false,   false,   true,  false, true,   true,   true },
		/*FIRE_R*/{ false,   true,    false, true,  true,   true,   true },
		/*DEAD*/{ false,   true,    false, false, false,  false,  true }
	};
public:
	cv::Rect findMario(cv::Mat input);
	Mario();
};