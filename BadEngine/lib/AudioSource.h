#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H

#include <al/al.h>
#include <al/alc.h>
#include <glm/glm.hpp>
#include "Component.h"
#include "Transform.h"

class AudioSource : Component {
private:
	ALuint source;
public:
	bool p_LoopSound = false;
	std::string name;
	AudioSource(std::string name, ALuint source);
	~AudioSource();


	void play();
	void playConst();
	void stop();
	void setVolume(float volume);
	void update(Transform* transform);
};

#endif
