#ifndef CapsuleCollider_H
#define CapsuleCollider_H

#include "Collider.h"

class CapsuleCollider : Collider {
public:
    glm::vec3 center;
    float radius;
    float height;
    bool customSize;

    CapsuleCollider(const glm::vec3& center, float radius, float height, bool custom = false)
        : center(center), radius(radius), height(height), customSize(custom) {}

    glm::vec3 getCenter() const { return center; }
    void setCenter(const glm::vec3& newCenter) { center = newCenter; }

    float getRadius() const { return radius; }
    void setRadius(float newRadius) { radius = newRadius; }

    float getHeight() const { return height; }
    void setHeight(float newHeight) { height = newHeight; }
};

#endif