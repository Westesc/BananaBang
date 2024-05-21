#ifndef CollisionManager_H
#define CollisionManager_H

#include <vector>
#include "Section.h"
#include <cmath>
#include "Enemy.h"

class CollisionManager {
public:
	std::vector<Section*> sections;
	float sectionSize;

	CollisionManager(float worldSize, float sectionSize) : sectionSize(sectionSize) {
		int numSections = worldSize / sectionSize;
		int IDcounter = 0;
		for (int i = -numSections*0.5f; i < numSections*0.5f; i++) {
			for (int j = -numSections * 0.5f; j < numSections * 0.5f; j++) { //TODO: ograniczyæ wysokoœæ do realnych potrzeb
				for (int k = -numSections * 0.5f; k < numSections * 0.5f; k++) {
					sections.push_back(new Section(i * sectionSize, j * sectionSize, k * sectionSize, sectionSize, IDcounter));
					IDcounter++;
				}
			}
		}
	}
	~CollisionManager() {
		for (auto section : sections) {
			delete section;
		}
	}

	void addObject(GameObject* go) { //TODO: dla obiektów, które zostan¹ dodane do sektora, sprawdziæ tylko przylegaj¹ce sektory
		for (auto section : sections) {
			auto it = std::find(section->objects.begin(), section->objects.end(), go);
			if (it != section->objects.end()) {
				section->objects.erase(it);
			}
			if (section->checkCollision(go)) {
				section->objects.push_back(go);
			}
		}
	}

	void addStaticObject(GameObject* go) {
		for (auto section : sections) {
			if (section->checkCollision(go)) {
				section->staticObjects.push_back(go);
			}
		}
	}

	void addObjectPredict(GameObject* go) {
		for (auto section : sections) {
			if (section->checkCollision(go)) {
				section->predictObject = go;
			}
			else {
				section->predictObject = nullptr;
			}
		}
	}

	std::vector<GameObject*> checkPrediction() {
		std::vector<GameObject*> collisions;
		for (auto section : sections) {
			if (section->predictObject != nullptr) {
				for (int i = 0; i < section->objects.size(); i++) {
					if (section->predictObject != section->objects.at(i) && checkCollision(section->objects.at(i), section->predictObject)) {
						collisions.push_back(section->objects.at(i));
					}
				}
				for (int i = 0; i < section->staticObjects.size(); i++) {
					if (checkCollision(section->staticObjects.at(i), section->predictObject)) {
						collisions.push_back(section->staticObjects.at(i));
					}
				}
			}
		}
		return collisions;
	}

	void checkResolveCollisions(float deltaTime) {
		for (auto section : sections) {
			for (int i = 0; i < section->objects.size(); i++) {
				for (int j = 0; j < section->staticObjects.size(); j++) {
					if (checkCollision(section->objects.at(i), section->staticObjects.at(j))) {
						resolveCollisionStatic(section->objects.at(i), section->staticObjects.at(j), deltaTime);
					}
				}
				for (int j = i + 1; j < section->objects.size(); j++) {
					if (checkCollision(section->objects.at(i), section->objects.at(j))) {
						resolveCollision(section->objects.at(i), section->objects.at(j), deltaTime);
					}
				}
			}
		}
	}

	bool checkCollision(Model* first, Model* second) {
		if (first->boundingBox != nullptr) {
			if (second->boundingBox != nullptr) {
				return checkBoundingBoxCollision(*first->boundingBox, *second->boundingBox, *first->getTransform(), *second->getTransform());
			}
			else if (second->capsuleCollider != nullptr) {
				return checkBoxCapsuleCollision(*first->boundingBox, *second->capsuleCollider, *second->getTransform(), *first->getTransform());
			}
		}
		else if (first->capsuleCollider != nullptr) {
			if (second->boundingBox != nullptr) {
				return checkBoxCapsuleCollision(*second->boundingBox, *first->capsuleCollider, *first->getTransform(), *second->getTransform());
			}
			else if (second->capsuleCollider != nullptr) {
				return checkCapsuleCollision(*first->capsuleCollider, *second->capsuleCollider, *first->getTransform(), *second->getTransform());
			}
		}

		return false;
	}

