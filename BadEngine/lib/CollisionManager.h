#ifndef CollisionManager_H
#define CollisionManager_H

#include <vector>
#include "Section.h"

class CollisionManager {
public:
	std::vector<Section*> sections;
	float sectionSize;

	CollisionManager(float worldSize, float sectionSize) : sectionSize(sectionSize) {
		int numSections = worldSize / sectionSize;
		for (int i = 0; i < numSections; i++) {
			for (int j = 0; j < numSections; j++) {
				for (int k = 0; k < numSections; k++) {
					sections.push_back(new Section(i * sectionSize, j * sectionSize, k * sectionSize, sectionSize));
				}
			}
		}
	}
	~CollisionManager() {
		for (auto section : sections) {
			delete section;
		}
	}

	void addObject(GameObject* go) {
		for (auto section : sections) {
			section->objects.clear();
			if (section->checkCollision(go->getModelComponent())) {
				section->objects.push_back(go);
			}
		}
	}

	void checkResolveCollisions(std::vector<GameObject*> gameObjects, float deltaTime) {
		for (auto section : sections) {
			for (int i = 0; i < section->objects.size(); i++) {
				for (int j = i + 1; j < section->objects.size(); j++) {
					section->objects.at(i)->checkResolveCollisions(section->objects.at(j), deltaTime);
				}
			}
		}
		/*for (int i = 0; i < gameObjects.size(); i++) {
			for (int j = i + 1; j < gameObjects.size(); j++) {
				if (checkCollision(go->getBoundingBox(), gameObjects[i]->getBoundingBox())) {
					resolveCollision(go, gameObjects[i]);
				}
				gameObjects.at(i)->checkResolveCollisions(gameObjects.at(j), deltaTime);
			}
		}*/
	}

	/*static bool checkCollision(BoundingBox* box1, BoundingBox* box2) {
		if (box1->max.x < box2->min.x || box1->min.x > box2->max.x) {
			return false;
		}
		if (box1->max.y < box2->min.y || box1->min.y > box2->max.y) {
			return false;
		}
		if (box1->max.z < box2->min.z || box1->min.z > box2->max.z) {
			return false;
		}
		return true;
	}
	static void resolveCollision(GameObject* go1, GameObject* go2) {
		glm::vec3 direction = go1->getTransform()->position - go2->getTransform()->position;
		float distance = glm::length(direction);
		glm::vec3 collisionDirection = glm::normalize(direction);
		go1->Move(collisionDirection * 0.01f);
		go2->Move(-collisionDirection * 0.01f);
	}

	bool checkCollision(Model* first, Model* second) {
		if (boundingBox != nullptr) {
			if (other->boundingBox != nullptr) {
				return checkBoundingBoxCollision(*boundingBox, *other->boundingBox, *Transform, *other->getTransform());
			}
			else if (other->capsuleCollider != nullptr) {
				return checkBoxCapsuleCollision(*boundingBox, *other->capsuleCollider, *other->getTransform(), *Transform);
			}
		}
		else if (capsuleCollider != nullptr) {
			if (other->boundingBox != nullptr) {
				return checkBoxCapsuleCollision(*other->boundingBox, *capsuleCollider, *getTransform(), *other->getTransform());
			}
			else if (other->capsuleCollider != nullptr) {
				return checkCapsuleCollision(*capsuleCollider, *other->capsuleCollider, *getTransform(), *other->getTransform());
			}
		}

		return false;
	}*/
};

#endif
