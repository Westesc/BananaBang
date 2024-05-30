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
        if (m_CurrentAnimation)
        {
            m_DeltaTime = dt;
            m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;

            if (m_CurrentTime >= m_CurrentAnimation->GetDuration()) {
                m_Playing = false;
                m_Finished = true;
            }

            if (m_Finished && playOnce) {
                m_CurrentTime = m_CurrentAnimation->GetDuration() - 0.0001f;
            }
            else {
                m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
            }
            CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));

            if (m_Blending)
            {
                m_BlendFactor += dt * m_BlendSpeed;
                if (m_BlendFactor >= 1.0f)
                {
                    m_BlendFactor = 1.0f;
                    m_Blending = false;
                }
                //BlendBoneMatrices();
            }
        }
    }

    bool changeAnim = false;
    void changeAnimation() {
        changeAnim = true;
    }

    void PlayAnimation(Animation* pAnimation, bool once)
    {
        m_PreviousBoneMatrices = m_FinalBoneMatrices;
        m_PreviousAnimation = m_CurrentAnimation;
        m_CurrentAnimation = pAnimation;
        m_PrevTime = m_CurrentTime;
        m_CurrentTime = 0.0f;
        m_BlendFactor = 0.1f;
        m_Blending = true;
        m_Playing = true;
        m_Finished = false;
        playOnce = once;
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

        Bone* bone = m_CurrentAnimation->FindBone(nodeName);
        Bone* BonePrev;
        if (m_PreviousAnimation != nullptr) {
           BonePrev = m_PreviousAnimation->FindBone(nodeName);
        }

        if (bone)
        {
            bone->Update(m_CurrentTime);
            nodeTransform = bone->GetLocalTransform();

            if (m_Blending) {
                prev_keyPositions = BonePrev->GetPosition();
                prev_keyRotations = BonePrev->GetRotation();
                prev_keyScales = BonePrev->GetScale();

                keyPositions = bone->GetPosition();
                keyRotations = bone->GetRotation();
                keyScales = bone->GetScale();
                nodeTransform = BlendBonesMatrices(bone->GetPositionIndex(m_CurrentTime), BonePrev->GetPositionIndex(m_PrevTime), bone->GetRotationIndex(m_CurrentTime), BonePrev->GetRotationIndex(m_PrevTime), bone->GetScaleIndex(m_CurrentTime), BonePrev->GetScaleIndex(m_PrevTime));
                
                //nodeTransform = BlendBonesMatrices();
            }
            //else {
            //    nodeTransform = ;
            //    keyPositions = Bone->GetPosition();
            //    keyRotations = Bone->GetRotation();
            //    keyScales = Bone->GetScale();
            //}
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
                if (changeAnim == true) {
                    deltaPostion = glm::vec3(0.f, 0.f, 0.f);
                    changeAnim = false;
                }
                m_FinalBoneMatrices[index][3][0] -= position.x;
                if (!isAnimY) {
                    m_FinalBoneMatrices[index][3][1] -= position.y;
                }
                else {
                    m_FinalBoneMatrices[index][3][1] -= 1.f;
                }
                m_FinalBoneMatrices[index][3][2] -= position.z;

                globalTransformation[3][0] -= position.x;
                if (!isAnimY) {
                    globalTransformation[3][1] -= position.y;
                }
                else {
                    globalTransformation[3][1] -= 1.f;
                }
                globalTransformation[3][2] -= position.z;
            }
  
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }

    void setIsAnimY(bool Y)
    {
        isAnimY = Y;
    }

    std::vector<glm::mat4> GetFinalBoneMatrices()
    {
        return m_FinalBoneMatrices;
    }

