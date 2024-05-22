#ifndef TimeManager_H
#define TimeManager_H

class TimeManager {
public:
	float time;

	TimeManager() {
		time = 0.1f;
	};
	~TimeManager();

	void setTime(float newTime) {
		time = newTime;
	}

	float getFramePerSeconds() {
		return time;
	}
};

#endif