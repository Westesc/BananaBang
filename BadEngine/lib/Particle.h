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

class Constraint {
public:
    virtual void project(std::vector<glm::vec3>& positions, float mass) const = 0;
};

struct DistanceConstraint : public Constraint {
    int p1;
    int p2;

    DistanceConstraint(int particle1, int particle2) : p1(particle1), p2(particle2) {}

    void project(std::vector<glm::vec3>& positions, float mass) const override {
        glm::vec3 delta = positions[p1] - positions[p2];
        float currentLength = glm::length(delta);
        float restLength = glm::distance(positions[p1], positions[p2]);
        float correction = (currentLength - restLength) / currentLength;

        float w1 = mass > 0 ? 1.0f / mass : 0.0f;
        float wSum = w1 * 2;

        if (wSum > 0.0f) {
            positions[p1] -= (w1 / wSum) * correction * delta;
            positions[p2] += (w1 / wSum) * correction * delta;
        }
    }
};
#endif
