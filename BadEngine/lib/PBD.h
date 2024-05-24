#ifndef PBD_H
#define PBD_H

#include "Component.h"
#include "../thirdparty/tracy/public/tracy/Tracy.hpp"
#include "Section.h"

class Particle : public Component {
public:
    glm::vec3 position;
    glm::vec3 predictedPosition;
    glm::vec3 velocity;
    glm::vec3 force;
    float mass;
    float inverseMass;
    float dampingFactor;

    bool friction = false;
    bool controllable = true;
    const float kFriction = 0.35f;

    Particle(const glm::vec3& pos, float m, float damp)
        : position(pos), predictedPosition(pos), velocity(glm::vec3(0.0f)), force(glm::vec3(0.0f)), mass(m), inverseMass(m > 0 ? 1.0f / m : 0.0f), dampingFactor(damp) {}
    ~Particle() {}

    void AddForce(glm::vec3 Force) {
        force += Force;
    }
    void ZeroForces() {
		force = glm::vec3(0.0f);
    }
    void SympleticEulerIntegration(float t) {
        UpdateVelocity(t);
        DampVelocity();
        ZeroForces();
        PredictPosition(t);
    }
    void DampVelocity() {
        velocity *= (dampingFactor - (int(friction) * kFriction * dampingFactor));
    }
    void UpdateVelocity(float t) {
        glm::clamp(force, 3000.0f, -3000.f);
        velocity = (predictedPosition - position) / t + t * inverseMass * force;
        velocity.y = 0.0;
    }
    void PredictPosition(float t) {
        predictedPosition = position + t * velocity;
    }
    void UpdatePosition(float t) {
		position = predictedPosition;
    }
};

class Constraint {
public:
    float k;
    Constraint() = default;
    ~Constraint() = default;
    virtual void project(std::vector<glm::vec3>& positions, float mass) = 0;
};

class DistanceConstraint : public Constraint {
public:
    int particle1;
    int particle2;
    float restLength;

    DistanceConstraint(int p1, int p2, float length)
        : particle1(p1), particle2(p2), restLength(length) {}

    void project(std::vector<glm::vec3>& positions, float mass) {
        glm::vec3 p1 = positions[particle1];
        glm::vec3 p2 = positions[particle2];
        glm::vec3 delta = p2 - p1;
        float currentLength = glm::length(delta);
        float correction = (currentLength - restLength) / currentLength;
        glm::vec3 correctionVector = delta * correction * 0.5f;
        positions[particle1] += correctionVector * (1.0f / mass);
        positions[particle2] -= correctionVector * (1.0f / mass);
    }
};

class CapsuleConstraint : public Constraint {
public:
    float height;

    CapsuleConstraint(float h) : height(h) {}

    void project(std::vector<glm::vec3>& positions, float mass) {
        glm::vec3 p1 = positions[0];
        glm::vec3 p2 = positions[1];

        glm::vec3 delta = p2 - p1;
        float currentHeight = glm::length(delta);
        float heightCorrection = (currentHeight - height) / currentHeight;
        glm::vec3 heightCorrectionVector = delta * heightCorrection * 0.5f;
        positions[0] += heightCorrectionVector * (1.0f / mass);
        positions[1] -= heightCorrectionVector * (1.0f / mass);
    }
};

class PBDManager {
public:
    PBDManager(int it) {
		iterations = it;
    }
    ~PBDManager() = default;

    int iterations;

    void simulateB4Collisions(std::vector<Section*> sections, float deltaTime) {
        ZoneScopedN("PBD Simulation before collisions");
        for (auto section : sections) {
            for (auto object : section->objects) {
                object->updateVelocity(deltaTime);
                object->velocity *= object->dampingFactor;
                object->predictPosition(deltaTime);
                object->calculateOffset();
            }
        }
    }
    void simulateAfterCollisions(std::vector<Section*> sections, float deltaTime) {
		ZoneScopedN("PBD Simulation after collisions");
        for (int i = 0; i < iterations; i++) {
            for (auto section : sections) {
				for (auto object : section->objects) {
                    if (object->boundingBox) {
                        std::vector<glm::vec3> positions;
                        for (Particle* particle : object->boundingBox->particles) {
							positions.push_back(particle->predictedPosition);
						}
                        for (DistanceConstraint* constraint : object->boundingBox->constraints) {
							constraint->project(positions, object->boundingBox->particles[0]->mass);
						}
                        object->updatePredictedPosition();
                        glm::vec3 displacement = object->predictedPosition - object->getTransform()->localPosition;
                        object->velocity = displacement / deltaTime;
                        object->getTransform()->localPosition = object->predictedPosition;
                        float belowGround = 0.0f;
                        if (object->getTransform()->localPosition.y < 0.0f) {
                            belowGround = -object->getTransform()->localPosition.y;
                            object->getTransform()->localPosition.y = 0.0f;
                        }
                        for (int i = 0; i < 8; i++) {
                            object->boundingBox->particles[i]->position = object->boundingBox->particles[i]->predictedPosition;
                            object->boundingBox->particles[i]->position.y -= belowGround;
                            object->boundingBox->vertices[i].y -= belowGround;
                        }
                    }
                    else if (object->capsuleCollider) {
                        std::vector<glm::vec3> positions = {
                            object->capsuleCollider->top->predictedPosition,
                            object->capsuleCollider->bottom->predictedPosition
                        };
                        for (CapsuleConstraint* constraint : object->capsuleCollider->constraints) {
                            constraint->project(positions, object->capsuleCollider->top->mass);
                        }
                        object->updatePredictedPosition();
                        glm::vec3 displacement = object->predictedPosition - object->getTransform()->localPosition;
                        object->velocity = displacement / deltaTime;
                        object->getTransform()->localPosition = object->predictedPosition;
                        float belowGround = 0.0f;
                        if (object->getTransform()->localPosition.y < 0.0f) {
                            belowGround = -object->getTransform()->localPosition.y;
                            object->getTransform()->localPosition.y = 0.0f;
                        }
                        object->capsuleCollider->top->position = object->capsuleCollider->top->predictedPosition;
                        object->capsuleCollider->top->position.y -= belowGround;
                        object->capsuleCollider->bottom->position = object->capsuleCollider->bottom->predictedPosition;
                        object->capsuleCollider->bottom->position.y -= belowGround;
                        object->capsuleCollider->center.y -= belowGround;
                    }
				}
			}
        }
	}
};
#endif
