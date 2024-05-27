#ifndef MODEL_H
#define MODEL_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <glm/gtx/string_cast.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <yaml-cpp/yaml.h>
#include "Mesh.h"
#include "animation/AssimpGLMHelpers.h"
#include "animation/Animdata.h"


class Model
{
private:
    glm::mat4* Transform;
    glm::mat4* prevTransform = new glm::mat4(1.f);
    std::vector<Texture> textures_loaded;
    std::vector<Mesh*> meshes;
    std::string directory;
    Shader* shader;
    Shader* outlineShader = nullptr;
    Shader* fillingShader = nullptr;
    bool isFromFile;
    char* pathObj;
    bool gammaCorrection;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;

    float x;
    float y;
    float z;
   
    void loadModel(std::string path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

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
        // data to fill
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        return new Mesh(vertices, indices, textures);
    }

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
    {
             std::vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if (!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str());
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
            }
        }
        return textures;
    }
    //funkcje do animacji
    void loadModelAnim(std::string path)
    {

        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }

        directory = path.substr(0, path.find_last_of('/'));

        processNodeAnim(scene->mRootNode, scene);
    }


    void processNodeAnim(aiNode* node, const aiScene* scene)
    {

        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMeshAnim(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNodeAnim(node->mChildren[i], scene);
        }

    }

    void SetVertexBoneDataToDefault(Vertex& vertex)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            vertex.m_BoneIDs[i] = -1;
            vertex.m_Weights[i] = 0.0f;
        }
    }


    Mesh* processMeshAnim(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            SetVertexBoneDataToDefault(vertex);
            vertex.Position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);
            vertex.Normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);

            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        ExtractBoneWeightForVertices(vertices, mesh, scene);

        return new Mesh(vertices, indices, textures);
    }

    void SetVertexBoneData(Vertex& vertex, int boneID, float weight)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            if (vertex.m_BoneIDs[i] < 0)
            {
                vertex.m_Weights[i] = weight;
                vertex.m_BoneIDs[i] = boneID;
                break;
            }
        }
    }


    void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
    {
        auto& boneInfoMap = m_BoneInfoMap;
        int& boneCount = m_BoneCounter;

        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                BoneInfo newBoneInfo;
                newBoneInfo.id = boneCount;
                newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
                boneInfoMap[boneName] = newBoneInfo;
                boneID = boneCount;
                boneCount++;
            }
            else
            {
                boneID = boneInfoMap[boneName].id;
            }
            assert(boneID != -1);
            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                assert(vertexId <= vertices.size());
                SetVertexBoneData(vertices[vertexId], boneID, weight);
            }
        }
    }
