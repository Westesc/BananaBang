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
		/*if (state == EnemyState::Walking && glm::cos(glm::dot(velocity, direction)) < 0.0f) {
			velocity *= 0.5f;
			timeSpentWalking *= 2;
		}
		else if (glm::length(direction) < velLimits.first) {
			velocity = glm::normalize(direction) * velLimits.first;
		}
		else if (glm::length(direction) > velLimits.second) {
			velocity = glm::normalize(direction) * velLimits.second;
		}
		else {
			velocity = direction;
		}*/
		glm::vec3 directionToTree = glm::normalize(chosenTreePos - localTransform->localPosition);
		if (glm::dot(direction, directionToTree) < 0.0f) {
			direction = directionToTree;
		}
		else {
			direction = glm::normalize(direction + directionToTree);
		}
		velocity = direction * (velLimits.second - velLimits.first);
		/*
		if (glm::dot(velocity, direction) > 0.99f) {
			velocity = direction * (velLimits.second - velLimits.first);
		}
		else if (timeSinceDirChange > 1.f) {
			velocity = direction * (velLimits.second - velLimits.first);
			timeSinceDirChange = 0.f;
		}
		else {
			timeSinceDirChange = 0.f;
		}*/
	}
};
#endif