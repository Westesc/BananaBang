#ifndef SECTION_H
#define SECTION_H

#include <vector>
#include "BoundingBox.h"
#include "GameObject.h"
#include <glm/glm.hpp>

class Section {
public:
    BoundingBox* bounds;
    std::vector<GameObject*> objects;
    std::vector<GameObject*> staticObjects;
    GameObject* predictObject;
    int ID;

    Section(float x, float y, float z, float size, int ID) : ID(ID) {
        bounds = new BoundingBox(glm::vec3(x, y, z), glm::vec3(x + size, y + size, z + size), 0.0f, true);
    }

    bool checkFrustumCollision(BoundingBox* frustum, glm::mat4 matrix) {
        glm::vec3 vertices[] = {
            glm::vec3(matrix * glm::vec4(frustum->vertices.at(0), 1.0f)),
            glm::vec3(matrix * glm::vec4(frustum->vertices.at(1), 1.0f)),
            glm::vec3(matrix * glm::vec4(frustum->vertices.at(2), 1.0f)),
            glm::vec3(matrix * glm::vec4(frustum->vertices.at(3), 1.0f)),
            glm::vec3(matrix * glm::vec4(frustum->vertices.at(4), 1.0f)),
   			glm::vec3(matrix * glm::vec4(frustum->vertices.at(5), 1.0f)),
            glm::vec3(matrix * glm::vec4(frustum->vertices.at(6), 1.0f)),
			glm::vec3(matrix * glm::vec4(frustum->vertices.at(7), 1.0f))
		};
        for (int i = 0; i < 8; i++) {
            if (vertices[i].x >= bounds->min.x && vertices[i].x <= bounds->max.x &&
                vertices[i].y >= bounds->min.y && vertices[i].y <= bounds->max.y &&
                vertices[i].z >= bounds->min.z && vertices[i].z <= bounds->max.z) {
				return true;
			}
		}
		return false;
	}

    bool checkCollision(GameObject* object) {
        if (object->boundingBox) {
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
        else if (object->capsuleCollider) {
            glm::vec3 capsuleCenter = object->capsuleCollider->center;
            float capsuleRadius = object->capsuleCollider->radius;
            float capsuleHeight = object->capsuleCollider->height;
            glm::vec3 transformedCenter = glm::vec3(object->getTransform()->getMatrix() * glm::vec4(capsuleCenter, 1.0f));
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

    std::vector<Section*> getNeighborSections(std::vector<Section*> allSections, std::vector<int>* visited) {
        std::vector<Section*> neighbors;
        if (getSectionByID(ID - 1) && (std::find(visited->begin(), visited->end(), ID-1) == visited->end())) {
			neighbors.push_back(getSectionByID(ID - 1));
		}
        if (getSectionByID(ID + 1) && (std::find(visited->begin(), visited->end(), ID + 1) == visited->end())) {
            neighbors.push_back(getSectionByID(ID + 1));
        }
        if (getSectionByID(ID - 10) && (std::find(visited->begin(), visited->end(), ID - 10) == visited->end())) {
			neighbors.push_back(getSectionByID(ID - 10));
		}
        if (getSectionByID(ID + 10) && (std::find(visited->begin(), visited->end(), ID + 10) == visited->end())) {
            neighbors.push_back(getSectionByID(ID + 10));
        }
        if (getSectionByID(ID - 100) && (std::find(visited->begin(), visited->end(), ID - 100) == visited->end())) {
            neighbors.push_back(getSectionByID(ID - 100));
        }
        if (getSectionByID(ID + 100) && (std::find(visited->begin(), visited->end(), ID + 100) == visited->end())) {
			neighbors.push_back(getSectionByID(ID + 100));
		}
        return neighbors;
    }

    bool isNeighbor(Section* other) {
        float size = bounds->max.x - bounds->min.x;
        return std::abs(this->bounds->min.x - other->bounds->min.x) <= size &&
            std::abs(this->bounds->min.y - other->bounds->min.y) <= size &&
            std::abs(this->bounds->min.z - other->bounds->min.z) <= size;
    }

    Section* getSectionByID(int id) {
        if (ID == id) {
			return this;
		}
        return nullptr;
    }

};

#endif