public:
    bool rotating;
    bool isBlue, iswhite;

    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCount() { return m_BoneCounter; }

    void setTransform(const glm::mat4& matrix) {
        *Transform = matrix;
    }
    void setPrevTransform(const glm::mat4& matrix) {
        *prevTransform = matrix;
    }
    glm::mat4* getTransform() { return Transform; }
    glm::mat4* getPrevTransform() { return prevTransform; }
    void SetShader(Shader* s) { shader = s; }
    void SetOutlineShader(Shader* s) { outlineShader = s; }
    void SetFillingShader(Shader* s) { fillingShader = s; }
    Shader* GetShader() { return shader; }
    Shader* GetOutlineShader() { return outlineShader; }
    Shader* GetFillingShader() { return fillingShader; }
    BoundingBox* boundingBox = nullptr;
    CapsuleCollider* capsuleCollider = nullptr;
    void AddTexture(std::string const& filename, std::string typeName)
    {
        Texture texture;
        texture.id = TextureFromFile(filename);
        texture.type = typeName;
        texture.path = filename;
        textures_loaded.push_back(texture);
    }

    void AddTexture(unsigned int id, std::string typeName, std::string filename = "")
    {

        for (auto tex : textures_loaded) {
            if (tex.type == typeName) {
                tex.id = id;
            }
        }
        Texture texture;
        texture.id = id;
        texture.type = typeName;
        texture.path = filename;
        textures_loaded.push_back(texture);
    }

    Model(char* path, bool isAnim, bool rotate = true, bool gamma = false) : gammaCorrection(gamma)
    {
        if (isAnim) {
            rotating = rotate;
            isFromFile = true;
            loadModelAnim(path);
            pathObj = path;
            Transform = new glm::mat4(1);
        }
        else {
            rotating = rotate;
            isFromFile = true;
            loadModel(path);
            pathObj = path;
            Transform = new glm::mat4(1);
        }
    }

    Model(Mesh* mesh, bool isBlue = false, bool rotate = true)
    {
        this->isBlue = isBlue;
        rotating = rotate;
        isFromFile = false;
        meshes.push_back(mesh);
        Transform = new glm::mat4(1);
    }

    unsigned int TextureFromFile(std::string const& filename, bool gamma = false)
    {
        //std::string filename = std::string(path);
        //filename = directory + '/' + filename;

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
            std::cout << "Texture failed to load at path: " << filename << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

    Model(YAML::Node node) {
        shader = new Shader(node["shader"]);
        if (node["path"]) {
            isFromFile = true;
            this->pathObj = strdup(node["path"].as<std::string>().c_str());
            loadModel(pathObj);
        }
        else if (node["meshes"]) {
            YAML::Node meshesNodes = node["meshes"];
            isFromFile = false;
            for (auto mn : meshesNodes) {
                meshes.push_back(new Mesh(mn));
            }

        }
        Transform = new glm::mat4(1);
        if (node["capsuleCollider"]) {
            capsuleCollider = new CapsuleCollider(node["capsuleCollider"]);
        }
        if (node["boundingBox"]) {
            boundingBox = new BoundingBox(node["boundingBox"]);
        }


        if (node["textures"])
        {
            YAML::Node  texturesNode = node["textures"];
            for (auto tex : texturesNode) {
                AddTexture(tex["path"].as<std::string>(),tex["type"].as<std::string>());
            }
        }
    }

    void Draw()
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
        {
            meshes[i]->Draw(shader, textures_loaded, Transform, isFromFile, rotating, isBlue);
        }
    }

    void Draw(Shader* shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
        {
            meshes[i]->Draw(shader, textures_loaded, Transform, isFromFile, rotating, isBlue);
        }
    }

    void DrawBoundingBox(const BoundingBox& bbox) {
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
        updateBoundingBox(transformation);
        DrawBoundingBox(*boundingBox);
        /*for (const auto& mesh : meshes) {
            mesh->updateBoundingBox(transformation);
            DrawBoundingBox(mesh->getBoundingBox(), shader, Transform);
        }*/
    }

    void updateBoundingBox(glm::mat4& modelMatrix) {
        glm::vec3 transformedMin = glm::vec3(modelMatrix * glm::vec4(boundingBox->min, 1.0f));
        glm::vec3 transformedMax = glm::vec3(modelMatrix * glm::vec4(boundingBox->max, 1.0f));
        boundingBox->min = transformedMin;
        boundingBox->max = transformedMax;
        /*if (boundingBox->customSize) {
            glm::vec3 transformedMin = glm::vec3(modelMatrix * glm::vec4(boundingBox->min, 1.0f));
            glm::vec3 transformedMax = glm::vec3(modelMatrix * glm::vec4(boundingBox->max, 1.0f));
            boundingBox->min = transformedMin;
            boundingBox->max = transformedMax;
        }
        else {
            glm::vec3 min(FLT_MAX);
            glm::vec3 max(-FLT_MAX);
            for (const auto& mesh : meshes) {
                for (const auto& vertex : mesh->vertices) {
                    glm::vec4 transformedVertex = modelMatrix * glm::vec4(vertex.Position, 1.0f);

                    min = glm::min(min, glm::vec3(transformedVertex));
                    max = glm::max(max, glm::vec3(transformedVertex));
                }
            }

            boundingBox = new BoundingBox(min, max);
        }
        */
    }

    void UpdateCollider(glm::mat4 transformation) {
        glm::vec3 transformedCenter = glm::vec3(transformation * glm::vec4(capsuleCollider->center, 1.0f));
        capsuleCollider->center = transformedCenter;
        /*for (const auto& mesh : meshes) {
            mesh->updateCapsuleCollider(transformation);
        }*/
    }

    void setCustomBox(const glm::vec3& customMin, const glm::vec3& customMax) {
        for (const auto& mesh : meshes) {
            mesh->setBoundingBox(customMin, customMax);
        }
    }

    /*void setCustomCapsule(float radius, float height, glm::vec3 position) {
        for (const auto& mesh : meshes) {
            mesh->setCapsuleCollider(radius, height, position);
        }
    }*/

    void addCollider(int collider, glm::vec3 position, float mass) {
        if (collider == 0) {
            boundingBox = nullptr;
            capsuleCollider = nullptr;
            /*for (const auto& mesh : meshes) {
                mesh->boundingBox = nullptr;
                mesh->capsuleCollider = nullptr;
            }*/
        }
        else if (collider == 1) {
            if (boundingBox == nullptr) {
                calculateBoundingBox(mass);
            }
            /*for (const auto& mesh : meshes) {
                if (mesh->boundingBox == nullptr) {
                    mesh->calculateBoundingBox();
                }
            }*/
        }
        else if (collider == 2) {
            if (capsuleCollider == nullptr) {
                calculateBoundingCapsule(position,mass);
            }
            /*for (const auto& mesh : meshes) {
                if (mesh->capsuleCollider == nullptr) {
                    mesh->calculateBoundingCapsule(position);
                }
            }*/
        }
    }

    void calculateBoundingBox(float mass) {
        glm::vec3 min(FLT_MAX);
        glm::vec3 max(-FLT_MAX);
        for (const auto& mesh : meshes) {
            for (const auto& vertex : mesh->vertices) {
                min = glm::min(min, vertex.Position);
                max = glm::max(max, vertex.Position);
            }
        }

        boundingBox = new BoundingBox(min, max, mass);
    }

    void calculateBoundingCapsule(glm::vec3 position,float mass) {
        float minY = FLT_MAX;
        float maxY = -FLT_MAX;
        float maxRadius = 0.0f;
        for (const auto& mesh : meshes) {
            for (const auto& vertex : mesh->vertices) {
                float y = vertex.Position.y;
                minY = std::min(minY, y);
                maxY = std::max(maxY, y);
                maxRadius = std::max(maxRadius, glm::length(glm::vec2(vertex.Position.x, vertex.Position.z)));
            }
        }
        float height = maxY - minY;
        capsuleCollider = new CapsuleCollider(glm::vec3(position.x, (minY + maxY) * 0.5f * 0.1f, position.z), maxRadius * 0.1f, height * 0.1f, mass);
    }

    bool checkBoundingBoxCollision(const BoundingBox& box1, const BoundingBox& box2, const glm::mat4& transform1, const glm::mat4& transform2) {
        glm::vec3 vertices1[] = {
            glm::vec3(transform1 * glm::vec4(box1.vertices.at(0), 1.0f)),
            glm::vec3(transform1 * glm::vec4(box1.vertices.at(1), 1.0f)),
            glm::vec3(transform1 * glm::vec4(box1.vertices.at(2), 1.0f)),
            glm::vec3(transform1 * glm::vec4(box1.vertices.at(3), 1.0f)),
            glm::vec3(transform1 * glm::vec4(box1.vertices.at(4), 1.0f)),
            glm::vec3(transform1 * glm::vec4(box1.vertices.at(5), 1.0f)),
            glm::vec3(transform1 * glm::vec4(box1.vertices.at(6), 1.0f)),
            glm::vec3(transform1 * glm::vec4(box1.vertices.at(7), 1.0f))
        };

        glm::vec3 vertices2[] = {
            glm::vec3(transform2 * glm::vec4(box2.vertices.at(0), 1.0f)),
            glm::vec3(transform2 * glm::vec4(box2.vertices.at(1), 1.0f)),
            glm::vec3(transform2 * glm::vec4(box2.vertices.at(2), 1.0f)),
            glm::vec3(transform2 * glm::vec4(box2.vertices.at(3), 1.0f)),
            glm::vec3(transform2 * glm::vec4(box2.vertices.at(4), 1.0f)),
            glm::vec3(transform2 * glm::vec4(box2.vertices.at(5), 1.0f)),
            glm::vec3(transform2 * glm::vec4(box2.vertices.at(6), 1.0f)),
            glm::vec3(transform2 * glm::vec4(box2.vertices.at(7), 1.0f))
        };
        for (int i = 0; i < 8; i++) {
            if (vertices1[i].x >= vertices2[0].x && vertices1[i].x <= vertices2[1].x &&
                vertices1[i].y >= vertices2[0].y && vertices1[i].y <= vertices2[1].y &&
                vertices1[i].z >= vertices2[0].z && vertices1[i].z <= vertices2[1].z) {
                return true;
            }
        }
        for (int i = 0; i < 8; i++) {
            if (vertices2[i].x >= vertices1[0].x && vertices2[i].x <= vertices1[1].x &&
                vertices2[i].y >= vertices1[0].y && vertices2[i].y <= vertices1[1].y &&
                vertices2[i].z >= vertices1[0].z && vertices2[i].z <= vertices1[1].z) {
                return true;
            }
        }
        return false;
    }


    bool checkBoxCapsuleCollision(const BoundingBox& meshBox, const CapsuleCollider& otherCapsule, glm::mat4 transform, glm::mat4 boxtransform) {
        glm::vec3 boxMin = glm::vec3(boxtransform * glm::vec4(meshBox.min, 1.0f));
        glm::vec3 boxMax = glm::vec3(boxtransform * glm::vec4(meshBox.max, 1.0f));
        glm::vec3 capsuleCenter = otherCapsule.center;
        float capsuleRadius = otherCapsule.radius;
        float capsuleHeight = otherCapsule.height;

        glm::vec3 transformedCapsuleCenter = glm::vec3(transform * glm::vec4(capsuleCenter, 1.0f));
        bool insideBox = transformedCapsuleCenter.x >= boxMin.x && transformedCapsuleCenter.x <= boxMax.x &&
            transformedCapsuleCenter.y >= boxMin.y && transformedCapsuleCenter.y <= boxMax.y &&
            transformedCapsuleCenter.z >= boxMin.z && transformedCapsuleCenter.z <= boxMax.z;
        float distanceToBox = 0.0f;
        if (!insideBox) {
            if (transformedCapsuleCenter.x < boxMin.x)
                distanceToBox += (transformedCapsuleCenter.x - boxMin.x) * (transformedCapsuleCenter.x - boxMin.x);
            else if (transformedCapsuleCenter.x > boxMax.x)
                distanceToBox += (transformedCapsuleCenter.x - boxMax.x) * (transformedCapsuleCenter.x - boxMax.x);

            if (transformedCapsuleCenter.y < boxMin.y)
                distanceToBox += (transformedCapsuleCenter.y - boxMin.y) * (transformedCapsuleCenter.y - boxMin.y);
            else if (transformedCapsuleCenter.y > boxMax.y)
                distanceToBox += (transformedCapsuleCenter.y - boxMax.y) * (transformedCapsuleCenter.y - boxMax.y);

            if (transformedCapsuleCenter.z < boxMin.z)
                distanceToBox += (transformedCapsuleCenter.z - boxMin.z) * (transformedCapsuleCenter.z - boxMin.z);
            else if (transformedCapsuleCenter.z > boxMax.z)
                distanceToBox += (transformedCapsuleCenter.z - boxMax.z) * (transformedCapsuleCenter.z - boxMax.z);
        }
        float minDistance = capsuleRadius;
        return insideBox || distanceToBox <= minDistance * minDistance;
    }

    bool checkCapsuleCollision(const CapsuleCollider& capsule1, const CapsuleCollider& capsule2, glm::mat4 transform1, glm::mat4 transform2) {
        glm::vec3 center1 = capsule1.center;
        glm::vec3 center2 = capsule2.center;
        float radius1 = capsule1.radius;
        float radius2 = capsule2.radius;
        float height1 = capsule1.height;
        float height2 = capsule2.height;
        glm::vec3 transformedCenter1 = glm::vec3(transform1 * glm::vec4(center1, 1.0f));
        glm::vec3 transformedCenter2 = glm::vec3(transform2 * glm::vec4(center2, 1.0f));

        float distanceSquared = glm::length(transformedCenter1 - transformedCenter2);
        distanceSquared *= distanceSquared;
        float sumRadius = radius1 + radius2;
        if (distanceSquared > sumRadius * sumRadius) {
            return false;
        }
        float distanceY = std::abs(transformedCenter1.y - transformedCenter2.y) - (height1 + height2) / 2.0f;
        return distanceY <= sumRadius;
    }

    bool checkCollision(Model* other) {
        if (boundingBox != nullptr) {
            if (other->boundingBox != nullptr) {
                return checkBoundingBoxCollision(*boundingBox, *other->boundingBox, *Transform, *other->getTransform());
            }
            else if (other->capsuleCollider != nullptr) {
                return checkBoxCapsuleCollision(*boundingBox, *other->capsuleCollider, *other->getTransform(), *Transform);
            }
        }
        else if (capsuleCollider != nullptr) {
            if (other->boundingBox != nullptr) {
                return checkBoxCapsuleCollision(*other->boundingBox, *capsuleCollider, *getTransform(), *other->getTransform());
            }
            else if (other->capsuleCollider != nullptr) {
                return checkCapsuleCollision(*capsuleCollider, *other->capsuleCollider, *getTransform(), *other->getTransform());
            }
        }

        return false;
    }

    std::vector<BoundingBox> getBoundingBoxes() {
        std::vector<BoundingBox> boxes;
        for (const auto& mesh : meshes) {
            boxes.push_back(mesh->getBoundingBox());
        }
        return boxes;
    }

    glm::vec3 calculateCollisionResponse(Model* other) {
        glm::vec3 displacement(0.0f);
        if (boundingBox != nullptr) {
            if (other->boundingBox != nullptr) {
                glm::vec3 direction = glm::normalize(glm::vec3(*Transform * glm::vec4(boundingBox->center(), 1.0f)) - glm::vec3(*other->getTransform() * glm::vec4(other->boundingBox->center(), 1.0f)));
                float magnitude = (boundingBox->radius() + other->boundingBox->radius()) - glm::distance(glm::vec3(*Transform * glm::vec4(boundingBox->center(), 1.0f)), glm::vec3(*other->getTransform() * glm::vec4(other->boundingBox->center(), 1.0f)));
                displacement += direction * magnitude;
                std::cout << "2box" << glm::to_string(displacement) << std::endl;
            }
            else if (other->capsuleCollider != nullptr) {
                glm::vec3 direction = glm::normalize(glm::vec3(*Transform * glm::vec4(boundingBox->center(), 1.0f)) - other->capsuleCollider->center);
                float magnitude = (boundingBox->radius() + other->capsuleCollider->radius) - glm::distance(glm::vec3(*Transform * glm::vec4(boundingBox->center(), 1.0f)), other->capsuleCollider->center);
                displacement += direction * magnitude;
                std::cout << "boxcapsule" << glm::to_string(displacement) << std::endl;
            }
        }
        else if (capsuleCollider != nullptr) {
            if (other->boundingBox != nullptr) {
                glm::vec3 direction = glm::normalize(capsuleCollider->center - glm::vec3(*other->getTransform() * glm::vec4(other->boundingBox->center(), 1.0f)));
                float magnitude = (capsuleCollider->radius + other->boundingBox->radius()) - glm::distance(capsuleCollider->center, glm::vec3(*other->getTransform() * glm::vec4(other->boundingBox->center(), 1.0f)));
                displacement += direction * magnitude;
                std::cout << "capsulebox" << glm::to_string(displacement) << std::endl;
            }
            else if (other->capsuleCollider != nullptr) {
                glm::vec3 direction = glm::normalize(capsuleCollider->center - other->capsuleCollider->center);
                float magnitude = (capsuleCollider->radius + other->capsuleCollider->radius) - glm::distance(capsuleCollider->center, other->capsuleCollider->center);
                displacement += direction * magnitude;
                std::cout << "2capsule" << glm::to_string(displacement) << std::endl;
            }
        }
        return displacement;
    }

    YAML::Node serialize()
    {

        YAML::Node node;
        if (isFromFile) {
            node["path"] = pathObj;
        }
        else
        {
            for (auto m : meshes)
            {
                node["meshes"].push_back(m->serialize());
            }
        }
        for (auto tex : textures_loaded) {
            YAML::Node textureNode;
            textureNode["path"] = tex.path;
            textureNode["type"] = tex.type;
            node["textures"].push_back(textureNode);
        }
        if (boundingBox != nullptr) {
            node["boundingBox"] = boundingBox->serialize();
        }
        else if (capsuleCollider != nullptr) {
            node["capsuleCollider"] = capsuleCollider->serialize();
        }
        node["shader"] = shader->serialize();
        return node;
    }
    
};

#endif