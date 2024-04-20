#ifndef Transform_H
#define Transform_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <yaml-cpp/yaml.h>
#include "Component.h"
#include "Serialize.h"

class Transform : Component {
public:
	Transform();
	~Transform();

	glm::vec3 localPosition;
	glm::vec3 localRotation; //a to nie powinien byæ vec4?
	glm::vec3 localScale;

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

	friend Transform operator+(const Transform& lhs, const Transform& rhs) {
		Transform result(lhs); 
		result.localPosition += rhs.localPosition;
		result.localRotation += rhs.localRotation;
		result.localScale += rhs.localScale;
		return result;
	}
};

#endif
