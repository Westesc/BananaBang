#ifndef BoundingBox_H
#define BoundingBox_H

#include "Collider.h"
#include "Particle.h"

struct BoxConstraint {
    Particle* p1;
    Particle* p2;
    float restLength;

    BoxConstraint(Particle* particle1, Particle* particle2)
        : p1(particle1), p2(particle2), restLength(glm::distance(particle1->position, particle2->position)) {}

    void solve() {
        glm::vec3 delta = p1->predictedPosition - p2->predictedPosition;
        float distance = glm::length(delta);
        glm::vec3 correction = (distance - restLength) * delta / distance * 0.5f;
        p1->predictedPosition -= correction;
        p2->predictedPosition += correction;
    }
};

class BoundingBox : public Collider {
public:
    glm::vec3 min;
    glm::vec3 max;
    bool customSize;
    std::vector<glm::vec3> vertices;
    bool isTriggerOnly = false;
    std::vector<Particle> particles;

    glm::vec3 nodeToVec3(YAML::Node node) {
        return glm::vec3(node["x"].as<float>(), node["y"].as<float>(), node["z"].as<float>());
    }

    BoundingBox(const glm::vec3& min, const glm::vec3& max, float mass, bool custom = false)
        : min(min), max(max), customSize(custom) {
        vertices.push_back(min);
    	vertices.push_back(max);
        vertices.push_back(glm::vec3(min.x, min.y, max.z));
        vertices.push_back(glm::vec3(min.x, max.y, min.z));
        vertices.push_back(glm::vec3(max.x, min.y, min.z));
        vertices.push_back(glm::vec3(min.x, max.y, max.z));
        vertices.push_back(glm::vec3(max.x, min.y, max.z));
        vertices.push_back(glm::vec3(max.x, max.y, min.z));
        for (auto vertex : vertices) {
			particles.push_back(Particle(vertex, mass));
		}
    }

    BoundingBox(YAML::Node node) {
        min = nodeToVec3(node["min"]);
        max = nodeToVec3(node["max"]);
        customSize = node["customSize"].as<bool>();
        vertices.push_back(min);
        vertices.push_back(max);
        vertices.push_back(glm::vec3(min.x, min.y, max.z));
        vertices.push_back(glm::vec3(min.x, max.y, min.z));
        vertices.push_back(glm::vec3(max.x, min.y, min.z));
        vertices.push_back(glm::vec3(min.x, max.y, max.z));
        vertices.push_back(glm::vec3(max.x, min.y, max.z));
        vertices.push_back(glm::vec3(max.x, max.y, min.z));
        for (auto vertex : vertices) {
            particles.push_back(Particle(vertex, node["mass"].as<float>()));
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
        node["customSize"] = customSize;
        node["mass"] = particles[0].mass;
        return node;
    }
};

#endif
