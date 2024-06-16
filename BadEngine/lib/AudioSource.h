#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H

#include <al/al.h>
#include <al/alc.h>
#include <glm/glm.hpp>
#include "Transform.h"

class AudioSource : Component {
private:
	unsigned int source;
public:
	
	void update(Transform* Transform);
};

#endif