private:
    bool m_Playing = false;
    bool m_Finished = false;
    bool isAnimY = true;

    std::vector<KeyPosition> prev_keyPositions;
    std::vector<KeyScale> prev_keyScales;
    std::vector<KeyRotation> prev_keyRotations;

    std::vector<KeyPosition> keyPositions;
    std::vector<KeyScale> keyScales;
    std::vector<KeyRotation> keyRotations;

    glm::mat4 BlendBonesMatrices(int posIndex, int prevPosIndex, int rotIndex,int prevRotIndex , int scaleIndex, int prevScaleIndex) {
        glm::vec3 finalPosition = glm::mix(prev_keyPositions[prevPosIndex].position, keyPositions[posIndex].position
            , m_BlendFactor);
        glm::mat4 Position = glm::translate(glm::mat4(1.0f), finalPosition);

        glm::quat finalRotation = glm::slerp(prev_keyRotations[prevRotIndex].orientation, keyRotations[rotIndex].orientation
            , m_BlendFactor);
        finalRotation = glm::normalize(finalRotation);
        glm::mat4 Rotation = glm::toMat4(finalRotation);

        glm::vec3 finalScale = glm::mix(prev_keyScales[prevScaleIndex].scale, keyScales[scaleIndex].scale
            , m_BlendFactor);
        glm::mat4 Scale =  glm::scale(glm::mat4(1.0f), finalScale);

        return Position * Rotation * Scale;
    }

    void BlendBoneMatrices()
    {
        for (int i = 0; i < m_FinalBoneMatrices.size(); ++i)
        {
            for (int row = 0; row < 4; ++row)
            {
                for (int col = 0; col < 4; ++col)
                {
                   // m_FinalBoneMatrices[i][row][col] = glm::mix(m_PreviousBoneMatrices[i][row][col], m_FinalBoneMatrices[i][row][col], m_BlendFactor);
                    m_FinalBoneMatrices[i][row][col] = m_PreviousBoneMatrices[i][row][col] * (1 - m_BlendFactor) + m_FinalBoneMatrices[i][row][col] * m_BlendFactor;
                }
            }
        }
            /*
            float blendFactor = glm::clamp(m_BlendFactor, 0.0f, 1.0f);

            for (size_t i = 0; i < m_FinalBoneMatrices.size(); ++i)
            {
                // Interpolacja translacji
                glm::vec3 translationA = glm::vec3(m_PreviousBoneMatrices[i][3]);
                glm::vec3 translationB = glm::vec3(m_FinalBoneMatrices[i][3]);
                glm::vec3 blendedTranslation = glm::mix(translationA, translationB, blendFactor);

                // Ekstrakcja skalowania
                glm::vec3 scaleA(
                    glm::length(glm::vec3(m_PreviousBoneMatrices[i][0])),
                    glm::length(glm::vec3(m_PreviousBoneMatrices[i][1])),
                    glm::length(glm::vec3(m_PreviousBoneMatrices[i][2]))
                );
                glm::vec3 scaleB(
                    glm::length(glm::vec3(m_FinalBoneMatrices[i][0])),
                    glm::length(glm::vec3(m_FinalBoneMatrices[i][1])),
                    glm::length(glm::vec3(m_FinalBoneMatrices[i][2]))
                );
                glm::vec3 blendedScale = glm::mix(scaleA, scaleB, blendFactor);

                // Ekstrakcja i normalizacja rotacji
                glm::mat3 rotationA(
                    glm::vec3(m_PreviousBoneMatrices[i][0]) / (scaleA.x != 0 ? scaleA.x : 1.0f),
                    glm::vec3(m_PreviousBoneMatrices[i][1]) / (scaleA.y != 0 ? scaleA.y : 1.0f),
                    glm::vec3(m_PreviousBoneMatrices[i][2]) / (scaleA.z != 0 ? scaleA.z : 1.0f)
                );
                glm::mat3 rotationB(
                    glm::vec3(m_FinalBoneMatrices[i][0]) / (scaleB.x != 0 ? scaleB.x : 1.0f),
                    glm::vec3(m_FinalBoneMatrices[i][1]) / (scaleB.y != 0 ? scaleB.y : 1.0f),
                    glm::vec3(m_FinalBoneMatrices[i][2]) / (scaleB.z != 0 ? scaleB.z : 1.0f)
                );

                glm::quat rotQuatA = glm::normalize(glm::quat_cast(rotationA));
                glm::quat rotQuatB = glm::normalize(glm::quat_cast(rotationB));
                glm::quat blendedRotation = glm::slerp(rotQuatA, rotQuatB, blendFactor);

                // Rekonstrukcja macierzy
                glm::mat4 blendedMatrix = glm::translate(glm::mat4(1.0f), blendedTranslation) *
                    glm::mat4_cast(blendedRotation) *
                    glm::scale(glm::mat4(1.0f), blendedScale);

                m_FinalBoneMatrices[i] = blendedMatrix;*/
            
    }
    float pom = 0.f;
    std::vector<glm::mat4> m_FinalBoneMatrices;
    std::vector<glm::mat4> m_PreviousBoneMatrices;
    Animation* m_CurrentAnimation;
    Animation* m_PreviousAnimation = nullptr;
    float m_CurrentTime;
    float m_PrevTime = 0.f;
    float m_DeltaTime;
    float m_BlendFactor;
    bool m_Blending;
    const float m_BlendSpeed = 2.f;
    bool playOnce = false;
    glm::vec3 position;
    glm::vec3 lastPosition = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 deltaPostion = glm::vec3(0.f, 0.f, 0.f);
};
