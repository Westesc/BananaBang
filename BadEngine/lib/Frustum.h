#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <array>

struct Plane {
    glm::vec3 normal;
    float distance;

    Plane() : normal(0.0f), distance(0.0f) {}
    Plane(glm::vec3 n, float d) : normal(n), distance(d) {}

    static std::array<Plane, 6> calculateFrustumPlanes(const glm::mat4& VP) {
        std::array<Plane, 6> planes;

        // Left plane
        planes[0].normal.x = VP[0][3] + VP[0][0];
        planes[0].normal.y = VP[1][3] + VP[1][0];
        planes[0].normal.z = VP[2][3] + VP[2][0];
        planes[0].distance = VP[3][3] + VP[3][0];

        // Right plane
        planes[1].normal.x = VP[0][3] - VP[0][0];
        planes[1].normal.y = VP[1][3] - VP[1][0];
        planes[1].normal.z = VP[2][3] - VP[2][0];
        planes[1].distance = VP[3][3] - VP[3][0];

        // Bottom plane
        planes[2].normal.x = VP[0][3] + VP[0][1];
        planes[2].normal.y = VP[1][3] + VP[1][1];
        planes[2].normal.z = VP[2][3] + VP[2][1];
        planes[2].distance = VP[3][3] + VP[3][1];

        // Top plane
        planes[3].normal.x = VP[0][3] - VP[0][1];
        planes[3].normal.y = VP[1][3] - VP[1][1];
        planes[3].normal.z = VP[2][3] - VP[2][1];
        planes[3].distance = VP[3][3] - VP[3][1];

        // Near plane
        planes[4].normal.x = VP[0][3] + VP[0][2];
        planes[4].normal.y = VP[1][3] + VP[1][2];
        planes[4].normal.z = VP[2][3] + VP[2][2];
        planes[4].distance = VP[3][3] + VP[3][2];

        // Far plane
        planes[5].normal.x = VP[0][3] - VP[0][2];
        planes[5].normal.y = VP[1][3] - VP[1][2];
        planes[5].normal.z = VP[2][3] - VP[2][2];
        planes[5].distance = VP[3][3] - VP[3][2];

        // Normalize the planes
        for (int i = 0; i < 6; ++i) {
            planes[i] = NormalizePlane(planes[i]);
        }

        return planes;
    }

    static Plane NormalizePlane(Plane p) {
        float length = glm::length(p.normal);
        p.normal /= length;
        p.distance /= length;
        return p;
    }
};

#endif
