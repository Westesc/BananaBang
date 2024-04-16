#ifndef GameObject_H
#define GameObject_H

#include <string>
#include <vector>
#include "Transform.h"
#include "Component.h"
#include "Model.h"

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
	Model* modelComponent;
	bool isRotating;

	void setParent(GameObject* Parent);
	void addChild(GameObject* Child);
	void removeChild(GameObject* Child);
	Transform* getTransform();
	Component* getComponent();
	Component* getComponentInChildren();
	Component* getComponentInParent();
	void addModelComponent(Model* model);
	Model* getModelComponent() const;
	Transform* getGlobalTransform();
	void Move(glm::vec3 translation);
	void Update(glm::mat4 view, glm::mat4 perspective, float time);
	void setRotating(bool rotating);
	void checkResolveCollisions(GameObject* other);
	void Draw(Shader* shaders, glm::mat4 view, glm::mat4 perspective);
};

#endif