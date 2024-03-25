#ifndef RigidBody_H
#define RigidBody_H

#include "Component.h"

class RigidBody : Component {
public:
	RigidBody();
	~RigidBody();

	float gravity;
	float mass;

	void useGravity();
	bool checkCollision();
};

#endif
