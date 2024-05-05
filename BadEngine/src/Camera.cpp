#include "../lib/Camera.h"
#include "../lib/SceneManager.h"

Camera::Camera(SceneManager* sm) {
	transform = new Transform();
	transform->localRotation = sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition();
	this->sm = sm;
}
Camera::~Camera() {
	delete transform;
}

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(transform->getLocalPosition(), transform->getLocalPosition() + front, up);
}

glm::mat4 Camera::GetViewMatrix() {
	glm::vec3 playerPosition = sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition();

	glm::vec3 cameraPosition;
	cameraPosition.x = playerPosition.x + 10.f * cos(glm::radians(Yaw));
	cameraPosition.y = playerPosition.y + 10.f * sin(glm::radians(Pitch));
	cameraPosition.z = playerPosition.z + 10.f * sin(glm::radians(Yaw));

	glm::mat4 viewMatrix = glm::lookAt(cameraPosition, playerPosition, glm::vec3(0.0f, 1.0f, 0.0f));

	return viewMatrix;
}

void Camera::updateCamera(glm::vec2 vector) {
	Yaw += vector.x * sensitivity;
	Pitch += vector.y * sensitivity;
	std::cout << "YAW: " << Yaw << ", Pitch" << Pitch << std::endl;
	if (Pitch > 89.0f)
		Pitch = 89.0f;
	if (Pitch < -10.0f)
		Pitch = -10.0f;

	glm::vec3 Front;
	transform->localRotation += glm::vec3(vector.x / 300, -vector.y / 300, 0);
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