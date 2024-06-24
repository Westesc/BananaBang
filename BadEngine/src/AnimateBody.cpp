#include "../lib/AnimateBody.h"
#include "../lib/animation/Animation.h"

AnimateBody::AnimateBody(Model* m) {
    model = m;
}

AnimateBody::~AnimateBody()
{
}

void AnimateBody::changeModel(Model* m) {
    *model = *m;
}

void AnimateBody::addAnimation(char* path, std::string nameAnim, float time)
{
    if (model != nullptr) {
        Animation* animationModel = new Animation(path, model);
        Animator* animatorModel = new Animator(animationModel);

        Animacje newAnimacje;
        newAnimacje.name = nameAnim;
        newAnimacje.animation = animationModel;
        newAnimacje.speedAnimation = time;
        animator = animatorModel;

        allAnimation.push_back(newAnimacje);
        activeAnimation = nameAnim;
    }
}

void AnimateBody::changeAnimationSpeed(const std::string& nameAnim, float newSpeed) {
    for (auto& anim : allAnimation) {
        if (anim.name == nameAnim) {
            anim.speedAnimation = newSpeed;
        }
    }
}


glm::vec3 AnimateBody::getPosition() {
    return animator->getposition();
}

void AnimateBody::UpdateAnimation(float deltaTime) {
    Animacje anim = searchActiveAnimation();
    animator->UpdateAnimation(deltaTime * anim.speedAnimation);
    model->GetShader()->use();
    auto transforms = animator->GetFinalBoneMatrices();
    for (int i = 0; i < transforms.size(); ++i) {
        model->GetShader()->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
    }
}

void AnimateBody::UpdateAnimation(float deltaTime, Shader* shader) {
    Animacje anim = searchActiveAnimation();
    //animator->UpdateAnimation(deltaTime * anim.speedAnimation);
    shader->use();
    auto transforms = animator->GetFinalBoneMatrices();
    for (int i = 0; i < transforms.size(); ++i)
        shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
}

std::string AnimateBody::getActiveAnimation() {
    return searchActiveAnimation().name;
}

void AnimateBody::setActiveAnimation(std::string name, bool playOnce) {
    if (activeAnimation != name) {
        activeAnimation = name;
        Animacje anim = searchActiveAnimation();
        if (activeLegAnimation == "walking") {
            Animacje* anim2 = findAnimationByName("walking");
            animator->PlayAnimation(anim.animation, anim2->animation, playOnce);
        }
        else {
            animator->PlayAnimation(anim.animation, playOnce);
        }
        animator->setIsAnimY(true);
        //animator->changeAnimation();
    }
}

void AnimateBody::addLegAnimation(std::string name) {
    if (activeLegAnimation != name) {
        activeLegAnimation = name;
        Animacje* anim = findAnimationByName(name);
        animator->addLegAnimation(anim->animation);
        //animator->changeAnimation();
    }
}

void AnimateBody::removeLegAnimation() {
    if (activeLegAnimation != " ") {
        activeLegAnimation = " ";
        //Animacje* anim = findAnimationByName(name);
        animator->removeLegAnimation();
        //animator->changeAnimation();
    }
}


void AnimateBody::setActiveAnimation(std::string name, std::string name2, bool playOnce) {
    if (activeAnimation != name) {
        activeAnimation = name;
        Animacje anim = searchActiveAnimation();
        Animacje* anim2 = findAnimationByName(name2);
        animator->PlayAnimation(anim.animation, anim2->animation, playOnce);
        animator->setIsAnimY(true);
        //animator->changeAnimation();
    }
}

void AnimateBody::setActiveAnimationWithY(std::string name, bool playOnce) {
    if (activeAnimation != name) {
        activeAnimation = name;
        Animacje anim = searchActiveAnimation();
        animator->PlayAnimation(anim.animation, playOnce);
        animator->setIsAnimY(false);
    }
}

void AnimateBody::play()
{
}

bool AnimateBody::isPlay()
{
    return animator->IsPlaying();
}

void AnimateBody::reset()
{
    animator->Reset();
}
