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
	std::string name;
public:
	bool p_LoopSound = false;
	AudioSource(std::string name, ALuint source);
	~AudioSource();


	void play();
	void stop();
	void setVolume(float volume);
	void update(Transform* transform);
};

#endif
