#include "../lib/Camera.h"
Camera::Camera() {
	transform = new Transform();
}
Camera::~Camera() {
	delete transform;
}

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(transform->getLocalPosition(), transform->getLocalPosition() + front, up);
}


void Camera::updateCamera(glm::vec2 vector) {
	glm::vec3 Front;
	transform->localRotation += glm::vec3(vector.x/300,-vector.y/300, 0);
	Front.x = cos(transform->localRotation.x ) * cos(transform->localRotation.y);
	Front.y = sin(transform->localRotation.y);
	Front.z = sin(transform->localRotation.x) * cos(transform->localRotation.y);
	front = glm::normalize(Front);
	right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
	up = glm::normalize(glm::cross(right, front));
	
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = .05f * deltaTime;
	if (direction == FORWARD)
		transform->localPosition += front * velocity;
	if (direction == BACKWARD)
		transform->localPosition -= front * velocity;
	if (direction == LEFT)
		transform->localPosition -= right * velocity;
	if (direction == RIGHT)
		transform->localPosition += right * velocity;
}