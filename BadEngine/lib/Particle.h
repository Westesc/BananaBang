#ifndef PARTICLE_H
#define PARTICLE_H

#include <glm/glm.hpp>

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
#endif
