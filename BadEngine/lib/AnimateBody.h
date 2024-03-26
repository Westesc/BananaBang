#ifndef AnimateBody_H
#define AnimateBody_H

#include "Component.h"

class AnimateBody : Component {
public:
	AnimateBody();
	~AnimateBody();

	float time;
	int Animation; //temp

	void play();
	bool isPlay();
	void reset();
};

#endif
