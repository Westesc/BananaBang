#ifndef Section_H
#define Section_H

#include "BoundingBox.h"

class Section {
public:
	BoundingBox* bounds;
	std::vector<GameObject*> objects;

	Section(float x, float y, float z, float size) {
		bounds = new BoundingBox(glm::vec3(x, y, z), glm::vec3(x + size, y + size, z + size), true);
	}

	bool checkCollision(Model* object) {
		if (object->boundingBox != nullptr) {
			glm::vec3 transformedMin = glm::vec3(*object->getTransform() * glm::vec4(object->boundingBox->min, 1.0f));
			glm::vec3 transformedMax = glm::vec3(*object->getTransform() * glm::vec4(object->boundingBox->max, 1.0f));
			if (transformedMax.x < bounds->min.x || transformedMin.x > bounds->max.x) {
				return false;
			}
			if (transformedMax.y < bounds->min.y || transformedMin.y > bounds->max.y) {
				return false;
			}
			if (transformedMax.z < bounds->min.z || transformedMin.z > bounds->max.z) {
				return false;
			}
			return true;
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
};

#endif
