#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "Collider.h"
#include "Particle.h"
#include "Constraints.h"

class BoundingBox : public Collider {
public:
    glm::vec3 min;
    glm::vec3 max;
    bool customSize;
    std::vector<glm::vec3> vertices;
    bool isTriggerOnly = false;
    std::vector<Particle*> particles;
    std::vector<DistanceConstraint*> constraints;

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
        for (auto& vertex : vertices) {
			particles.push_back(new Particle(vertex, mass, 1.0f));
		}
        addConstraints();
    }

    void addConstraints() {
        constraints.push_back(new DistanceConstraint(0, 1, glm::distance(particles[0]->position, particles[1]->position)));
        constraints.push_back(new DistanceConstraint(1, 2, glm::distance(particles[1]->position, particles[2]->position)));
        constraints.push_back(new DistanceConstraint(2, 3, glm::distance(particles[2]->position, particles[3]->position)));
        constraints.push_back(new DistanceConstraint(3, 0, glm::distance(particles[3]->position, particles[0]->position)));
        constraints.push_back(new DistanceConstraint(4, 5, glm::distance(particles[4]->position, particles[5]->position)));
        constraints.push_back(new DistanceConstraint(5, 6, glm::distance(particles[5]->position, particles[6]->position)));
        constraints.push_back(new DistanceConstraint(6, 7, glm::distance(particles[6]->position, particles[7]->position)));
        constraints.push_back(new DistanceConstraint(7, 4, glm::distance(particles[7]->position, particles[4]->position)));
        constraints.push_back(new DistanceConstraint(0, 4, glm::distance(particles[0]->position, particles[4]->position)));
        constraints.push_back(new DistanceConstraint(1, 5, glm::distance(particles[1]->position, particles[5]->position)));
        constraints.push_back(new DistanceConstraint(2, 6, glm::distance(particles[2]->position, particles[6]->position)));
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
        for (auto& vertex : vertices) {
            particles.push_back(new Particle(vertex, node["mass"].as<float>(), node["dampingFactor"].as<float>()));
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
        node["mass"] = particles[0]->mass;
        node["dampingFactor"] = particles[0]->dampingFactor;
        return node;
    }
};

#endif
