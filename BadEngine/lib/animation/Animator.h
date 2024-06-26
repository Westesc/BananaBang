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
        m_CurrentTimeArm = 0.0;
        m_CurrentTimeLeg = 0.0f;
        m_CurrentAnimationArms = animation;
        m_FinalBoneMatrices.reserve(100);
        for (int i = 0; i < 100; i++)
            m_FinalBoneMatrices.push_back(glm::mat4(1.0f));

        m_BlendFactor = 0.0f;
        m_Blending = false;

        m_LegBones = {
          "mixamorig_Hips",
          "mixamorig_RightUpLeg", "mixamorig_RightLeg", "mixamorig_RightFoot", "mixamorig_RightToeBase", "mixamorig_RightToe_End",
          "mixamorig_LeftUpLeg", "mixamorig_LeftLeg", "mixamorig_LeftFoot", "mixamorig_LeftToeBase", "mixamorig_LeftToe_End"
        };
    }

    void UpdateAnimation(float dt)
    {
        if (m_CurrentAnimationArms)
        {
            m_DeltaTime = dt;
            if (m_CurrentAnimationLeg) {
                m_CurrentTimeLeg += m_CurrentAnimationLeg->GetTicksPerSecond() * dt;
                m_CurrentTimeLeg = fmod(m_CurrentTimeLeg, m_CurrentAnimationLeg->GetDuration());
            }
            m_CurrentTimeArm += m_CurrentAnimationArms->GetTicksPerSecond() * dt;

            if (m_CurrentTimeArm >= m_CurrentAnimationArms->GetDuration()) {
                m_Playing = false;
                m_Finished = true;
            }

            if (m_Finished && playOnce) {
                m_CurrentTimeArm = m_CurrentAnimationArms->GetDuration() - 0.0001f;
            }
            else {
                m_CurrentTimeArm = fmod(m_CurrentTimeArm, m_CurrentAnimationArms->GetDuration());
            }
            CalculateBoneTransform(&m_CurrentAnimationArms->GetRootNode(), glm::mat4(1.0f));

            if (m_Blending)
            {
                m_BlendFactor += dt * m_BlendSpeed;
                if (m_BlendFactor >= 1.f)
                {
                    m_BlendFactor = 1.f;
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

    void addLegAnimation(Animation* sAnimation) {
        if (sAnimation != m_CurrentAnimationLeg) {
            m_PreviousAnimationLeg = m_CurrentAnimationLeg;
            m_CurrentAnimationLeg = sAnimation;
            m_PrevTimeLeg = m_CurrentTimeLeg;
            m_CurrentTimeLeg = 0.0f;
        }
    }

    void removeLegAnimation() {
        m_PreviousAnimationLeg = m_CurrentAnimationLeg;
        m_CurrentAnimationLeg = nullptr;
        m_PrevTimeLeg = m_CurrentTimeLeg;
        m_CurrentTimeLeg = 0.0f;
    }

    void PlayAnimation(Animation* pAnimation, bool once)
    {
        m_PreviousAnimationArms = m_CurrentAnimationArms;
        //m_PreviousAnimationLeg = m_CurrentAnimationLeg;
        m_CurrentAnimationArms = pAnimation;
        //m_CurrentAnimationLeg = nullptr;
        m_PrevTimeArm = m_CurrentTimeArm;
        //m_PrevTimeLeg = m_CurrentTimeLeg;
        m_CurrentTimeArm = 0.0f;
        //m_CurrentTimeLeg = 0.0f;
        m_BlendFactor = 0.0f;
        m_Blending = true;
        m_Playing = true;
        m_Finished = false;
        playOnce = once;
    }

    void PlayAnimation(Animation* pAnimation, Animation* sAnimation, bool once)
    {
        m_PreviousAnimationArms = m_CurrentAnimationArms;
        m_PreviousAnimationLeg = m_CurrentAnimationLeg;
        m_CurrentAnimationArms = pAnimation;
        m_CurrentAnimationLeg = sAnimation;
        m_PrevTimeArm = m_CurrentTimeArm;
        m_PrevTimeLeg = m_CurrentTimeLeg;
        m_CurrentTimeArm = 0.0f;
        m_CurrentTimeLeg = 0.0f;
        m_BlendFactor = 0.0f;
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
        m_CurrentTimeArm = 0.0f;
        m_Playing = false;
        m_Finished = false;
    }


    glm::mat4 GetBoneOffset(const std::string& boneName)
    {
        auto boneInfoMap = m_CurrentAnimationArms->GetBoneIDMap();
        if (boneInfoMap.find("mixamorig_Hips") != boneInfoMap.end())
        {
            return boneInfoMap[boneName].offset;
        }
        return glm::mat4(1.0f);
    }

    glm::mat4 GetRootNodeTransform() const
    {
        if (m_CurrentAnimationArms)
        {
            return m_CurrentAnimationArms->GetRootNode().transformation;
        }
        return glm::mat4(1.0f);
    }

    glm::vec3 getposition() {
        return deltaPostion;
    }

    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
    {
        Animation* m_CurrentAnimation = m_CurrentAnimationArms;
        Animation* m_PreviousAnimation = m_PreviousAnimationArms;
        float m_PrevTime = m_PrevTimeArm;
        std::string nodeName = node->name;

        //std::cout << nodeName << std::endl;

        float m_CurrentTime = m_CurrentTimeArm;

        glm::mat4 nodeTransform = node->transformation;


        if (std::find(m_LegBones.begin(), m_LegBones.end(), nodeName) != m_LegBones.end() && m_CurrentAnimationLeg) {
            m_CurrentAnimation = m_CurrentAnimationLeg;
            m_CurrentTime = m_CurrentTimeLeg;
            if (m_PreviousAnimationLeg) {
                m_PreviousAnimation = m_PreviousAnimationLeg;
                m_PrevTime = m_PrevTimeLeg;
            }
        }

        Bone* bone = m_CurrentAnimation->FindBone(nodeName);

        Bone* BonePrev;
        if (m_PreviousAnimation != nullptr) {
            BonePrev = m_PreviousAnimation->FindBone(nodeName);
        }

        /*  Bone* boneLeg;
          if (m_CurrentAnimationLeg != nullptr) {
              boneLeg = m_CurrentAnimationLeg->FindBone(nodeName);
              if (boneLeg) {
                  boneLeg->Update(m_CurrentTime);
                  if (std::find(m_LegBones.begin(), m_LegBones.end(), nodeName) != m_LegBones.end() && m_CurrentAnimationLeg) {
                      nodeTransform = boneLeg->GetLocalTransform();
                  }
              }
          }*/

        if (bone)
        {
            bone->Update(m_CurrentTime);
            nodeTransform = bone->GetLocalTransform();

            if (m_Blending && BonePrev) {
                if (nodeName != "mixamorig_Hips") {
                    bone->Update(m_CurrentTime, m_PrevTime, BonePrev, m_BlendFactor);
                    nodeTransform = bone->GetLocalTransform();
                }
                else {
                    bone->Update(m_CurrentTime, m_PrevTime, BonePrev, m_BlendFactor);
                    glm::mat4 temp = nodeTransform;
                    nodeTransform = bone->GetLocalTransform();
                    nodeTransform[3][0] = temp[3][0];
                    nodeTransform[3][1] = temp[3][1];
                    nodeTransform[3][2] = temp[3][2];
                }
            }
        }

        glm::mat4 globalTransformation = parentTransform * nodeTransform;
        auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();

        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int index = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = globalTransformation * offset;
            if (nodeName == "mixamorig_Hips") {
                //std::cout << "jestem tutaj";
                position.x = globalTransformation[3][0];
                position.y = globalTransformation[3][1];
                position.z = globalTransformation[3][2];
                //position /= 40.f;

                if (abs(position.x - lastPosition.x) < 0.01f) {
                    deltaPostion.x = position.x - lastPosition.x;
                }
                if (abs(position.y - lastPosition.y) < 0.01f) {
                    deltaPostion.y = position.y - lastPosition.y;
                }
                if (abs(position.z - lastPosition.z) < 0.01f) {
                    deltaPostion.z = position.z - lastPosition.z;
                }
               // std::cout << glm::to_string(deltaPostion) << std::endl;
                lastPosition = position;
                if (changeAnim == true) {
                    deltaPostion = glm::vec3(0.f, 0.f, 0.f);
                    changeAnim = false;
                }
                m_FinalBoneMatrices[index][3][0] -= position.x;
                if (!isAnimY) {
                    position.y -= 0.06f;
                    m_FinalBoneMatrices[index][3][1] -= position.y;
                }
                else {
                   // m_FinalBoneMatrices[index][3][1] -= 1.f;
                }
                m_FinalBoneMatrices[index][3][2] -= position.z;

                globalTransformation[3][0] -= position.x;
                if (!isAnimY) {
                    globalTransformation[3][1] -= position.y;
                }
                else {
                    //globalTransformation[3][1] -= 1.f;
                }
                globalTransformation[3][2] -= position.z;
            }

        }

        glm::mat4 oldTransorm = globalTransformation;
        for (int i = 0; i < node->childrenCount; i++)
        {
            if (nodeName == "mixamorig_Hips" && i == 0 && m_CurrentAnimationLeg)
            {
                if (bone) {
                    Bone* bone = m_CurrentAnimationArms->FindBone(nodeName);
                    bone->Update(m_CurrentTimeArm);
                    nodeTransform = bone->GetLocalTransform();
                    glm::mat4 temp = parentTransform * nodeTransform;
                    globalTransformation[0] = temp[0];
                    globalTransformation[1] = temp[1];
                    globalTransformation[2] = temp[2];
                }
            }
            else if (nodeName == "mixamorig_Hips") {
                globalTransformation = oldTransorm;
            }
            CalculateBoneTransform(&node->children[i], globalTransformation);
        }
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

    glm::mat4 BlendBonesMatrices(int posIndex, int prevPosIndex, int rotIndex, int prevRotIndex, int scaleIndex, int prevScaleIndex) {
        glm::vec3 finalPosition = glm::mix(prev_keyPositions[prevPosIndex].position, keyPositions[posIndex].position
            , m_BlendFactor);
        glm::mat4 Position = glm::translate(glm::mat4(1.0f), finalPosition);

        glm::quat finalRotation = glm::slerp(prev_keyRotations[prevRotIndex].orientation, keyRotations[rotIndex].orientation
            , m_BlendFactor);
        finalRotation = glm::normalize(finalRotation);
        glm::mat4 Rotation = glm::toMat4(finalRotation);

        glm::vec3 finalScale = glm::mix(prev_keyScales[prevScaleIndex].scale, keyScales[scaleIndex].scale
            , m_BlendFactor);
        glm::mat4 Scale = glm::scale(glm::mat4(1.0f), finalScale);

        return Position * Rotation * Scale;
    }

    float pom = 0.f;
    std::vector<std::string> m_LegBones;
    std::vector<glm::mat4> m_FinalBoneMatrices;
    Animation* m_CurrentAnimationArms = nullptr;
    Animation* m_CurrentAnimationLeg = nullptr;
    Animation* m_PreviousAnimationArms = nullptr;
    Animation* m_PreviousAnimationLeg = nullptr;
    float m_CurrentTimeArm;
    float m_CurrentTimeLeg;
    float m_PrevTimeArm = 0.f;
    float m_PrevTimeLeg = 0.f;
    float m_DeltaTime;
    float m_BlendFactor;
    bool m_Blending;
    const float m_BlendSpeed = 2.5f;
    bool playOnce = false;
    glm::vec3 position;
    glm::vec3 lastPosition = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 deltaPostion = glm::vec3(0.f, 0.f, 0.f);
};
