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
    std::vector<GameObject*> objects;

    void simulateB4Collisions(float deltaTime) {
        ZoneScopedN("PBD Simulation before collisions");
        for (auto& object : objects) {
            object->updateVelocity(deltaTime);
            float yVel = object->velocity.y;
            object->velocity *= (object->dampingFactor - (int(object->friction) * 0.25f * object->dampingFactor));
            object->velocity.y = yVel;
            object->predictPosition(deltaTime);
            if (object->name == "player") {
                //std::cout << "first predict: " << glm::to_string(object->predictedPosition) << std::endl;
            }
            object->calculateOffset();
        }
    }
    void simulateAfterCollisions(float deltaTime) {
		ZoneScopedN("PBD Simulation after collisions");
        /*for (int i = 0; i < iterations; i++) {
            for (auto& object : objects) {
                if (object->boundingBox) {
                    std::vector<glm::vec3> positions;
                    for (Particle* particle : object->boundingBox->particles) {
                        positions.push_back(particle->predictedPosition);
                    }
                    for (DistanceConstraint* constraint : object->boundingBox->constraints) {
                        constraint->project(positions, object->boundingBox->particles[0]->inverseMass);
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
                }
            }
        }*/
        for (auto& object : objects) {
            //object->updatePredictedPosition();
            glm::vec3 displacement = object->localTransform->predictedPosition - object->getTransform()->localPosition;
            object->velocity = displacement / deltaTime;
            object->getTransform()->localPosition = object->localTransform->predictedPosition;
            if (object->getTransform()->localPosition.y <= object->capsuleCollider->height * 0.5f * object->localTransform->localScale.y) {
                object->getTransform()->localPosition.y = object->capsuleCollider->height * 0.5f * object->localTransform->localScale.y;
                if (object->name == "player") {
                    object->getTransform()->localPosition.y = 0.0f;
                }
                object->velocity.y = 0.0f;
            }
            if (object->name.starts_with("enemy") && object->getTransform()->localPosition.y <= 2.0f) {
                object->getTransform()->localPosition.y = 2.0f;
                object->velocity.y = 0.0f;
            }
        }
	}
};
#endif
