#ifndef BoundingBox_H
#define BoundingBox_H

#include "Collider.h"

class BoundingBox : Collider {
public:
    glm::vec3 min;
    glm::vec3 max;
    bool customSize;
    std::vector<glm::vec3> vertices;
    BoundingBox(const glm::vec3& min, const glm::vec3& max, bool custom = false)
        : min(min), max(max), customSize(custom) {
        vertices.push_back(min);
    	vertices.push_back(max);
        vertices.push_back(glm::vec3(min.x, min.y, max.z));
        vertices.push_back(glm::vec3(min.x, max.y, min.z));
        vertices.push_back(glm::vec3(max.x, min.y, min.z));
        vertices.push_back(glm::vec3(min.x, max.y, max.z));
        vertices.push_back(glm::vec3(max.x, min.y, max.z));
        vertices.push_back(glm::vec3(max.x, max.y, min.z));
    }

    glm::vec3 center() const {
        return 0.5f * (min + max);
    }
    float radius() const {
        glm::vec3 halfDiagonal = (max - min) * 0.5f;
        float halfDiagonalLength = glm::length(halfDiagonal);
        return halfDiagonalLength;
    }
};

#endif
