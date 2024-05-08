#ifndef Camera_H
#define Camera_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Transform.h"
#include "SceneManager.h"

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
	SceneManager* sm;
	glm::vec3 background;
	glm::vec3 right;
	glm::vec3 up;
	glm::vec3 front;
	Transform* transform;
	float Zoom = ZOOM;
	float Yaw = 0.f;
	float Pitch = 0.f;
	float sensitivity = 0.05f;
	float x, y, z;

	Camera(SceneManager* sm);
	~Camera();
	glm::mat4 getViewMatrix();
	glm::mat4 getViewMatrixPlayer();
	glm::vec3 getFront();
	void updateCamera(glm::vec2 vector);
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
};

#endif