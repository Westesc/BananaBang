#ifndef AnimateBody_H
#define AnimateBody_H

#include "Component.h"
#include <vector>
#include "Model.h"
#include "animation/Animation.h"
#include "animation/Animator.h"

class AnimateBody : Component {
	Model* model = nullptr;
	struct Animacje {
		std::string name;
		Animation* animation;
		float speedAnimation;
	};

	Animator* animator;
	std::vector<Animacje> allAnimation;
	std::string activeAnimation;

	Animacje searchActiveAnimation() {
		for (auto& anim : allAnimation) {
			if (anim.name == activeAnimation) {
				return anim;
			}
		}
	}

public:
	AnimateBody(Model* m);
	~AnimateBody();

	void changeModel(Model* m);
	void addAnimation(char* path, std::string nameAnim, float time);
	void UpdateAnimation(float deltaTime);
	void UpdateAnimation(float deltaTime,Shader* shader);
	void setActiveAnimation(std::string name, bool once = false);
	glm::vec3 getPosition();
	std::string getActiveAnimation();
	void setActiveAnimationWithY(std::string name, bool playOnce = false);

	void play();
	bool isPlay();
	void reset();
};

#endif