	bool checkCollision(GameObject* first, GameObject* second) {
		glm::mat4 M = first->getTransform()->getMatrix();
		glm::mat4 M2 = second->getTransform()->getMatrix();
		if (first->boundingBox != nullptr) {
			if (second->boundingBox != nullptr) {
				return checkBoundingBoxCollision(*first->boundingBox, *second->boundingBox, M, M2);
			}
			else if (second->capsuleCollider != nullptr) {
				return checkBoxCapsuleCollision(*first->boundingBox, *second->capsuleCollider, M2, M);
			}
		}
		else if (first->capsuleCollider != nullptr) {
			if (second->boundingBox != nullptr) {
				return checkBoxCapsuleCollision(*second->boundingBox, *first->capsuleCollider, M, M2);
			}
			else if (second->capsuleCollider != nullptr) {
				return checkCapsuleCollision(*first->capsuleCollider, *second->capsuleCollider, M, M2);
			}
		}
		return false;
	}

	bool checkBoundingBoxCollision(const BoundingBox& box1, const BoundingBox& box2, const glm::mat4& transform1, const glm::mat4& transform2) {
		glm::vec3 vertices1[] = {
			glm::vec3(transform1 * glm::vec4(box1.vertices.at(0), 1.0f)),
			glm::vec3(transform1 * glm::vec4(box1.vertices.at(1), 1.0f)),
			glm::vec3(transform1 * glm::vec4(box1.vertices.at(2), 1.0f)),
			glm::vec3(transform1 * glm::vec4(box1.vertices.at(3), 1.0f)),
			glm::vec3(transform1 * glm::vec4(box1.vertices.at(4), 1.0f)),
			glm::vec3(transform1 * glm::vec4(box1.vertices.at(5), 1.0f)),
			glm::vec3(transform1 * glm::vec4(box1.vertices.at(6), 1.0f)),
			glm::vec3(transform1 * glm::vec4(box1.vertices.at(7), 1.0f))
		};

		glm::vec3 vertices2[] = {
			glm::vec3(transform2 * glm::vec4(box2.vertices.at(0), 1.0f)),
			glm::vec3(transform2 * glm::vec4(box2.vertices.at(1), 1.0f)),
			glm::vec3(transform2 * glm::vec4(box2.vertices.at(2), 1.0f)),
			glm::vec3(transform2 * glm::vec4(box2.vertices.at(3), 1.0f)),
			glm::vec3(transform2 * glm::vec4(box2.vertices.at(4), 1.0f)),
			glm::vec3(transform2 * glm::vec4(box2.vertices.at(5), 1.0f)),
			glm::vec3(transform2 * glm::vec4(box2.vertices.at(6), 1.0f)),
			glm::vec3(transform2 * glm::vec4(box2.vertices.at(7), 1.0f))
		};
		for (int i = 0; i < 8; i++) {
			if (vertices1[i].x >= vertices2[0].x && vertices1[i].x <= vertices2[1].x &&
				vertices1[i].y >= vertices2[0].y && vertices1[i].y <= vertices2[1].y &&
				vertices1[i].z >= vertices2[0].z && vertices1[i].z <= vertices2[1].z) {
				return true;
			}
		}
		for (int i = 0; i < 8; i++) {
			if (vertices2[i].x >= vertices1[0].x && vertices2[i].x <= vertices1[1].x &&
				vertices2[i].y >= vertices1[0].y && vertices2[i].y <= vertices1[1].y &&
				vertices2[i].z >= vertices1[0].z && vertices2[i].z <= vertices1[1].z) {
				return true;
			}
		}
		return false;
	}

