#ifndef Camera_H
#define Camera_H

#include <glm/glm.hpp>
#include "Transform.h"

class Camera {
public:
	Camera();
	~Camera();
	glm::vec3 background;
	Transform* transform;
};

#endif