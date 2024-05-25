#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H

class Constraint {
public:
    float k;
    Constraint() = default;
    ~Constraint() = default;
    virtual void project(std::vector<glm::vec3>& positions, float InverseMass) = 0;
};

class DistanceConstraint : public Constraint {
public:
    int particle1;
    int particle2;
    float restLength;

    DistanceConstraint(int p1, int p2, float length)
        : particle1(p1), particle2(p2), restLength(length) {}

    void project(std::vector<glm::vec3>& positions, float inverseMass) {
        glm::vec3 p1 = positions[particle1];
        glm::vec3 p2 = positions[particle2];
        glm::vec3 delta = p2 - p1;
        float currentLength = glm::length(delta);
        float correction = (currentLength - restLength) / currentLength;
        glm::vec3 correctionVector = delta * correction * 0.5f;
        positions[particle1] += correctionVector * inverseMass;
        positions[particle2] -= correctionVector * inverseMass;
    }
};

class CapsuleConstraint : public Constraint {
public:
    float height;

    CapsuleConstraint(float h) : height(h) {}

    void project(std::vector<glm::vec3>& positions, float inverseMass) {
        glm::vec3 p1 = positions[0];
        glm::vec3 p2 = positions[1];

        glm::vec3 delta = p2 - p1;
        float currentHeight = glm::length(delta);
        float heightCorrection = (currentHeight - height) / currentHeight;
        glm::vec3 heightCorrectionVector = delta * heightCorrection * 0.5f;
        positions[0] += heightCorrectionVector * inverseMass;
        positions[1] -= heightCorrectionVector * inverseMass;
    }
};
#endif
