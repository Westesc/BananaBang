#ifndef Camera_H
#define Camera_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Transform.h"
#include "../thirdparty/tracy/public/tracy/Tracy.hpp"

const float YAW = 90.0f;
const float PITCH = -10.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 90.0f;

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera {
public:
	glm::vec3 background;
	glm::vec3 right;
	glm::vec3 up;
	glm::vec3 front;
	Transform* transform;
	float Zoom = ZOOM;

	Camera();
	~Camera();
	glm::mat4 getViewMatrix();
	void updateCamera(glm::vec2 vector);
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
};

#endif