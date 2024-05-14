#ifndef Enemy_H
#define Enemy_H

#include "GameObject.h"

class Enemy : public GameObject {
public:
	glm::vec3 chosenTreePos = glm::vec3(0.f);
	Enemy(std::string name, glm::vec3 pos, glm::vec3 scale, glm::vec3 rot) : GameObject(name) {
		this->localTransform->localPosition = pos;
		this->localTransform->localScale = scale;
		this->localTransform->localRotation = rot;
	}
	~Enemy() {}
};
#endif