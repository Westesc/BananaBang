#ifndef BoundingBox_H
#define BoundingBox_H

#include <yaml-cpp/yaml.h>
#include "Collider.h"
#include "Serialize.h"

class BoundingBox : Collider {
public:
    glm::vec3 min;
    glm::vec3 max;
    bool customSize;

    glm::vec3 nodeToVec3(YAML::Node node) {
        return glm::vec3(node["x"].as<float>(), node["y"].as<float>(), node["y"].as<float>());
    }

    BoundingBox(const glm::vec3& min, const glm::vec3& max, bool custom = false)
        : min(min), max(max), customSize(custom) {}

    BoundingBox(YAML::Node node) {
        if (node["min"]) {
            min = nodeToVec3(node["min"]);
        }
        if (node["max"]) {
            max = nodeToVec3(node["max"]);
        }
    }
    glm::vec3 center() const {
        return 0.5f * (min + max);
    }
    float radius() const {
        glm::vec3 halfDiagonal = (max - min) * 0.5f;
        float halfDiagonalLength = glm::length(halfDiagonal);
        return halfDiagonalLength;
    }
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
        node["min"] = nodeVec3(min);
        node["max"] = nodeVec3(max);
        return node;
    }
};

#endif