	bool checkBoxCapsuleCollision(const BoundingBox& meshBox, const CapsuleCollider& otherCapsule, glm::mat4 transform, glm::mat4 boxtransform) {
		glm::vec3 boxMin = glm::vec3(boxtransform * glm::vec4(meshBox.min, 1.0f));
		glm::vec3 boxMax = glm::vec3(boxtransform * glm::vec4(meshBox.max, 1.0f));
		glm::vec3 capsuleCenter = otherCapsule.center;
		float capsuleRadius = otherCapsule.radius;
		float capsuleHeight = otherCapsule.height;

		glm::vec3 transformedCapsuleCenter = glm::vec3(transform * glm::vec4(capsuleCenter, 1.0f));
		bool insideBox = transformedCapsuleCenter.x >= boxMin.x && transformedCapsuleCenter.x <= boxMax.x &&
			transformedCapsuleCenter.y >= boxMin.y && transformedCapsuleCenter.y <= boxMax.y &&
			transformedCapsuleCenter.z >= boxMin.z && transformedCapsuleCenter.z <= boxMax.z;
		float distanceToBox = 0.0f;
		if (!insideBox) {
			if (transformedCapsuleCenter.x < boxMin.x)
				distanceToBox += (transformedCapsuleCenter.x - boxMin.x) * (transformedCapsuleCenter.x - boxMin.x);
			else if (transformedCapsuleCenter.x > boxMax.x)
				distanceToBox += (transformedCapsuleCenter.x - boxMax.x) * (transformedCapsuleCenter.x - boxMax.x);

			if (transformedCapsuleCenter.y < boxMin.y)
				distanceToBox += (transformedCapsuleCenter.y - boxMin.y) * (transformedCapsuleCenter.y - boxMin.y);
			else if (transformedCapsuleCenter.y > boxMax.y)
				distanceToBox += (transformedCapsuleCenter.y - boxMax.y) * (transformedCapsuleCenter.y - boxMax.y);

			if (transformedCapsuleCenter.z < boxMin.z)
				distanceToBox += (transformedCapsuleCenter.z - boxMin.z) * (transformedCapsuleCenter.z - boxMin.z);
			else if (transformedCapsuleCenter.z > boxMax.z)
				distanceToBox += (transformedCapsuleCenter.z - boxMax.z) * (transformedCapsuleCenter.z - boxMax.z);
		}
		float minDistance = capsuleRadius;
		return insideBox || distanceToBox <= minDistance * minDistance;
	}

	bool checkCapsuleCollision(const CapsuleCollider& capsule1, const CapsuleCollider& capsule2, glm::mat4 transform1, glm::mat4 transform2) {
		glm::vec3 center1 = capsule1.center;
		glm::vec3 center2 = capsule2.center;
		float radius1 = capsule1.radius;
		float radius2 = capsule2.radius;
		float height1 = capsule1.height;
		float height2 = capsule2.height;
		glm::vec3 transformedCenter1 = glm::vec3(transform1 * glm::vec4(center1, 1.0f));
		glm::vec3 transformedCenter2 = glm::vec3(transform2 * glm::vec4(center2, 1.0f));

		float distanceSquared = glm::length(transformedCenter1 - transformedCenter2);
		distanceSquared *= distanceSquared;
		float sumRadius = radius1 + radius2;
		if (distanceSquared > sumRadius * sumRadius) {
			return false;
		}
		float distanceY = std::abs(transformedCenter1.y - transformedCenter2.y) - (height1 + height2) / 2.0f;
		return distanceY <= sumRadius;
	}

