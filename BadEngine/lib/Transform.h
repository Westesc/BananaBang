#ifndef Transform_H
#define Transform_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Component.h"

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
};

#endif
