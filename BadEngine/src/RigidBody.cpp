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
	upwardsSpeed += gravity * tm->getFramePerSeconds();
	sm->getActiveScene()->findByName(tag)->velocity.y = upwardsSpeed * tm->getFramePerSeconds() / tm->getFramePerSeconds();
}

bool RigidBody::checkCollision()
{
	return false;
}