	/*void resolveCollision(GameObject* first, GameObject* second, float deltaTime) {
		std::cout << "KOLIZJA" << std::endl;
		glm::vec3 displacement = calculateCollisionResponse(first->getModelComponent(), second->getModelComponent());
		glm::vec3 otherDisplacement = -displacement;
		if (first->getModelComponent()->boundingBox != nullptr) {
			displacement *= 0.1f * deltaTime;
		}
		else {
			displacement *= deltaTime;
		}
		if (second->getModelComponent()->boundingBox != nullptr) {
			otherDisplacement *= 0.1f * deltaTime;
		}
		else {
			otherDisplacement *= deltaTime;
		}
		if (!(glm::any(glm::isnan(displacement)) || glm::any(glm::isinf(displacement)))) {
			first->localTransform->localPosition += displacement;
			second->localTransform->localPosition += otherDisplacement;
			first->modelComponent->setTransform(glm::translate(*first->getModelComponent()->getTransform(), displacement));
			second->modelComponent->setTransform(glm::translate(*second->getModelComponent()->getTransform(), otherDisplacement));
		}
	}*/

	void resolveCollisionStatic(GameObject* first, GameObject* second, float deltaTime) {
		glm::vec3 displacement = calculateCollisionResponse(first, second);
		glm::vec3 otherDisplacement = -displacement;
		//float scalar = glm::length(glm::normalize(displacement));
		displacement *= 15.f * deltaTime;
		if (second->name.starts_with("branch") || second->name.starts_with("log")) {
			displacement *= 0.05f;
		}
		if (first->boundingBox != nullptr) {
			displacement *= 0.1f;
		}
		if (second->boundingBox != nullptr) {
			otherDisplacement *= 0.1f;
		}
		if (!(glm::any(glm::isnan(displacement)) || glm::any(glm::isinf(displacement)))) {
			first->localTransform->localPosition += displacement;
		}
	}

	void resolveCollision(GameObject* first, GameObject* second, float deltaTime) {
		glm::vec3 displacement = calculateCollisionResponse(first, second);
		glm::vec3 otherDisplacement = -displacement;
		//float scalar = glm::length(glm::normalize(displacement));
		displacement *= 2.f * deltaTime;
		otherDisplacement *= 2.f * deltaTime;
		if (first->boundingBox != nullptr) {
			displacement *= 0.1f;
		}
		if (second->boundingBox != nullptr) {
			otherDisplacement *= 0.1f;
		}
		if (!(glm::any(glm::isnan(displacement)) || glm::any(glm::isinf(displacement)))) {
			first->localTransform->localPosition += displacement;
			second->localTransform->localPosition += otherDisplacement;
		}
		else if (first->name.starts_with("enemy") && second->name.starts_with("enemy")) {
			Enemy* enemy1 = static_cast<Enemy*>(first);
			first->localTransform->localPosition += glm::vec3(enemy1->velocity.z ,0.0f,-enemy1->velocity.x) * deltaTime * 0.1f;
			addObject(second);
		}
	}

