#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "Animation.h"
#include "Bone.h"

class Animator
{
public:
    Animator(Animation* animation)
    {
        m_CurrentTime = 0.0;
        m_CurrentAnimation = animation;
        m_FinalBoneMatrices.reserve(100);
        for (int i = 0; i < 100; i++)
            m_FinalBoneMatrices.push_back(glm::mat4(1.0f));

        m_BlendFactor = 0.0f;
        m_Blending = false;
    }

    void UpdateAnimation(float dt)
    {
        m_DeltaTime = dt;
        if (m_CurrentAnimation)
        {
            m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;

            if (m_CurrentTime >= m_CurrentAnimation->GetDuration()) {
                m_Playing = false;
                m_Finished = true;
            }


            m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
            CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));

            if (m_Blending)
            {
                m_BlendFactor += dt * m_BlendSpeed;
                if (m_BlendFactor >= 1.0f)
                {
                    m_BlendFactor = 1.0f;
                    m_Blending = false;
                }
                BlendBoneMatrices();
            }
        }
    }

    void PlayAnimation(Animation* pAnimation)
    {
        m_PreviousBoneMatrices = m_FinalBoneMatrices;
        m_CurrentAnimation = pAnimation;
        m_CurrentTime = 0.0f;
        m_BlendFactor = 0.8f;
        m_Blending = true;
        m_Playing = true;
        m_Finished = false;

    }

    bool IsPlaying() const {
        return m_Playing;
    }

    bool IsFinished() const {
        return m_Finished;
    }

    void Reset() {
        m_CurrentTime = 0.0f;
        m_Playing = false;
        m_Finished = false;
    }


    glm::mat4 GetBoneOffset(const std::string& boneName)
    {
        auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
        if (boneInfoMap.find("mixamorig_Hips") != boneInfoMap.end())
        {
            return boneInfoMap[boneName].offset;
        }
        return glm::mat4(1.0f);
    }

    glm::mat4 GetRootNodeTransform() const
    {
        if (m_CurrentAnimation)
        {
            return m_CurrentAnimation->GetRootNode().transformation;
        }
        return glm::mat4(1.0f);
    }

    glm::vec3 getposition() {
        return deltaPostion;
    }

    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
    {
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        Bone* Bone = m_CurrentAnimation->FindBone(nodeName);
        if (Bone)
        {
            Bone->Update(m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();
        }

        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int index = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = globalTransformation * offset;

            if (nodeName == "mixamorig_Hips") {
                position.x = globalTransformation[3][0];
                position.y = globalTransformation[3][1];
                position.z = globalTransformation[3][2];
                if (abs(position.x - lastPosition.x) < 0.5f) {
                    deltaPostion.x = position.x - lastPosition.x;
                }
                if (abs(position.y - lastPosition.y) < 0.5f) {
                    deltaPostion.y = position.y - lastPosition.y;
                }
                if (abs(position.z - lastPosition.z) < 0.5f) {
                    deltaPostion.z = position.z - lastPosition.z;
                }
                lastPosition = position;
            }
            m_FinalBoneMatrices[index][3][0] -= position.x;
            m_FinalBoneMatrices[index][3][1] -= position.y;
            m_FinalBoneMatrices[index][3][2] -= position.z;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }

    std::vector<glm::mat4> GetFinalBoneMatrices()
    {
        return m_FinalBoneMatrices;
    }

private:
    bool m_Playing = false;
    bool m_Finished = false;

    void BlendBoneMatrices()
    {
        for (int i = 0; i < m_FinalBoneMatrices.size(); ++i)
        {
            for (int row = 0; row < 4; ++row)
            {
                for (int col = 0; col < 4; ++col)
                {
                    m_FinalBoneMatrices[i][row][col] = glm::mix(m_PreviousBoneMatrices[i][row][col], m_FinalBoneMatrices[i][row][col], m_BlendFactor);
                }
            }
        }
    }

    std::vector<glm::mat4> m_FinalBoneMatrices;
    std::vector<glm::mat4> m_PreviousBoneMatrices;
    Animation* m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;
    float m_BlendFactor;
    bool m_Blending;
    const float m_BlendSpeed = 0.1f;
    glm::vec3 position;
    glm::vec3 lastPosition = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 deltaPostion = glm::vec3(0.f, 0.f, 0.f);
};
