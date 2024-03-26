#include "../lib/Transform.h"

Transform::Transform()
{
}

Transform::~Transform()
{
}

void Transform::Translate(glm::vec3 vector)
{
}

void Transform::Rotate(glm::vec3 vector)
{
}

void Transform::Scale(glm::vec3 vector)
{
}

glm::vec3 Transform::getGlobalPosition()
{
	return glm::vec3();
}

glm::vec3 Transform::getGlobalRotation()
{
	return glm::vec3();
}

glm::vec3 Transform::getLocalPosition()
{
	return localPosition;
}

glm::vec3 Transform::getLocalRotation()
{
	return localRotation;
}

glm::vec3 Transform::getLocalScale()
{
	return localScale;
}

glm::vec3 Transform::getGlobalScale()
{
	return glm::vec3();
}