	glm::vec3 calculateCollisionResponse(GameObject* first, GameObject* second) {
		glm::vec3 displacement(0.0f);
		glm::mat4 M = first->getTransform()->getMatrix();
		glm::mat4 M2 = second->getTransform()->getMatrix();
		if (first->boundingBox != nullptr) {
			if (second->boundingBox != nullptr) {
				glm::vec3 direction = (glm::vec3(M * glm::vec4(first->boundingBox->center(), 1.0f)) - glm::vec3(M2 * glm::vec4(second->boundingBox->center(), 1.0f)));
				if (second->name.starts_with("branch")) {
					glm::vec2 branchlogdist = glm::vec2(second->boundingBox->center().x - second->parent->boundingBox->center().x, second->boundingBox->center().z - second->parent->boundingBox->center().z);
					float costoX = glm::cos(glm::dot(glm::normalize(branchlogdist), glm::vec2(1.0f, 0.0f)));
					float sintoX = glm::sin(glm::dot(glm::normalize(branchlogdist), glm::vec2(1.0f, 0.0f)));
					glm::mat4 tmpM = glm::translate(M, glm::vec3(direction.x * costoX, 0.0f, 0.0f));
					tmpM = glm::translate(M, glm::vec3(0.0f, 0.0f, direction.z * costoX));
					glm::vec3 tmpCenter = glm::vec3(tmpM * glm::vec4(first->boundingBox->center(), 1.0f));
					direction = tmpCenter - glm::vec3(M2 * glm::vec4(second->boundingBox->center(), 1.0f));
					float magnitude = (first->boundingBox->radius() + second->boundingBox->radius()) - glm::distance(tmpCenter, glm::vec3(M2 * glm::vec4(second->boundingBox->center(), 1.0f)));
					displacement += direction * magnitude;
				}
				else if (second->name.starts_with("log")) {
					float yDiff = second->boundingBox->center().y - first->boundingBox->center().y;
					glm::mat4 tmpM = glm::translate(M, glm::vec3(0.0f, yDiff, 0.0f));
					glm::vec3 tmpCenter = glm::vec3(tmpM * glm::vec4(first->boundingBox->center(), 1.0f));
					direction = tmpCenter - glm::vec3(M2 * glm::vec4(second->boundingBox->center(), 1.0f));
					float magnitude = (first->boundingBox->radius() + second->boundingBox->radius()) - glm::distance(tmpCenter, glm::vec3(M2 * glm::vec4(second->boundingBox->center(), 1.0f)));
					displacement += direction * magnitude;
				}
				else {
					direction = glm::normalize(direction);
					float magnitude = (first->boundingBox->radius() + second->boundingBox->radius()) - glm::distance(glm::vec3(M * glm::vec4(first->boundingBox->center(), 1.0f)), glm::vec3(M2 * glm::vec4(second->boundingBox->center(), 1.0f)));
					displacement += direction * magnitude;
				}
			}
			else if (second->capsuleCollider != nullptr) {
				glm::vec3 direction = glm::normalize(glm::vec3(M * glm::vec4(first->boundingBox->center(), 1.0f)) - second->capsuleCollider->center);
				float magnitude = (first->boundingBox->radius() + second->capsuleCollider->radius) - glm::distance(glm::vec3(M * glm::vec4(first->boundingBox->center(), 1.0f)), second->capsuleCollider->center);
				displacement += direction * magnitude;
			}
		}
		else if (first->capsuleCollider != nullptr) {
			if (second->boundingBox != nullptr) {
				glm::vec3 direction = glm::normalize(first->capsuleCollider->center - glm::vec3(M2 * glm::vec4(second->boundingBox->center(), 1.0f)));
				if (second->name.starts_with("branch")) {
					glm::vec2 branchlogdist = glm::vec2(second->boundingBox->center().x - second->parent->boundingBox->center().x, second->boundingBox->center().z - second->parent->boundingBox->center().z);
					float costoX = glm::cos(glm::dot(glm::normalize(branchlogdist), glm::vec2(1.0f, 0.0f)));
					float sintoX = glm::sin(glm::dot(glm::normalize(branchlogdist), glm::vec2(1.0f, 0.0f)));
					glm::mat4 tmpM = glm::translate(M, glm::vec3(direction.x * costoX, 0.0f, 0.0f));
					tmpM = glm::translate(M, glm::vec3(0.0f, 0.0f, direction.z * costoX));
					glm::vec3 tmpCenter = glm::vec3(tmpM * glm::vec4(first->capsuleCollider->center, 1.0f));
					direction = tmpCenter - glm::vec3(M2 * glm::vec4(second->boundingBox->center(), 1.0f));
					float magnitude = (first->capsuleCollider->radius + second->boundingBox->radius()) - glm::distance(tmpCenter, glm::vec3(M2 * glm::vec4(second->boundingBox->center(), 1.0f)));
					displacement += direction * magnitude;
				}
				else if (second->name.starts_with("log")) {
					float yDiff = second->boundingBox->center().y - first->capsuleCollider->center.y;
					glm::mat4 tmpM = glm::translate(M, glm::vec3(0.0f, yDiff, 0.0f));
					glm::vec3 tmpCenter = glm::vec3(tmpM * glm::vec4(first->capsuleCollider->center, 1.0f));
					direction = tmpCenter - glm::vec3(M2 * glm::vec4(second->boundingBox->center(), 1.0f));
					float magnitude = (first->capsuleCollider->radius + second->boundingBox->radius()) - glm::distance(tmpCenter, glm::vec3(M2 * glm::vec4(second->boundingBox->center(), 1.0f)));
					displacement += direction * magnitude;
				}
				else {
					direction = glm::normalize(direction);
					float magnitude = (first->capsuleCollider->radius + second->boundingBox->radius()) - glm::distance(first->capsuleCollider->center, glm::vec3(M2 * glm::vec4(second->boundingBox->center(), 1.0f)));
					displacement += direction * magnitude;
				}
			}
			else if (second->capsuleCollider != nullptr) {
				glm::vec3 direction = glm::normalize(first->capsuleCollider->center - second->capsuleCollider->center);
				float magnitude = (first->capsuleCollider->radius + second->capsuleCollider->radius) - glm::distance(first->capsuleCollider->center, second->capsuleCollider->center);
				displacement += direction * magnitude;
			}
		}
		std::cout << first->name << ", " << second->name << glm::to_string(displacement) << std::endl;
		return displacement;
	}

