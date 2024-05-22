#ifndef Enemy_H
#define Enemy_H

#include "GameObject.h"

enum class EnemyState {
	Idle,
	Walking,
	Chopping,
	Attacking
};

class Enemy : public GameObject {
public:
	glm::vec3 chosenTreePos = glm::vec3(0.f);
	float timeSpentWalking = 0.f;
	int sector = 0;
	EnemyState state = EnemyState::Idle;
	glm::vec3 velocity = glm::vec3(0.f);
	std::pair <float, float> velLimits;
	float timeSinceDirChange = 0.f;
	Enemy(std::string name, glm::vec3 pos, glm::vec3 scale, glm::vec3 rot, std::pair<float,float> velLimits) : GameObject(name) {
		this->localTransform->localPosition = pos;
		this->localTransform->localScale = scale;
		this->localTransform->localRotation = rot;
		this->velLimits = velLimits;
	}
	~Enemy() {}

	void setVel(glm::vec3 direction) {
		ZoneScopedN("setVel");
		glm::vec3 directionToTree = glm::normalize(chosenTreePos - localTransform->localPosition);
		if (glm::dot(direction, directionToTree) < 0.0f) {
			direction = directionToTree;
		}
		else {
			direction = glm::normalize(direction + directionToTree);
		}
		velocity = direction * (velLimits.second - velLimits.first);
	}

	void setVel2(std::vector<GameObject*> collisions) {
		ZoneScopedN("setVel2");
		glm::vec3 avoid = glm::vec3(0.f);
		for (auto& collision : collisions) {
			glm::vec3 toCollision = collision->localTransform->localPosition - localTransform->localPosition;
			float distance = glm::length(toCollision);
			if (collision->name.starts_with("enemy") && distance > 0.1f) {
				toCollision = glm::vec3(toCollision.z, toCollision.y, -toCollision.x);
				avoid -= glm::normalize(toCollision) / distance;
			}
			else if (distance > 0.1f) {
				avoid -= glm::normalize(toCollision) / distance;
			}
			
		}
		if (!collisions.empty()) {
			avoid = glm::normalize(avoid);
			velocity = glm::normalize(velocity + avoid) * (velLimits.second - velLimits.first);
		}
		if ((glm::any(glm::isnan(velocity)) || glm::any(glm::isinf(velocity)))) {
			velocity = glm::vec3(0.f);
		}
	}
};
#endif