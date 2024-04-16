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

    void createSphere(int verticalSegments, int horizontalSegments) {


        float angleYDiff = 180.f / (float)horizontalSegments;
        float angleXZDiff = 360.f / (float)verticalSegments;

        std::vector<glm::vec3> verts = std::vector<glm::vec3>();
        std::vector<glm::vec2> texCoords = std::vector<glm::vec2>();
        std::vector<Vertex> vertics = std::vector<Vertex>();
        std::vector<unsigned int> ind = std::vector<unsigned int>();

        //top
        verts.push_back({ 0.f, 1.f, 0.f });
        texCoords.push_back({ .5f, 1.f });

        float horizontalDiff = 1.f / (float)horizontalSegments;
        float verticalDiff = 1.f / (float)verticalSegments;


        float angleY = angleYDiff;

        //middle
        for (unsigned int i = 0; i < horizontalSegments - 1; ++i) {
            float radiansY = glm::radians(angleY);
            float r = sinf(radiansY);
            float y = cosf(radiansY);
            unsigned int startV = i * verticalSegments + 1;

            float angleXZ = 0.f;
            for (unsigned int j = 0; j < verticalSegments; ++j) {
                float radiansXZ = glm::radians(angleXZ);
                float z = r * cosf(radiansXZ);
                float x = r * sinf(radiansXZ);
                verts.push_back({ x, y, z });
                texCoords.push_back({ j * verticalDiff, 1.f - horizontalDiff * (i + 1) });

                if (j == verticalSegments - 1)
                {
                    verts.push_back({ r * sinf(glm::radians(0.0f)), y, r * cosf(glm::radians(0.0f)) });
                }
                angleXZ += angleXZDiff;

                if ((j + 1) % verticalSegments == 0)
                {
                    texCoords.push_back({ 1.f , 1.f - horizontalDiff * (i + 1) });
                }
            }
            angleY += angleYDiff;
        }
        //down
        verts.push_back({ 0.f, -1.f, 0.f });
        texCoords.push_back({ .5f, 0.f });

        unsigned int verticiesNum = verts.size();
        for (unsigned int i = 0; i < verticalSegments; ++i) {
            //top 
            unsigned int leftVertex = i + 1;
            unsigned int topVertex = 0;
            unsigned int rightVertex = (i + 1) + 1;
            ind.push_back(leftVertex); ind.push_back(topVertex); ind.push_back(rightVertex);
            // down 
            leftVertex = verticiesNum - 2 - verticalSegments + i;
            topVertex = verticiesNum - 1;
            rightVertex = verticiesNum - 2 - verticalSegments + i + 1;
            ind.push_back(topVertex); ind.push_back(leftVertex); ind.push_back(rightVertex);
        }
        // rest 
        for (unsigned int i = 0; i < horizontalSegments - 2; ++i) {
            unsigned int startVertex = i * (verticalSegments + 1) + 1;
            for (unsigned int j = 0; j < verticalSegments; ++j) {
                unsigned int topLeft = j + startVertex;
                unsigned int topRight = (j + 1) + startVertex;
                unsigned int bottomLeft = j + verticalSegments + 1 + startVertex;
                unsigned int bottomRight = (j + 1) + verticalSegments + 1 + startVertex;

                ind.push_back(bottomLeft); ind.push_back(topLeft); ind.push_back(topRight);
                ind.push_back(bottomLeft); ind.push_back(topRight); ind.push_back(bottomRight);
            }
        }

        for (unsigned int i = 0; i < verticiesNum; ++i)
        {
            vertics.push_back({ verts[i], verts[i], texCoords[i] });
        }

        this->vertices = vertics;
        this->indices = ind;

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