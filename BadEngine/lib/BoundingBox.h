#ifndef BoundingBox_H
#define BoundingBox_H

#include "Collider.h"

class BoundingBox : Collider {
public:
    glm::vec3 min;
    glm::vec3 max;
    bool customSize;

    BoundingBox(const glm::vec3& min, const glm::vec3& max, bool custom = false)
        : min(min), max(max), customSize(custom) {}

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
