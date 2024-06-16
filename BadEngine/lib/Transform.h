#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <yaml-cpp/yaml.h>
#include "Component.h"
#include "Serialize.h"

class Transform {
public:
	Transform();
	Transform(YAML::Node node);
	Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	~Transform();

	glm::vec3 localPosition;
	glm::vec3 localRotation;
	glm::vec3 localScale;
	glm::vec3 predictedPosition = glm::vec3(0.0f);

	void Translate(glm::vec3 vector);
	void Rotate(glm::vec3 vector);
	void Scale(glm::vec3 vector);
	glm::vec3 getGlobalPosition();
	glm::vec3 getGlobalRotation();
	glm::vec3 getGlobalScale();
	glm::vec3 getLocalPosition();
	glm::vec3 getLocalRotation();
	glm::vec3 getLocalScale();
	YAML::Node serialize();
	glm::mat4 getMatrix();
	glm::mat4 getPredictedMatrix();

	friend Transform operator+(const Transform& lhs, const Transform& rhs) {
		Transform result(lhs); 
		result.localPosition += rhs.localPosition;
		result.localRotation += rhs.localRotation;
		result.localScale += rhs.localScale;
		return result;
	}
};

#endif
