#ifndef CapsuleCollider_H
#define CapsuleCollider_H

#include <yaml-cpp/yaml.h>
#include "Serialize.h"
#include "Collider.h"
#include "Particle.h"

class CapsuleCollider : public Collider {
public:
    glm::vec3 center;
    float radius;
    float height;
    bool customSize;
    bool isTriggerOnly = false;
    Particle top = Particle(glm::vec3(0.0f), 1.0f);
    Particle bottom = Particle(glm::vec3(0.0f), 1.0f);
    std::vector<DistanceConstraint> constraints;

    CapsuleCollider(const glm::vec3& center, float radius, float height, float mass, bool custom = false)
        : center(center), radius(radius), height(height), customSize(custom), top(center + glm::vec3(0.0f, height * 0.5f, 0.0f), mass), bottom(center - glm::vec3(0.0f, height * 0.5f, 0.0f), mass) {}

    glm::vec3 nodeToVec3(YAML::Node node) {
        return glm::vec3(node["x"].as<float>(), node["y"].as<float>(), node["z"].as<float>());
    }

    CapsuleCollider(YAML::Node node) {
        if (node["center"]) {
            center = nodeToVec3(node["center"]);
        }
        radius = node["radius"].as<float>();
        height = node["height"].as<float>();
        top = Particle(nodeToVec3(node["top"]["position"]), node["top"]["mass"].as<float>());
        bottom = Particle(nodeToVec3(node["bottom"]["position"]), node["bottom"]["mass"].as<float>());
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
    YAML::Node nodeParticle(Particle particle)
    {
		YAML::Node node;
		node["position"] = nodeVec3(particle.position);
		node["predictedPosition"] = nodeVec3(particle.predictedPosition);
		node["velocity"] = nodeVec3(particle.velocity);
		node["force"] = nodeVec3(particle.force);
		node["mass"] = particle.mass;
		return node;
	}

    YAML::Node serialize() {
        YAML::Node node;
        node["center"] = nodeVec3(center);
        node["radius"] = radius;
        node["height"] = height;
        node["top"] = nodeParticle(top);
        node["bottom"] = nodeParticle(bottom);
        return node;
    }
};

#endif