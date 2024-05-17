#ifndef GameObject_H
#define GameObject_H

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>
#include "Transform.h"
#include "Component.h"
#include "Model.h"

class GameObject {
public:
	GameObject(std::string Name, std::string Tag = "none", int Layer = 0);
	GameObject(YAML::Node node);
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
	float rotateSpeed;
	glm::vec3 rotateAxis;
	bool isVisible = true;
	BoundingBox* boundingBox = nullptr;

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
	void setRotating(bool rotating, float speed = 100.f, glm::vec3 rotateAxis = glm::vec3(0.f, 0.f, 1.f));
	void checkResolveCollisions(GameObject* other, float deltaTime);
	void Draw(glm::mat4 view, glm::mat4 perspective);
	YAML::Node serialize();
	void setVisible(bool visible);
	void lightSetting(glm::vec3 viewPos, glm::vec3 lightPos, glm::vec3 lightColor);
	void addColider();
};

#endif