#include "../lib/AnimateBody.h"
#include "../lib/animation/Animation.h"

AnimateBody::AnimateBody(Model* m){
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

glm::vec3 AnimateBody::getPosition() {

	return animator->getposition();
}

void AnimateBody::UpdateAnimation(float deltaTime) {
	Animacje anim = searchActiveAnimation();
	animator->UpdateAnimation(deltaTime*anim.speedAnimation);
	model->GetShader()->use();
	auto transforms = animator->GetFinalBoneMatrices();
	for (int i = 0; i < transforms.size(); ++i)
		model->GetShader()->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
}

void AnimateBody::setActiveAnimation(std::string name) {
	if (activeAnimation != name) {
		activeAnimation = name;
		Animacje anim = searchActiveAnimation();
		animator->PlayAnimation(anim.animation);
	}
}

void AnimateBody::play()
{
}

bool AnimateBody::isPlay()
{
	return false;
}

void AnimateBody::reset()
{
}
