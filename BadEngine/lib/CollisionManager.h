#ifndef CollisionManager_H
#define CollisionManager_H

#include <vector>
#include "Section.h"
#include <cmath>
#include "Enemy.h"
#include "PBD.h"

class CollisionManager {
public:
	std::vector<Section*> sections;
	float sectionSize;

	CollisionManager(float worldSize, float sectionSize) : sectionSize(sectionSize) {
		int numSections = worldSize / sectionSize;
		int IDcounter = 0;
		for (int i = -numSections*0.5f; i < numSections*0.5f; i++) {
			for (int j = -numSections * 0.5f; j < numSections * 0.5f; j++) { //TODO: ograniczy� wysoko�� do realnych potrzeb
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

	void addObject(GameObject* go) { //TODO: dla obiekt�w, kt�re zostan� dodane do sektora, sprawdzi� tylko przylegaj�ce sektory
		ZoneScopedN("AddObject");
		for (auto section : sections) {
			if (go->name != "player") {
				auto it = std::find(section->objects.begin(), section->objects.end(), go);
				if (it != section->objects.end()) {
					section->objects.erase(it);
				}
			}
			if (section->checkCollision(go)) {
				section->objects.push_back(go);
			}
		}
	}

	void addStaticObject(GameObject* go) {
		ZoneScopedN("AddStaticObject");
		for (auto section : sections) {
			if (section->checkCollision(go)) {
				section->staticObjects.push_back(go);
			}
		}
	}

	void addObjectPredict(GameObject* go) {
		ZoneScopedN("AddObjectPredict");
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
		ZoneScopedN("CheckPrediction");
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

	bool checkCollision(GameObject* first, GameObject* second) {
		glm::mat4 M = first->getTransform()->getMatrix();
		glm::mat4 M2 = second->getTransform()->getMatrix();
		if (first->boundingBox != nullptr) {
			if (second->boundingBox != nullptr) {
				return checkBoundingBoxCollision(*first->boundingBox, *second->boundingBox, M, M2);
			}
			else if (second->capsuleCollider != nullptr) {
				return checkBoxCapsuleCollision(first, second, M2, M);
			}
		}
		else if (first->capsuleCollider != nullptr) {
			if (second->boundingBox != nullptr) {
				return checkBoxCapsuleCollision(second, first, M, M2);
			}
			else if (second->capsuleCollider != nullptr) {
				return checkCapsuleCollision(first, second, M, M2);
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

	bool checkBoxCapsuleCollision(const GameObject* box, const GameObject* capsule, const glm::mat4& boxTransform, const glm::mat4& capsuleTransform) {
		glm::vec3 capsuleCenter = capsule->capsuleCollider->center;
		float capsuleRadius = capsule->capsuleCollider->radius * capsule->localTransform->localScale.x;
		float capsuleHeight = capsule->capsuleCollider->height * capsule->localTransform->localScale.y;

		glm::vec3 top = capsuleCenter + glm::vec3(0.0f, capsuleHeight * 0.5f, 0.0f);
		glm::vec3 bottom = capsuleCenter - glm::vec3(0.0f, capsuleHeight * 0.5f, 0.0f);

		glm::vec3 worldTop = glm::vec3(capsuleTransform * glm::vec4(top, 1.0f));
		glm::vec3 worldBottom = glm::vec3(capsuleTransform * glm::vec4(bottom, 1.0f));

		std::vector<glm::vec3> boxVertices;
		for (const auto& vertex : box->boundingBox->vertices) {
			boxVertices.push_back(glm::vec3(boxTransform * glm::vec4(vertex, 1.0f)));
		}

		for (const auto& vertex : boxVertices) {
			glm::vec3 closestPointOnCapsule = ClosestPointOnLineSegment(worldTop, worldBottom, vertex);
			float distanceSquared = glm::length(vertex - closestPointOnCapsule);
			distanceSquared *= distanceSquared;
			if (distanceSquared <= capsuleRadius * capsuleRadius) {
				return true;
			}
		}

		glm::vec3 boxMin = glm::vec3(boxTransform * glm::vec4(box->boundingBox->min, 1.0f));
		glm::vec3 boxMax = glm::vec3(boxTransform * glm::vec4(box->boundingBox->max, 1.0f));

		glm::vec3 closestPointOnBoxToTop = glm::clamp(worldTop, boxMin, boxMax);
		glm::vec3 closestPointOnBoxToBottom = glm::clamp(worldBottom, boxMin, boxMax);

		if (glm::length(worldTop - closestPointOnBoxToTop) * glm::length(worldTop - closestPointOnBoxToTop)  <= capsuleRadius * capsuleRadius ||
			glm::length(worldBottom - closestPointOnBoxToBottom) * glm::length(worldBottom - closestPointOnBoxToBottom) <= capsuleRadius * capsuleRadius) {
			return true;
		}

		return false;
	}

	bool checkCapsuleCollision(const GameObject* capsule1, const GameObject* capsule2, glm::mat4 transform1, glm::mat4 transform2) {
		glm::vec3 center1 = capsule1->capsuleCollider->center;
		glm::vec3 center2 = capsule2->capsuleCollider->center;
		float radius1 = capsule1->capsuleCollider->radius;
		float radius2 = capsule2->capsuleCollider->radius;
		float height1 = capsule1->capsuleCollider->height * capsule1->localTransform->localScale.y;
		float height2 = capsule2->capsuleCollider->height * capsule2->localTransform->localScale.y;
		glm::vec3 top1 = center1 + glm::vec3(0.0f, height1 * 0.5f, 0.0f);
		glm::vec3 bottom1 = center1 - glm::vec3(0.0f, height1 * 0.5f, 0.0f);
		glm::vec3 top2 = center2 + glm::vec3(0.0f, height2 * 0.5f, 0.0f);
		glm::vec3 bottom2 = center2 - glm::vec3(0.0f, height2 * 0.5f, 0.0f);
		glm::vec3 Normal1 = glm::normalize(glm::vec3(transform1 * glm::vec4(top1, 1.0f)) - glm::vec3(transform1 * glm::vec4(bottom1,1.0f)));
		glm::vec3 LineEndOffset1 = Normal1 * (radius1 * capsule1->localTransform->localScale.x);
		glm::vec A1 = glm::vec3(transform1 * glm::vec4(bottom1, 1.0f)) + LineEndOffset1;
		glm::vec B1 = glm::vec3(transform1 * glm::vec4(top1, 1.0f)) - LineEndOffset1;
		glm::vec3 Normal2 = glm::normalize(glm::vec3(transform2 * glm::vec4(top2, 1.0f)) - glm::vec3(transform2 * glm::vec4(bottom2, 1.0f)));
		glm::vec3 LineEndOffset2 = Normal2 * (radius2 * capsule2->localTransform->localScale.x);
		glm::vec3 A2 = glm::vec3(transform2 * glm::vec4(bottom2, 1.0f)) + LineEndOffset2;
		glm::vec3 B2 = glm::vec3(transform2 * glm::vec4(top2, 1.0f)) - LineEndOffset2;
		glm::vec3 v0 = A2 - A1;
		glm::vec3 v1 = B2 - A1;
		glm::vec3 v2 = A2 - B1;
		glm::vec3 v3 = B2 - B1;
		float d0 = glm::dot(v0, v0);
		float d1 = glm::dot(v1, v1);
		float d2 = glm::dot(v2, v2);
		float d3 = glm::dot(v3, v3);
		glm::vec3 bestA;
		if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1) {
			bestA = B1;
		}
		else {
			bestA = A1;
		}
		glm::vec3 bestB = ClosestPointOnLineSegment(A2, B2, bestA);
		bestA = ClosestPointOnLineSegment(A1, B1, bestB);
		glm::vec3 penetration = bestA - bestB;
		float len = glm::length(penetration);
		penetration = glm::normalize(penetration);
		float penetrationDepth = radius1 + radius2 - len;
		return penetrationDepth > 0.0f;
	}

	glm::vec3 ClosestPointOnLineSegment(glm::vec3 A, glm::vec3 B, glm::vec3 Point)
	{
		glm::vec3 AB = B - A;
		float t = dot(Point - A, AB) / dot(AB, AB);
		return A + glm::min(glm::max(t, 0.0f), 1.0f) * AB;
	}

	void resolveCollisionStatic(GameObject* first, GameObject* second, float deltaTime) {
		if (first->name == "player" && !(/*second->name.starts_with("tree") ||*/ second->name.starts_with("branch"))) {
			first->localTransform->predictedPosition = first->localTransform->localPosition;
		}
		else if (first->name == "player" && first->velocity.y > 0 && second->name.starts_with("tree")) {
			first->localTransform->predictedPosition = first->localTransform->localPosition;
		}
		else if (second->name.starts_with("branch")) {
			first->localTransform->predictedPosition.y = first->localTransform->localPosition.y;
		}
		else {
			glm::vec3 displacement = calculateCollisionResponseStatic(first, second);
			glm::vec3 otherDisplacement = -displacement;
			//float scalar = glm::length(glm::normalize(displacement));
			displacement *= deltaTime;
			if (second->name.starts_with("branch") || second->name.starts_with("log")) {
				displacement *= 0.1f;
			}
			if (second->name.starts_with("tree") || second->name.starts_with("log")) {
				displacement.y = 0.0f;
			}
			if (first->boundingBox != nullptr) {
				displacement *= 0.1f;
			}
			if (second->boundingBox != nullptr) {
				otherDisplacement *= 0.1f;
			}
			if (!(glm::any(glm::isnan(displacement)) || glm::any(glm::isinf(displacement)))) {
				first->localTransform->predictedPosition += displacement;
			}
		}
	}

	void resolveCollision(GameObject* first, GameObject* second, float deltaTime) {
		glm::vec3 displacement = calculateCollisionResponse(first, second);
		displacement.y = 0.0f;
		glm::vec3 otherDisplacement = -displacement;
		//float scalar = glm::length(glm::normalize(displacement));
		displacement *= deltaTime;
		otherDisplacement *= deltaTime;
		if (first->boundingBox != nullptr) {
			displacement *= 0.1f;
		}
		if (second->boundingBox != nullptr) {
			otherDisplacement *= 0.1f;
		}
		if (!(glm::any(glm::isnan(displacement)) || glm::any(glm::isinf(displacement)))) {
			first->localTransform->predictedPosition += displacement;
			second->localTransform->predictedPosition += otherDisplacement;
		}
		else {
			first->localTransform->predictedPosition = first->localTransform->localPosition;
			second->localTransform->predictedPosition = second->localTransform->localPosition;
		}
	}

	glm::vec3 calculateCollisionResponseStatic(GameObject* first, GameObject* second) {
		glm::vec3 displacement(0.0f);
		glm::mat4 M = first->getTransform()->getPredictedMatrix();
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

	glm::vec3 calculateCollisionResponse(GameObject* first, GameObject* second) {
		glm::vec3 displacement(0.0f);
		glm::mat4 M = first->getTransform()->getPredictedMatrix();
		glm::mat4 M2 = second->getTransform()->getPredictedMatrix();
		if (first->boundingBox != nullptr) {
			if (second->boundingBox != nullptr) {
				glm::vec3 direction = (glm::vec3(M * glm::vec4(first->boundingBox->center(), 1.0f)) - glm::vec3(M2 * glm::vec4(second->boundingBox->center(), 1.0f)));
				direction = glm::normalize(direction);
				float magnitude = (first->boundingBox->radius() + second->boundingBox->radius()) - glm::distance(glm::vec3(M * glm::vec4(first->boundingBox->center(), 1.0f)), glm::vec3(M2 * glm::vec4(second->boundingBox->center(), 1.0f)));
				displacement += direction * magnitude;
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
				direction = glm::normalize(direction);
				float magnitude = (first->capsuleCollider->radius + second->boundingBox->radius()) - glm::distance(first->capsuleCollider->center, glm::vec3(M2 * glm::vec4(second->boundingBox->center(), 1.0f)));
				displacement += direction * magnitude;
			}
			else if (second->capsuleCollider != nullptr) {
				glm::vec3 direction = glm::normalize(first->capsuleCollider->center - second->capsuleCollider->center);
				float magnitude = (first->capsuleCollider->radius + second->capsuleCollider->radius) - glm::distance(first->capsuleCollider->center, second->capsuleCollider->center);
				displacement += direction * magnitude;
			}
		}
		//std::cout << first->name << ", " << second->name << glm::to_string(displacement) << std::endl;
		return displacement;
	}

	void simulate(float deltaTime) {
		ZoneScopedN("simulate");
		for (auto section : sections) {
			for (int i = 0; i < section->objects.size(); i++) {
				for (int j = 0; j < section->staticObjects.size(); j++) {
					if (checkCollisionPBDStatic(section->objects.at(i), section->staticObjects.at(j))) {
						//separateStatic(section->objects.at(i), section->staticObjects.at(j), deltaTime);
						resolveCollisionStatic(section->objects.at(i), section->staticObjects.at(j), deltaTime);
					}
				}
				for (int j = i + 1; j < section->objects.size(); j++) {
					if (checkCollisionPBD(section->objects.at(i), section->objects.at(j))) {
						separate(section->objects.at(i), section->objects.at(j));
						resolveCollision(section->objects.at(i), section->objects.at(j), deltaTime);
						if (glm::distance(section->objects.at(i)->getTransform()->predictedPosition, section->objects.at(j)->getTransform()->predictedPosition) 
							< section->objects.at(j)->capsuleCollider->radius * section->objects.at(j)->getTransform()->localScale.x * 2.0f) {
							auto object = section->objects.at(i);
							object->getTransform()->predictedPosition += glm::vec3(object->velocity.z,object->velocity.y, -object->velocity.x) * deltaTime * 5.f;
						}
						
					}
				}
			}
		}
	}

	bool checkCollisionPBD(GameObject* first, GameObject* second) {
		if (first->boundingBox) {
			if (second->boundingBox) {
				return checkBoundingBoxCollision(*first->boundingBox, *second->boundingBox, first->getTransform()->getPredictedMatrix(), second->getTransform()->getPredictedMatrix());
			}
			else if (second->capsuleCollider) {
				return checkBoxCapsuleCollision(first, second, first->getTransform()->getPredictedMatrix(), second->getTransform()->getPredictedMatrix());
			}
		}
		else if (first->capsuleCollider) {
			if (second->boundingBox) {
				return checkBoxCapsuleCollision(second, first, second->getTransform()->getPredictedMatrix(), first->getTransform()->getPredictedMatrix());
			}
			else if (second->capsuleCollider) {
				return checkCapsuleCollision(first, second, first->getTransform()->getPredictedMatrix(), second->getTransform()->getPredictedMatrix());
			}
		}
		return false;
	}

	bool checkCollisionPBDStatic(GameObject* first, GameObject* second) {
		if (first->boundingBox) {
			if (second->boundingBox) {
				return checkBoundingBoxCollision(*first->boundingBox, *second->boundingBox, first->getTransform()->getPredictedMatrix(), second->getTransform()->getMatrix());
			}
			else if (second->capsuleCollider) {
				return checkBoxCapsuleCollision(first, second, first->getTransform()->getPredictedMatrix(), second->getTransform()->getMatrix());
			}
		}
		else if (first->capsuleCollider) {
			if (second->boundingBox) {
				return checkBoxCapsuleCollision(second, first, second->getTransform()->getMatrix(), first->getTransform()->getPredictedMatrix());
			}
			else if (second->capsuleCollider) {
				return checkCapsuleCollision(first, second, first->getTransform()->getPredictedMatrix(), second->getTransform()->getMatrix());
			}
		}
		return false;
	}

	bool checkBoundingBoxPBDCollision(const BoundingBox* box, const BoundingBox* other, const glm::mat4& transform, const glm::mat4& otherTransform) {
		glm::vec3 vertices1[] = {
			glm::vec3(transform * glm::vec4(box->particles.at(0)->predictedPosition, 1.0f)),
			glm::vec3(transform * glm::vec4(box->particles.at(1)->predictedPosition, 1.0f)),
			glm::vec3(transform * glm::vec4(box->particles.at(2)->predictedPosition, 1.0f)),
			glm::vec3(transform * glm::vec4(box->particles.at(3)->predictedPosition, 1.0f)),
			glm::vec3(transform * glm::vec4(box->particles.at(4)->predictedPosition, 1.0f)),
			glm::vec3(transform * glm::vec4(box->particles.at(5)->predictedPosition, 1.0f)),
			glm::vec3(transform * glm::vec4(box->particles.at(6)->predictedPosition, 1.0f)),
			glm::vec3(transform * glm::vec4(box->particles.at(7)->predictedPosition, 1.0f))
		};

		glm::vec3 vertices2[] = {
			glm::vec3(otherTransform * glm::vec4(other->particles.at(0)->predictedPosition, 1.0f)),
			glm::vec3(otherTransform * glm::vec4(other->particles.at(1)->predictedPosition, 1.0f)),
			glm::vec3(otherTransform * glm::vec4(other->particles.at(2)->predictedPosition, 1.0f)),
			glm::vec3(otherTransform * glm::vec4(other->particles.at(3)->predictedPosition, 1.0f)),
			glm::vec3(otherTransform * glm::vec4(other->particles.at(4)->predictedPosition, 1.0f)),
			glm::vec3(otherTransform * glm::vec4(other->particles.at(5)->predictedPosition, 1.0f)),
			glm::vec3(otherTransform * glm::vec4(other->particles.at(6)->predictedPosition, 1.0f)),
			glm::vec3(otherTransform * glm::vec4(other->particles.at(7)->predictedPosition, 1.0f))
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

	bool checkBoxCapsulePBDCollision(const BoundingBox* meshBox, const CapsuleCollider* otherCapsule, glm::mat4 transform, glm::mat4 boxtransform) {
		glm::vec3 boxMin = glm::vec3(boxtransform * glm::vec4(meshBox->particles.at(0)->predictedPosition, 1.0f));
		glm::vec3 boxMax = glm::vec3(boxtransform * glm::vec4(meshBox->particles.at(1)->predictedPosition, 1.0f));
		float capsuleHeight = glm::distance(glm::vec3(transform * glm::vec4(otherCapsule->top->predictedPosition, 1.0f)), glm::vec3(transform * glm::vec4(otherCapsule->bottom->predictedPosition, 1.0f)));
		glm::vec3 capsuleCenter = otherCapsule->bottom->predictedPosition + capsuleHeight * 0.5f;
		float capsuleRadius = otherCapsule->radius;

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

	bool checkCapsulePBDCollision(const CapsuleCollider* capsule1, const CapsuleCollider* capsule2, glm::mat4 transform1, glm::mat4 transform2) {
		float height1 = glm::distance(capsule1->top->predictedPosition, capsule1->bottom->predictedPosition);
		float height2 = glm::distance(capsule2->top->predictedPosition, capsule2->bottom->predictedPosition);
		glm::vec3 center1 = capsule1->bottom->predictedPosition + height1 * 0.5f;
		glm::vec3 center2 = capsule2->bottom->predictedPosition + height2 * 0.5f;
		float radius1 = capsule1->radius;
		float radius2 = capsule2->radius;
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

	void separate(GameObject* first, GameObject* second) {
		glm::vec3 normal = glm::vec3(0.0f);
		float penetration = 0.0f;
		if (first->boundingBox) {
			if (second->boundingBox) {
				normal = glm::normalize(glm::vec3(first->getTransform()->getPredictedMatrix() * glm::vec4(first->boundingBox->center(), 1.0f)) - glm::vec3(second->getTransform()->getPredictedMatrix() * glm::vec4(second->boundingBox->center(), 1.0f)));
				float distance = glm::distance(glm::vec3(first->getTransform()->getPredictedMatrix() * glm::vec4(first->boundingBox->center(), 1.0f)), glm::vec3(second->getTransform()->getPredictedMatrix() * glm::vec4(second->boundingBox->center(), 1.0f)));
				penetration = (first->boundingBox->radius() * first->getTransform()->localScale.x + second->boundingBox->radius() * second->getTransform()->localScale.x) - distance;
			}
			else if (second->capsuleCollider) {
				normal = glm::normalize(glm::vec3(first->getTransform()->getPredictedMatrix() * glm::vec4(first->boundingBox->center(), 1.0f)) - glm::vec3(second->getTransform()->getPredictedMatrix() * glm::vec4(second->capsuleCollider->center, 1.0f)));
				float distance = glm::distance(glm::vec3(first->getTransform()->getPredictedMatrix() * glm::vec4(first->boundingBox->center(), 1.0f)), glm::vec3(second->getTransform()->getPredictedMatrix() * glm::vec4(second->capsuleCollider->center, 1.0f)));
				penetration = (first->boundingBox->radius() * first->getTransform()->localScale.x + second->capsuleCollider->radius * second->getTransform()->localScale.x) - distance;
			}
		}
		else if (first->capsuleCollider) {
			if (second->boundingBox) {
				normal = glm::normalize(glm::vec3(first->getTransform()->getPredictedMatrix() * glm::vec4(first->capsuleCollider->center, 1.0f)) - glm::vec3(second->getTransform()->getPredictedMatrix() * glm::vec4(second->boundingBox->center(), 1.0f)));
				float distance = glm::distance(glm::vec3(first->getTransform()->getPredictedMatrix() * glm::vec4(first->capsuleCollider->center, 1.0f)), glm::vec3(second->getTransform()->getPredictedMatrix() * glm::vec4(second->boundingBox->center(), 1.0f)));
				penetration = (first->capsuleCollider->radius * first->getTransform()->localScale.x + second->boundingBox->radius() * second->getTransform()->localScale.x) - distance;
			}
			else if (second->capsuleCollider) {
				normal = glm::normalize(glm::vec3(first->getTransform()->getPredictedMatrix() * glm::vec4(first->capsuleCollider->center, 1.0f)) - glm::vec3(second->getTransform()->getPredictedMatrix() * glm::vec4(second->capsuleCollider->center, 1.0f)));
				float distance = glm::distance(glm::vec3(first->getTransform()->getPredictedMatrix() * glm::vec4(first->capsuleCollider->center, 1.0f)), glm::vec3(second->getTransform()->getPredictedMatrix() * glm::vec4(second->capsuleCollider->center, 1.0f)));
				penetration = (first->capsuleCollider->radius * first->getTransform()->localScale.x + second->capsuleCollider->radius * second->getTransform()->localScale.x) - distance;
			}
		}
		glm::vec3 separation = normal * penetration;
		separation.y = 0.0f;
		if (!(glm::any(glm::isnan(separation)) || glm::any(glm::isinf(separation)))) {
			first->localTransform->predictedPosition += separation;
			second->localTransform->predictedPosition -= separation;
		}
		//std::cout <<"separation: " << first->name << ", " << second->name << glm::to_string(separation) << std::endl;
	}

	void separateStatic(GameObject* first, GameObject* second, float deltaTime) {
		glm::vec3 normal = glm::vec3(0.0f);
		float penetration = 0.0f;
		if (first->boundingBox) {
			if (second->boundingBox) {
				normal = glm::normalize(glm::vec3(first->getTransform()->getPredictedMatrix() * glm::vec4(first->boundingBox->center(), 1.0f)) - glm::vec3(second->getTransform()->getMatrix() * glm::vec4(second->boundingBox->center(), 1.0f)));
				float distance = glm::distance(glm::vec3(first->getTransform()->getPredictedMatrix() * glm::vec4(first->boundingBox->center(), 1.0f)), glm::vec3(second->getTransform()->getMatrix() * glm::vec4(second->boundingBox->center(), 1.0f)));
				penetration = (first->boundingBox->radius() * first->getTransform()->localScale.x + second->boundingBox->radius() * second->getTransform()->localScale.x) - distance;
			}
			else if (second->capsuleCollider) {
				normal = glm::normalize(glm::vec3(first->getTransform()->getPredictedMatrix() * glm::vec4(first->boundingBox->center(), 1.0f)) - glm::vec3(second->getTransform()->getMatrix() * glm::vec4(second->capsuleCollider->center, 1.0f)));
				float distance = glm::distance(glm::vec3(first->getTransform()->getPredictedMatrix() * glm::vec4(first->boundingBox->center(), 1.0f)), glm::vec3(second->getTransform()->getMatrix() * glm::vec4(second->capsuleCollider->center, 1.0f)));
				penetration = (first->boundingBox->radius() * first->getTransform()->localScale.x + second->capsuleCollider->radius * second->getTransform()->localScale.x) - distance;
			}
		}
		else if (first->capsuleCollider) {
			if (second->boundingBox) {
				normal = glm::normalize(glm::vec3(first->getTransform()->getPredictedMatrix() * glm::vec4(first->capsuleCollider->center, 1.0f)) - glm::vec3(second->getTransform()->getMatrix() * glm::vec4(second->boundingBox->center(), 1.0f)));
				float distance = glm::distance(glm::vec3(first->getTransform()->getPredictedMatrix() * glm::vec4(first->capsuleCollider->center, 1.0f)), glm::vec3(second->getTransform()->getMatrix() * glm::vec4(second->boundingBox->center(), 1.0f)));
				penetration = (first->capsuleCollider->radius * first->getTransform()->localScale.x + second->boundingBox->radius() * second->getTransform()->localScale.x) - distance;
			}
			else if (second->capsuleCollider) {
				normal = glm::normalize(glm::vec3(first->getTransform()->getPredictedMatrix() * glm::vec4(first->capsuleCollider->center, 1.0f)) - glm::vec3(second->getTransform()->getMatrix() * glm::vec4(second->capsuleCollider->center, 1.0f)));
				float distance = glm::distance(glm::vec3(first->getTransform()->getPredictedMatrix() * glm::vec4(first->capsuleCollider->center, 1.0f)), glm::vec3(second->getTransform()->getMatrix() * glm::vec4(second->capsuleCollider->center, 1.0f)));
				penetration = (first->capsuleCollider->radius * first->getTransform()->localScale.x + second->capsuleCollider->radius * second->getTransform()->localScale.x) - distance;
			}
		}
		glm::vec3 separation = normal * penetration;
		if (second->name.starts_with("tree") || second->name.starts_with("log")) {
			separation.y = 0.0f;
		}
		first->localTransform->predictedPosition += separation * deltaTime;
		std::cout << first->name << ", " << second->name << glm::to_string(separation) << std::endl;
	}
};

#endif
