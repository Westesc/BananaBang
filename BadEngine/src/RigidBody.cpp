#include "../lib/RigidBody.h"


RigidBody::RigidBody(std::string tag, SceneManager* sm, TimeManager* tm)
{
	this->tag = tag;
	this->sm = sm;
	this->tm = tm;
}

RigidBody::~RigidBody()
{
}

void RigidBody::useGravity()
{
	upwardsSpeed += gravity * tm->getFramePerSeconds() / 4;
	sm->getActiveScene()->findByName(tag)->Move(glm::vec3(0.0f, upwardsSpeed * tm->getFramePerSeconds(), 0.0f));
}

bool RigidBody::checkCollision()
{
	return false;
}