	glm::vec3 calculateCollisionResponse(Model* first, Model* other) {
		glm::vec3 displacement(0.0f);
		if (first->boundingBox != nullptr) {
			if (other->boundingBox != nullptr) {
				glm::vec3 direction = glm::normalize(glm::vec3(*first->getTransform() * glm::vec4(first->boundingBox->center(), 1.0f)) - glm::vec3(*other->getTransform() * glm::vec4(other->boundingBox->center(), 1.0f)));
				float magnitude = (first->boundingBox->radius() + other->boundingBox->radius()) - glm::distance(glm::vec3(*first->getTransform() * glm::vec4(first->boundingBox->center(), 1.0f)), glm::vec3(*other->getTransform() * glm::vec4(other->boundingBox->center(), 1.0f)));
				displacement += direction * magnitude;
				std::cout << "2box" << glm::to_string(displacement) << std::endl;
			}
			else if (other->capsuleCollider != nullptr) {
				glm::vec3 direction = glm::normalize(glm::vec3(*first->getTransform() * glm::vec4(first->boundingBox->center(), 1.0f)) - other->capsuleCollider->center);
				float magnitude = (first->boundingBox->radius() + other->capsuleCollider->radius) - glm::distance(glm::vec3(*first->getTransform() * glm::vec4(first->boundingBox->center(), 1.0f)), other->capsuleCollider->center);
				displacement += direction * magnitude;
				std::cout << "boxcapsule" << glm::to_string(displacement) << std::endl;
			}
		}
		else if (first->capsuleCollider != nullptr) {
			if (other->boundingBox != nullptr) {
				glm::vec3 direction = glm::normalize(first->capsuleCollider->center - glm::vec3(*other->getTransform() * glm::vec4(other->boundingBox->center(), 1.0f)));
				float magnitude = (first->capsuleCollider->radius + other->boundingBox->radius()) - glm::distance(first->capsuleCollider->center, glm::vec3(*other->getTransform() * glm::vec4(other->boundingBox->center(), 1.0f)));
				displacement += direction * magnitude;
				std::cout << "capsulebox" << glm::to_string(displacement) << std::endl;
			}
			else if (other->capsuleCollider != nullptr) {
				glm::vec3 direction = glm::normalize(first->capsuleCollider->center - other->capsuleCollider->center);
				float magnitude = (first->capsuleCollider->radius + other->capsuleCollider->radius) - glm::distance(first->capsuleCollider->center, other->capsuleCollider->center);
				displacement += direction * magnitude;
				std::cout << "2capsule" << glm::to_string(displacement) << std::endl;
			}
		}
		return displacement;
	}
};

#endif
