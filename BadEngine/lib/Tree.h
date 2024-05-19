#ifndef Tree_H
#define Tree_H

#include "GameObject.h"

class Tree : public GameObject {
public:
	Tree(std::string name) : GameObject(name) {}
	~Tree() {}

	float health = 100.f;
};
#endif
