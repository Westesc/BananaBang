#ifndef GameObject_H
#define GameObject_H

#include <string>
#include <vector>
#include "Transform.h"
#include "Component.h"

class GameObject {
public:
	GameObject(std::string Name, std::string Tag = "none", int Layer = 0);
	~GameObject();

	std::string name;
	bool active;
	std::string tag;
	int layer;
	GameObject* parent;
	std::vector<GameObject*> children;
	Transform* localTransform;

	void setParent(GameObject* Parent);
	void addChild(GameObject* Child);
	void removeChild(GameObject* Child);
	Transform* getTransform();
	Component* getComponent();
	Component* getComponentInChildren();
	Component* getComponentInParent();
};

#endif