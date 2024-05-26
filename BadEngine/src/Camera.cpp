#include "../lib/Camera.h"
#include "../lib/SceneManager.h"

Camera::Camera(SceneManager* sm) {
	transform = new Transform();
	//transform->localPosition = sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition();
	this->sm = sm;
}
Camera::~Camera() {
	delete transform;
}

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(transform->getLocalPosition(), transform->getLocalPosition() + front, up);
}

glm::mat4 Camera::getViewMatrixPlayer() {
	glm::vec3 playerPosition = sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition();

	transform->localPosition.x = playerPosition.x + 10.f * cos(glm::radians(Yaw));
	transform->localPosition.y = playerPosition.y + 10.f * sin(glm::radians(Pitch));
	transform->localPosition.z = playerPosition.z + 10.f * sin(glm::radians(Yaw));

	return glm::lookAt(transform->localPosition, playerPosition, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 Camera::getFront() {
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front = glm::normalize(front);
	return front;
}

void Camera::updateCamera(glm::vec2 vector) {
	Yaw += vector.x * sensitivity;
	Pitch += vector.y * sensitivity;
	if (Pitch > 89.0f)
		Pitch = 89.0f;
	if (Pitch < 0.0f)
		Pitch = 0.0f;

	glm::vec3 Front;
	transform->localRotation += glm::vec3(vector.x / 300, -vector.y / 300, 0);
	if (transform->localRotation.y > 1.56)
		transform->localRotation.y = 1.56;
	else if (transform->localRotation.y < -1.56)
		transform->localRotation.y = -1.56;
	Front.x = cos(transform->localRotation.x) * cos(transform->localRotation.y);
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