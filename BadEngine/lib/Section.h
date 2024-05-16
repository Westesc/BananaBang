#ifndef Section_H
#define Section_H

#include "BoundingBox.h"

class Section {
public:
	BoundingBox* bounds;
	std::vector<GameObject*> objects;
	std::vector<GameObject*> staticObjects;
	int ID;

	Section(float x, float y, float z, float size,int ID) : ID(ID) {
		bounds = new BoundingBox(glm::vec3(x, y, z), glm::vec3(x + size, y + size, z + size), true);
	}

	bool checkCollision(Model* object) {
		if (object->boundingBox != nullptr) {
			glm::vec3 vertices1[] = {
				glm::vec3(*object->getTransform() * glm::vec4(object->boundingBox->vertices.at(0), 1.0f)),
				glm::vec3(*object->getTransform() * glm::vec4(object->boundingBox->vertices.at(1), 1.0f)),
				glm::vec3(*object->getTransform() * glm::vec4(object->boundingBox->vertices.at(2), 1.0f)),
				glm::vec3(*object->getTransform() * glm::vec4(object->boundingBox->vertices.at(3), 1.0f)),
				glm::vec3(*object->getTransform() * glm::vec4(object->boundingBox->vertices.at(4), 1.0f)),
				glm::vec3(*object->getTransform() * glm::vec4(object->boundingBox->vertices.at(5), 1.0f)),
				glm::vec3(*object->getTransform() * glm::vec4(object->boundingBox->vertices.at(6), 1.0f)),
				glm::vec3(*object->getTransform() * glm::vec4(object->boundingBox->vertices.at(7), 1.0f))
			};
			for (int i = 0; i < 8; i++) {
				if (vertices1[i].x >= bounds->min.x && vertices1[i].x <= bounds->max.x &&
					vertices1[i].y >= bounds->min.y && vertices1[i].y <= bounds->max.y &&
					vertices1[i].z >= bounds->min.z && vertices1[i].z <= bounds->max.z) {
					return true;
				}
			}
			return false;
		}
		else if (object->capsuleCollider != nullptr) {
			glm::vec3 capsuleCenter = object->capsuleCollider->center;
			float capsuleRadius = object->capsuleCollider->radius;
			float capsuleHeight = object->capsuleCollider->height;
			glm::vec3 transformedCenter = glm::vec3(*object->getTransform() * glm::vec4(capsuleCenter, 1.0f));
			bool insideBox = transformedCenter.x >= bounds->min.x && transformedCenter.x <= bounds->max.x &&
				transformedCenter.y >= bounds->min.y && transformedCenter.y <= bounds->max.y &&
				transformedCenter.z >= bounds->min.z && transformedCenter.z <= bounds->max.z;
			float distanceToBox = 0.0f;
			if (!insideBox) {
				if (transformedCenter.x < bounds->min.x)
					distanceToBox += (transformedCenter.x - bounds->min.x) * (transformedCenter.x - bounds->min.x);
				else if (transformedCenter.x > bounds->max.x)
					distanceToBox += (transformedCenter.x - bounds->max.x) * (transformedCenter.x - bounds->max.x);

				if (transformedCenter.y < bounds->min.y)
					distanceToBox += (transformedCenter.y - bounds->min.y) * (transformedCenter.y - bounds->min.y);
				else if (transformedCenter.y > bounds->max.y)
					distanceToBox += (transformedCenter.y - bounds->max.y) * (transformedCenter.y - bounds->max.y);

				if (transformedCenter.z < bounds->min.z)
					distanceToBox += (transformedCenter.z - bounds->min.z) * (transformedCenter.z - bounds->min.z);
				else if (transformedCenter.z > bounds->max.z)
					distanceToBox += (transformedCenter.z - bounds->max.z) * (transformedCenter.z - bounds->max.z);
			}
			float minDistance = capsuleRadius;
			return insideBox || distanceToBox <= minDistance * minDistance;
		}
		return false;
	}
	bool checkCollision(GameObject* object) {
		if (object->boundingBox != nullptr) {

			glm::mat4 M = object->getTransform()->getMatrix();
			glm::vec3 vertices1[] = {
				glm::vec3(M * glm::vec4(object->boundingBox->vertices.at(0), 1.0f)),
				glm::vec3(M * glm::vec4(object->boundingBox->vertices.at(1), 1.0f)),
				glm::vec3(M * glm::vec4(object->boundingBox->vertices.at(2), 1.0f)),
				glm::vec3(M * glm::vec4(object->boundingBox->vertices.at(3), 1.0f)),
				glm::vec3(M * glm::vec4(object->boundingBox->vertices.at(4), 1.0f)),
				glm::vec3(M * glm::vec4(object->boundingBox->vertices.at(5), 1.0f)),
				glm::vec3(M * glm::vec4(object->boundingBox->vertices.at(6), 1.0f)),
				glm::vec3(M * glm::vec4(object->boundingBox->vertices.at(7), 1.0f))
			};
			for (int i = 0; i < 8; i++) {
				if (vertices1[i].x >= bounds->min.x && vertices1[i].x <= bounds->max.x &&
					vertices1[i].y >= bounds->min.y && vertices1[i].y <= bounds->max.y &&
					vertices1[i].z >= bounds->min.z && vertices1[i].z <= bounds->max.z) {
					return true;
				}
			}
			return false;
		}
		/*else if (object->capsuleCollider != nullptr) {
			glm::vec3 capsuleCenter = object->capsuleCollider->center;
			float capsuleRadius = object->capsuleCollider->radius;
			float capsuleHeight = object->capsuleCollider->height;
			glm::vec3 transformedCenter = glm::vec3(*object->getTransform() * glm::vec4(capsuleCenter, 1.0f));
			bool insideBox = transformedCenter.x >= bounds->min.x && transformedCenter.x <= bounds->max.x &&
				transformedCenter.y >= bounds->min.y && transformedCenter.y <= bounds->max.y &&
				transformedCenter.z >= bounds->min.z && transformedCenter.z <= bounds->max.z;
			float distanceToBox = 0.0f;
			if (!insideBox) {
				if (transformedCenter.x < bounds->min.x)
					distanceToBox += (transformedCenter.x - bounds->min.x) * (transformedCenter.x - bounds->min.x);
				else if (transformedCenter.x > bounds->max.x)
					distanceToBox += (transformedCenter.x - bounds->max.x) * (transformedCenter.x - bounds->max.x);

				if (transformedCenter.y < bounds->min.y)
					distanceToBox += (transformedCenter.y - bounds->min.y) * (transformedCenter.y - bounds->min.y);
				else if (transformedCenter.y > bounds->max.y)
					distanceToBox += (transformedCenter.y - bounds->max.y) * (transformedCenter.y - bounds->max.y);

				if (transformedCenter.z < bounds->min.z)
					distanceToBox += (transformedCenter.z - bounds->min.z) * (transformedCenter.z - bounds->min.z);
				else if (transformedCenter.z > bounds->max.z)
					distanceToBox += (transformedCenter.z - bounds->max.z) * (transformedCenter.z - bounds->max.z);
			}
			float minDistance = capsuleRadius;
			return insideBox || distanceToBox <= minDistance * minDistance;
		}*/
		return false;
	}
};

#endif
