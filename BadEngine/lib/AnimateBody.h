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

	//float time;
	//int Animation; //temp

	void changeModel(Model* m);
	void addAnimation(char* path, std::string nameAnim);
	void UpdateAnimation(float deltaTime);
	void setActiveAnimation(std::string name);

	void play();
	bool isPlay();
	void reset();
};

#endif
