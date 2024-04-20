#include "../lib/Transform.h"

Transform::Transform()
{
	localPosition = glm::vec3(0.f, 0.f, 0.f);
	localRotation = glm::vec3(0.f, 0.f, 0.f);
	localScale = glm::vec3(1.f, 1.f, 1.f);
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

YAML::Node nodeVec3(glm::vec3 vector)
{
	YAML::Node node;
	node["x"] = vector.x;
	node["y"] = vector.y;
	node["z"] = vector.z;
	return node;
}

YAML::Node Transform::serialize() 
{
	YAML::Node node;
	node["position"] = nodeVec3(localPosition);
	node["rotation"] = nodeVec3(localRotation);
	node["scale"] = nodeVec3(localScale);
	return node;
}
