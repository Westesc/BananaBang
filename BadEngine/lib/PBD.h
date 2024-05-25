#ifndef PBD_H
#define PBD_H

#include "Component.h"
#include "../thirdparty/tracy/public/tracy/Tracy.hpp"
#include "Section.h"
#include <string>

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
							constraint->project(positions, object->boundingBox->particles[0]->inverseMass);
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
                            object->boundingBox->particles[i]->position.y += belowGround;
                            object->boundingBox->vertices[i].y += belowGround;
                        }
                    }
                    else if (object->capsuleCollider) {
                        std::vector<glm::vec3> positions = {
                            object->capsuleCollider->top->predictedPosition,
                            object->capsuleCollider->bottom->predictedPosition
                        };
                        for (CapsuleConstraint* constraint : object->capsuleCollider->constraints) {
                            constraint->project(positions, object->capsuleCollider->top->inverseMass);
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
                        if (object->name.starts_with("enemy") && object->getTransform()->localPosition.y < 5.0f) {
                            belowGround = 5.0f - object->getTransform()->localPosition.y;
                            object->getTransform()->localPosition.y = 5.0f;
                        }
                        object->capsuleCollider->top->position = object->capsuleCollider->top->predictedPosition;
                        object->capsuleCollider->top->position.y += belowGround;
                        object->capsuleCollider->bottom->position = object->capsuleCollider->bottom->predictedPosition;
                        object->capsuleCollider->bottom->position.y += belowGround;
                        object->capsuleCollider->center.y += belowGround;
                    }
				}
			}
        }
	}
};
#endif
