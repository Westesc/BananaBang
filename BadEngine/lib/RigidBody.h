#ifndef RigidBody_H
#define RigidBody_H

#include "Component.h"
#include <string>
#include <glm/ext/vector_float3.hpp>
#include "TimeManager.h"
#include "SceneManager.h"

class RigidBody : Component {
public:
	std::string tag;
	SceneManager* sm;
	TimeManager* tm;

	RigidBody(std::string tag, SceneManager* sm, TimeManager* tm);
	~RigidBody();

	float gravity = -0.16f;
	float mass;
	float upwardsSpeed = 0.f;

	void calculateGravity(float upwardSpeed, float limitJump);
	void useGravity();
	bool checkCollision();
};

#endif
