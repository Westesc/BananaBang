#ifndef BoundingBox_H
#define BoundingBox_H

#include "Component.h"

class BoundingBox : Component {
public:
    glm::vec3 min;
    glm::vec3 max;

    BoundingBox(const glm::vec3& min, const glm::vec3& max)
        : min(min), max(max) {}

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
