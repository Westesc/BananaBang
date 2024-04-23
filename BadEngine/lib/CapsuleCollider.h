#ifndef CapsuleCollider_H
#define CapsuleCollider_H

#include <yaml-cpp/yaml.h>
#include "Serialize.h"
#include "Collider.h"

class CapsuleCollider : Collider {
public:
    glm::vec3 center;
    float radius;
    float height;
    bool customSize;

    CapsuleCollider(const glm::vec3& center, float radius, float height, bool custom = false)
        : center(center), radius(radius), height(height), customSize(custom) {}

    glm::vec3 nodeToVec3(YAML::Node node) {
        return glm::vec3(node["x"].as<float>(), node["y"].as<float>(), node["y"].as<float>());
    }

    CapsuleCollider(YAML::Node node) {
        if (node["center"]) {
            center = nodeToVec3(node["center"]);
        }
        radius = node["radius"].as<float>();
        height = node["height"].as<float>();
    }

    glm::vec3 getCenter() const { return center; }
    void setCenter(const glm::vec3& newCenter) { center = newCenter; }

    float getRadius() const { return radius; }
    void setRadius(float newRadius) { radius = newRadius; }

    float getHeight() const { return height; }
    void setHeight(float newHeight) { height = newHeight; }
    YAML::Node nodeVec3(glm::vec3 vector)
    {
        YAML::Node node;
        node["x"] = vector.x;
        node["y"] = vector.y;
        node["z"] = vector.z;
        return node;
    }
    YAML::Node serialize() {
        YAML::Node node;
        node["center"] = nodeVec3(center);
        node["radius"] = radius;
        node["height"] = height;
        return node;
    }
};

#endif