#include "../lib/Transform.h"

Transform::Transform()
{
	localPosition = glm::vec3(0.f, 0.f, 0.f);
	localRotation = glm::vec3(0.f, 0.f, 0.f);
	localScale = glm::vec3(1.f, 1.f, 1.f);
}

glm::vec3 nodeToVec3(YAML::Node node) {
	return glm::vec3(node["x"].as<float>(), node["y"].as<float>(), node["z"].as<float>());
}

Transform::Transform(YAML::Node node) {
	this->localPosition = nodeToVec3(node["position"]);
	this->localRotation = nodeToVec3(node["rotation"]);
	this->localScale = nodeToVec3(node["scale"]);
}

Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	localPosition = position;
	localRotation = rotation;
	localScale = scale;
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

glm::mat4 Transform::getMatrix() {
	glm::mat4 M = glm::translate(glm::mat4(1.f), localPosition);
	M = glm::rotate(M, glm::radians(localRotation.y), glm::vec3(0.f, 1.f, 0.f));
	M = glm::rotate(M, glm::radians(localRotation.x), glm::vec3(1.0f, 0.f, 0.f));
	M = glm::rotate(M, glm::radians(localRotation.z), glm::vec3(0.f, 0.f, 1.f));
	M = glm::scale(M, localScale);
	return M;
}

glm::mat4 Transform::getPredictedMatrix() {
	glm::mat4 M = glm::translate(glm::mat4(1.f), predictedPosition);
	M = glm::rotate(M, glm::radians(localRotation.y), glm::vec3(0.f, 1.f, 0.f));
	M = glm::rotate(M, glm::radians(localRotation.x), glm::vec3(1.0f, 0.f, 0.f));
	M = glm::rotate(M, glm::radians(localRotation.z), glm::vec3(0.f, 0.f, 1.f));
	M = glm::scale(M, localScale);
	return M;
}
