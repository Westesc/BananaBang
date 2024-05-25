#ifndef CAPSULECOLLIDER_H
#define CAPSULECOLLIDER_H

#include <yaml-cpp/yaml.h>

class CapsuleCollider : public Collider {
public:
    glm::vec3 center;
    float radius;
    float height;
    bool customSize;
    bool isTriggerOnly = false;
    Particle* top = new Particle(glm::vec3(0.0f), 1.0f, 1.0f);
    Particle* bottom = new Particle(glm::vec3(0.0f), 1.0f, 1.0f);
    std::vector<CapsuleConstraint*> constraints;

    CapsuleCollider(const glm::vec3& center, float radius, float height, float mass, bool custom = false)
        : center(center), radius(radius), height(height), customSize(custom), top(new Particle(center + glm::vec3(0.0f, height * 0.5f, 0.0f), mass, 1.0f)), bottom(new Particle(center - glm::vec3(0.0f, height * 0.5f, 0.0f), mass, 1.0f)) {
    	constraints.push_back(new CapsuleConstraint(height));
    }

    glm::vec3 nodeToVec3(YAML::Node node) {
        return glm::vec3(node["x"].as<float>(), node["y"].as<float>(), node["z"].as<float>());
    }

    CapsuleCollider(YAML::Node node) {
        if (node["center"]) {
            center = nodeToVec3(node["center"]);
        }
        radius = node["radius"].as<float>();
        height = node["height"].as<float>();
        top = new Particle(nodeToVec3(node["top"]["position"]), node["top"]["mass"].as<float>(), node["dampingFactor"].as<float>());
        bottom = new Particle(nodeToVec3(node["bottom"]["position"]), node["bottom"]["mass"].as<float>(), node["dampingFactor"].as<float>());
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
    YAML::Node nodeParticle(Particle* particle)
    {
		YAML::Node node;
		node["position"] = nodeVec3(particle->position);
		node["predictedPosition"] = nodeVec3(particle->predictedPosition);
		node["velocity"] = nodeVec3(particle->velocity);
		node["force"] = nodeVec3(particle->force);
		node["mass"] = particle->mass;
		return node;
	}

    YAML::Node serialize() {
        YAML::Node node;
        node["center"] = nodeVec3(center);
        node["radius"] = radius;
        node["height"] = height;
        node["top"] = nodeParticle(top);
        node["bottom"] = nodeParticle(bottom);
        node["dampingFactor"] = top->dampingFactor;
        return node;
    }
};

#endif