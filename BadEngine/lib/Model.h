#ifndef Model_H
#define Model_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "Mesh.h"


class Model
{
private:
    glm::mat4* Transform;
    std::vector<Texture> textures_loaded;
    std::vector<Mesh*> meshes;
    std::string directory;
    Shader* shader;
    bool isFromFile;

    float x;
    float y;
    float z;
    unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false)
    {
        std::string filename = std::string(path);
        filename = directory + '/' + filename;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }
    void loadModel(std::string path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }

        directory = path.substr(0, path.find_last_of('/'));
        std::cout << directory << std::endl;

        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode* node, const aiScene* scene)
    {

        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }
    Mesh* processMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            vertices.push_back(vertex);

            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // przetwórz materia³y
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
                aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            std::vector<Texture> specularMaps = loadMaterialTextures(material,
                aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }

        return new Mesh(vertices, indices, textures);
    }

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
    {
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip)
            {
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }
public:
    bool rotating;
    bool isBlue, iswhite;
    void setTransform(glm::mat4* matrix) { Transform = matrix; }
    glm::mat4* getTransform() { return Transform; }
    void SetShader(Shader* s) { shader = s; }

    Model(char* path, bool rotate = true)
    {
        rotating = rotate;
        isFromFile = true;
        loadModel(path);
        Transform = new glm::mat4(1);
    }

    Model(Mesh* mesh, bool isBlue = false, bool rotate = true)
    {
        this->isBlue = isBlue;
        rotating = rotate;
        isFromFile = false;
        meshes.push_back(mesh);
        Transform = new glm::mat4(1);
    }

    void Draw()
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
        {
            meshes[i]->Draw(shader, Transform, isFromFile, rotating, isBlue);
        }
    }

    void DrawBoundingBox(const BoundingBox& bbox, Shader* shader, glm::mat4* model) {
        glm::vec3 vertices[] = {
            glm::vec3(bbox.min.x, bbox.min.y, bbox.min.z),
            glm::vec3(bbox.max.x, bbox.min.y, bbox.min.z),
            glm::vec3(bbox.max.x, bbox.max.y, bbox.min.z),
            glm::vec3(bbox.min.x, bbox.max.y, bbox.min.z),
            glm::vec3(bbox.min.x, bbox.min.y, bbox.max.z),
            glm::vec3(bbox.max.x, bbox.min.y, bbox.max.z),
            glm::vec3(bbox.max.x, bbox.max.y, bbox.max.z),
            glm::vec3(bbox.min.x, bbox.max.y, bbox.max.z)
        };

        unsigned int indices[] = {
            0, 1, 1, 2, 2, 3, 3, 0, // Bottom face
            4, 5, 5, 6, 6, 7, 7, 4, // Top face
            0, 4, 1, 5, 2, 6, 3, 7  // Connections between top and bottom faces
        };
        unsigned int VBO, VAO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glDrawElements(GL_LINES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void DrawBoundingBoxes(Shader* shader, glm::mat4 transformation) {
        for (const auto& mesh : meshes) {
            mesh->updateBoundingBox(transformation);
            DrawBoundingBox(mesh->getBoundingBox(), shader, Transform);
        }
    }

    void setCustomCollider(const glm::vec3& customMin, const glm::vec3& customMax) {
        for (const auto& mesh : meshes) {
            mesh->setBoundingBox(customMin, customMax);
        }
    }

    bool checkBoundingBoxCollision(const BoundingBox& box1, const BoundingBox& box2) {
        bool collisionX = box1.max.x >= box2.min.x && box1.min.x <= box2.max.x;
        bool collisionY = box1.max.y >= box2.min.y && box1.min.y <= box2.max.y;
        bool collisionZ = box1.max.z >= box2.min.z && box1.min.z <= box2.max.z;
        return collisionX && collisionY && collisionZ;
    }

    bool checkCollision(const Model* other) {
        for (const auto& mesh : meshes) {
            const BoundingBox& meshBox = mesh->getBoundingBox();
            bool collisionFound = false;
            for (const auto& otherMesh : other->meshes) {
                const BoundingBox& otherMeshBox = otherMesh->getBoundingBox();
                if (checkBoundingBoxCollision(meshBox, otherMeshBox)) {
                    collisionFound = true;
                    break;
                }
            }
            if (!collisionFound) {
                return false;
            }
        }
        return true;
    }

    std::vector<BoundingBox> getBoundingBoxes() {
        std::vector<BoundingBox> boxes;
        for (const auto& mesh : meshes) {
            boxes.push_back(mesh->getBoundingBox());
        }
        return boxes;
    }
    glm::vec3 calculateCollisionResponse(const Model* other) {
        glm::vec3 displacement(0.0f);

        for (const auto& mesh : meshes) {
            const BoundingBox& meshBox = mesh->getBoundingBox();
            for (const auto& otherMesh : other->meshes) {
                const BoundingBox& otherMeshBox = otherMesh->getBoundingBox();
                if (checkBoundingBoxCollision(meshBox, otherMeshBox)) {
                    glm::vec3 direction = glm::normalize(meshBox.center() - otherMeshBox.center());
                    float magnitude = (meshBox.radius() + otherMeshBox.radius()) - glm::distance(meshBox.center(), otherMeshBox.center());
                    displacement += direction * magnitude;
                }
            }
        }

        return displacement;
    }
};

#endif