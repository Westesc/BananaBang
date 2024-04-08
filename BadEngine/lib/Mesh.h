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

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        setupMesh();
        calculateBoundingBox();
    }

    Mesh()
    {
    }

    const BoundingBox& getBoundingBox() const {
        return boundingBox;
    }

    void updateBoundingBox(const glm::mat4& modelMatrix) {
        // Recalculate bounding box position based on transformed vertices
        glm::vec3 min(FLT_MAX);
        glm::vec3 max(-FLT_MAX);

        // Transform mesh vertices using the provided model matrix
        for (const auto& vertex : vertices) {
            glm::vec4 transformedVertex = modelMatrix * glm::vec4(vertex.Position, 1.0f);

            // Update bounding box extents
            min = glm::min(min, glm::vec3(transformedVertex));
            max = glm::max(max, glm::vec3(transformedVertex));
        }

        // Update bounding box
        boundingBox = BoundingBox(min, max);
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

    void generateTorus(int num_segments, int num_rings, float thickness, float radius) {
        std::vector<unsigned int> indexBuffer;
        std::vector<float> vertBuffer;
        int num_vertices = (num_rings + 1) * (num_segments + 1);

        const float pi = 3.1415926535f;
        const float r1 = radius;
        const float r2 = thickness;
        for (int i = 0, index = 0; i <= num_rings; ++i) {
            for (int j = 0; j <= num_segments; ++j, ++index) {
                float u = float(i) / num_rings;
                float v = (float(j) + u) / num_segments;

                float u_angle = u * 2 * pi;
                float v_angle = v * 2 * pi;

                float x = cos(u_angle) * (r1 + cos(v_angle) * r2);
                float y = sin(u_angle) * (r1 + cos(v_angle) * r2);
                float z = sin(v_angle) * r2;
                vertBuffer.push_back(x);
                vertBuffer.push_back(y);
                vertBuffer.push_back(z);

            }
        }

        for (int i = 0, index = 0; i <= num_vertices; ++i) {
            indexBuffer.push_back(int(i % num_vertices));
            indexBuffer.push_back(int((i + num_segments + 1) % num_vertices));
        }

        vert = vertBuffer;
        indices = indexBuffer;
        setupMeshTorus();
    }

private:
    BoundingBox boundingBox = BoundingBox(glm::vec3(FLT_MAX),glm::vec3(FLT_MIN));
    unsigned int VBO, EBO;

    void calculateBoundingBox() {
        glm::vec3 min(FLT_MAX);
        glm::vec3 max(-FLT_MAX);

        for (const auto& vertex : vertices) {
            min = glm::min(min, vertex.Position);
            max = glm::max(max, vertex.Position);
        }

        boundingBox = BoundingBox(min, max);
    }

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