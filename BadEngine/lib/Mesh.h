#ifndef Mesh_H
#define Mesh_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <vector>
#include "Shader.h"
#include "BoundingBox.h"
#include "CapsuleCollider.h"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    //tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    GLfloat deltaTime;

    std::vector<float> vert;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int VAO;
    BoundingBox* boundingBox = nullptr;
    CapsuleCollider* capsuleCollider = nullptr;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        setupMesh();
        //calculateBoundingBox();
    }

    Mesh()
    {
    }

    const BoundingBox& getBoundingBox() const {
        return *boundingBox;
    }

    const CapsuleCollider& getCapsuleCollider() const {
        return *capsuleCollider;
    }

    void updateBoundingBox(glm::mat4& modelMatrix) {
        if (boundingBox->customSize) {
            glm::vec3 transformedMin = glm::vec3(modelMatrix * glm::vec4(boundingBox->min, 1.0f));
            glm::vec3 transformedMax = glm::vec3(modelMatrix * glm::vec4(boundingBox->max, 1.0f));
            boundingBox->min = transformedMin;
            boundingBox->max = transformedMax;
        }
        else {
            glm::vec3 min(FLT_MAX);
            glm::vec3 max(-FLT_MAX);
            for (const auto& vertex : vertices) {
                glm::vec4 transformedVertex = modelMatrix * glm::vec4(vertex.Position, 1.0f);

                // Update bounding box extents
                min = glm::min(min, glm::vec3(transformedVertex));
                max = glm::max(max, glm::vec3(transformedVertex));
            }

            // Update bounding box
            boundingBox = new BoundingBox(min, max);
        }
    }

    void updateCapsuleCollider(glm::mat4& modelMatrix) {
        glm::vec3 transformedCenter = glm::vec3(modelMatrix * glm::vec4(capsuleCollider->center, 1.0f));
        capsuleCollider->center = transformedCenter;
    }

    void setBoundingBox(const glm::vec3& customMin, const glm::vec3& customMax) {
        boundingBox = new BoundingBox(customMin, customMax, true);
    }

    void setCapsuleCollider(float radius, float height, glm::vec3 position) {
        if (capsuleCollider != nullptr) {
            capsuleCollider = new CapsuleCollider(capsuleCollider->getCenter(), radius, height, true);
        }
        else {
            calculateBoundingCapsule(position);
            capsuleCollider = new CapsuleCollider(capsuleCollider->getCenter(), radius, height, true);
        }
    }

    void calculateBoundingBox() {
        glm::vec3 min(FLT_MAX);
        glm::vec3 max(-FLT_MAX);

        for (const auto& vertex : vertices) {
            min = glm::min(min, vertex.Position);
            max = glm::max(max, vertex.Position);
        }

        boundingBox = new BoundingBox(min, max);
    }

    void calculateBoundingCapsule(glm::vec3 position) {
        float minY = FLT_MAX;
        float maxY = -FLT_MAX;
        float maxRadius = 0.0f;

        for (const auto& vertex : vertices) {
            float y = vertex.Position.y;
            minY = std::min(minY, y);
            maxY = std::max(maxY, y);
            maxRadius = std::max(maxRadius, glm::length(glm::vec2(vertex.Position.x, vertex.Position.z)));
        }
        float height = maxY - minY;
        capsuleCollider = new CapsuleCollider(glm::vec3(position.x, (minY + maxY) * 0.5f* 0.1f, position.z), maxRadius* 0.1f, height *0.1f);
    }

    // render the mesh
    void Draw(Shader* shader, glm::mat4* model, bool& isFromFile, bool& rotating, bool& isBlue)
    {

        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        shader->use();
        shader->setMat4("model", *model);

        if (!isFromFile) {
            shader->setBool("isBlue", isBlue);
        }

        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);

            std::string number;
            std::string name = textures[i].type;

            if (name == "texture_diffuse") {
                number = std::to_string(diffuseNr);
                diffuseNr++;
            }
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);
            else if (name == "texture_normal")
                number = std::to_string(normalNr++);
            else if (name == "texture_height")
                number = std::to_string(heightNr++);

            glUniform1i(glGetUniformLocation(shader->ID, (name + number).c_str()), i);

            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }


        glBindVertexArray(VAO);
        if (indices.size() != 0)
        {
            if (isFromFile) {
                glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
                glActiveTexture(GL_TEXTURE0);
            }
            else
                glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
        }
        else {
            glDrawArrays(GL_POINTS, 0, vert.size());
        }
        glBindVertexArray(0);
    }

    void createSphere(int verticalSegments, int horizontalSegments, int size) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for (int j = 0; j <= verticalSegments; ++j) {
            for (int i = 0; i <= horizontalSegments; ++i) {
                float theta = j * glm::pi<float>() / verticalSegments;
                float phi = i * 2 * glm::pi<float>() / horizontalSegments;
                float x = size*sin(theta) * cos(phi);
                float y = size*cos(theta);
                float z = size*sin(theta) * sin(phi);
                vertices.push_back({ glm::vec3(x, y, z), glm::vec3(x, y, z), glm::vec2(i / (float)horizontalSegments, j / (float)verticalSegments) });
            }
        }

        for (int j = 0; j < verticalSegments/1.5; ++j) {
            for (int i = 0; i < horizontalSegments; ++i) {
                int first = (j * (horizontalSegments + 1)) + i;
                int second = first + horizontalSegments + 1;
                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }

        this->vertices = vertices;
        this->indices = indices;

        setupMesh();
    }
private:
    unsigned int VBO, EBO;

    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
    }

    void setupMeshTorus()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
    }
};
#endif