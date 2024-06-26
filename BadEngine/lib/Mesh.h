#ifndef MESH_H
#define MESH_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <vector>
#include <yaml-cpp/yaml.h>
#include "../thirdparty/tracy/public/tracy/Tracy.hpp"

#include "Shader.h"
#include "BoundingBox.h"
#include "CapsuleCollider.h"
#include "Transform.h"

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

enum meshType {
    fromFile,
    sphere,
    dome
};

class Mesh {
public:
    GLfloat deltaTime;

    std::vector<float> vert;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    meshType type = fromFile;
    unsigned int VAO;
    BoundingBox* boundingBox = nullptr;
    CapsuleCollider* capsuleCollider = nullptr;
    //zmienne do sfery
    int verticalSegments, horizontalSegments, size;


    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        setupMesh();
        //calculateBoundingBox();
    }

    Mesh(YAML::Node node) {
        if (node["type"].as<int>() == (int)sphere) {
            createSphere(node["verticalSegments"].as<int>(), node["horizontalSegments"].as<int>(), node["size"].as<int>());
        }
        else if(node["type"].as<int>() == (int)dome) {
            createDome(node["verticalSegments"].as<int>(), node["horizontalSegments"].as<int>(), node["size"].as<int>());
        }
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

    /*void updateBoundingBox(glm::mat4& modelMatrix) {
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
    }*/

    void updateCapsuleCollider(glm::mat4& modelMatrix) {
        glm::vec3 transformedCenter = glm::vec3(modelMatrix * glm::vec4(capsuleCollider->center, 1.0f));
        capsuleCollider->center = transformedCenter;
    }

    void setBoundingBox(const glm::vec3& customMin, const glm::vec3& customMax) {
        boundingBox = new BoundingBox(customMin, customMax, true);
    }

    /*void setCapsuleCollider(float radius, float height, glm::vec3 position) {
        if (capsuleCollider != nullptr) {
            capsuleCollider = new CapsuleCollider(capsuleCollider->getCenter(), radius, height, true);
        }
        else {
            calculateBoundingCapsule(position);
            capsuleCollider = new CapsuleCollider(capsuleCollider->getCenter(), radius, height, true);
        }
    }*/

    /*void calculateBoundingBox() {
        glm::vec3 min(FLT_MAX);
        glm::vec3 max(-FLT_MAX);

        for (const auto& vertex : vertices) {
            min = glm::min(min, vertex.Position);
            max = glm::max(max, vertex.Position);
        }

        boundingBox = new BoundingBox(min, max);
    }*/

    /*void calculateBoundingCapsule(glm::vec3 position) {
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
    }*/

    // render the mesh
    void Draw(Shader* shader, std::vector<Texture> texture, glm::mat4* model, bool& isFromFile, bool& rotating, bool& isBlue)
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

        for (unsigned int i = 0; i < texture.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);

            std::string number;
            std::string name = texture[i].type;

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

            glBindTexture(GL_TEXTURE_2D, texture[i].id);
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
        float angleYDiff = 180.f / (float)horizontalSegments;
        float angleXZDiff = 360.f / (float)verticalSegments;
        this->verticalSegments = verticalSegments;
        this->horizontalSegments = horizontalSegments;
        this->size = size;
        type = sphere;
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

    void createDome(int verticalSegments, int horizontalSegments, int size) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        this->verticalSegments = verticalSegments;
        this->horizontalSegments = horizontalSegments;
        this->size = size;
        type = dome;
        for (int j = 0; j <= verticalSegments; ++j) {
            for (int i = 0; i <= horizontalSegments; ++i) {
                float theta = j * glm::pi<float>() / verticalSegments;
                float phi = i * 2 * glm::pi<float>() / horizontalSegments;
                float x = size * sin(theta) * cos(phi);
                float y = size * cos(theta);
                float z = size * sin(theta) * sin(phi);
                vertices.push_back({ glm::vec3(x, y, z), glm::vec3(x, y, z), glm::vec2(i / (float)horizontalSegments, j / (float)verticalSegments) });
            }
        }
        for (int j = 0; j < verticalSegments / 1.5; ++j) {
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

    YAML::Node serialize()
    {
        YAML::Node node;
        
        /*for (auto v : vertices) {
            YAML::Node vertexNode;
            vertexNode["position"] = nodeVec3(v.Position);
            vertexNode["normal"] = nodeVec3(v.Normal);
            vertexNode["texCoords"] = nodeVec2(v.TexCoords);
            node["vertex"].push_back(vertexNode);
        }
        for (auto i : indices) {
            node["indices"].push_back(i);
        }
        for (auto t : textures) {
            YAML::Node textureNode;
            textureNode["id"] = t.id;
            textureNode["path"] = t.path;
            textureNode["type"] = t.type;
            node["vertex"].push_back(textureNode);
        }*/
        if (type == sphere || type == dome) {
            node["type"] = (int)type;
            node["verticalSegments"] = verticalSegments;
            node["horizontalSegments"] = horizontalSegments;
            node["size"] = size;
        }
        

        return node;
    }
    std::vector<glm::mat4> instanceMatrices;
    unsigned int instanceVBO;
    std::size_t currentBufferSize = 0;

    void initInstances(std::vector<Transform*> transforms) {
        if (glIsBuffer(instanceVBO)) {
            glDeleteBuffers(1, &instanceVBO);
        }
        instanceMatrices.clear();
        for (Transform* transform : transforms) {
            instanceMatrices.push_back(transform->getMatrix());
        }
        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

        glBufferData(GL_ARRAY_BUFFER, instanceMatrices.size() * sizeof(glm::mat4), &instanceMatrices[0], GL_STATIC_DRAW);

        glBindVertexArray(VAO);
        std::size_t vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        //glBindVertexArray(0);
    }

    void drawInstances(Shader* shader, std::vector<Texture> texture) {
        ZoneTransientN(zoneName, "drawInstances", true);

        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        shader->use();

        for (unsigned int i = 0; i < texture.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);

            std::string number;
            std::string name = texture[i].type;

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

            glBindTexture(GL_TEXTURE_2D, texture[i].id);
        }

        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instanceMatrices.size());
        glBindVertexArray(0);
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

        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        // ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
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

    YAML::Node nodeVec2(glm::vec2 vector)
    {
        YAML::Node node;
        node["x"] = vector.x;
        node["y"] = vector.y;
        return node;
    }
    YAML::Node nodeVec3(glm::vec3 vector)
    {
        YAML::Node node;
        node["x"] = vector.x;
        node["y"] = vector.y;
        node["z"] = vector.z;
        return node;
    }
};
#endif