#ifndef PARTICLE_H
#define PARTICLE_H

struct Particle {
    glm::vec3 position;
    glm::vec3 predictedPosition;
    glm::vec3 velocity;
    glm::vec3 force;
    float mass;

    Particle(const glm::vec3& pos, float m)
        : position(pos), predictedPosition(pos), velocity(0.0f), force(0.0f), mass(m) {}
};
#endif
