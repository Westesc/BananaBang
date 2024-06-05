#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <yaml-cpp/yaml.h>
#include "Transform.h"
#include "Model.h"
#include "UI.h"
#include "AnimateBody.h"

class GameObject {
public:
	GameObject(std::string Name, std::string Tag = "none", int Layer = 0);
	GameObject(YAML::Node node);
	virtual ~GameObject();

	AnimateBody* animPlayer;
	std::string name;
	bool active;
	std::string tag;
	int layer;
	GameObject* parent;
	std::vector<GameObject*> children;
	std::vector<Component> components;
	Transform* localTransform;
	std::shared_ptr<Model> modelComponent;
	float deltaTime;
	UI* uiComponent = nullptr;
	bool isRotating;
	float rotateSpeed;
	glm::vec3 rotateAxis;
	bool isVisible = true;
	BoundingBox* boundingBox = nullptr;
	CapsuleCollider* capsuleCollider = nullptr;
	glm::vec3 velocity = glm::vec3(0.f);
	float inverseMass = 1.f;
	float dampingFactor = 1.f;
	glm::vec3 colliderOffset = glm::vec3(0.f);
	bool friction = true;

	void setParent(GameObject* Parent);
	void addChild(GameObject* Child);
	void removeChild(GameObject* Child);
	Transform* getTransform();
	Component* getComponent();
	Component* getComponentInChildren();
	Component* getComponentInParent();
	void addModelComponent(std::shared_ptr<Model> model);
	void addAnimation(char* path,std::string name, float speed);
	AnimateBody* getAnimateBody() { return animPlayer; }
	std::shared_ptr<Model> getModelComponent() const;
	Transform* getGlobalTransform();
	void Move(glm::vec3 translation);
	void Update(glm::mat4 view, glm::mat4 perspective, float time);
	void setRotating(bool rotating, float speed = 100.f, glm::vec3 rotateAxis = glm::vec3(0.f, 0.f, 1.f));
	//void checkResolveCollisions(GameObject* other, float deltaTime);

	void Draw(glm::mat4 view, glm::mat4 perspective);
	void Draw(Shader* shader, Shader* animationShader);
	YAML::Node serialize();
	void setVisible(bool visible);
	void lightSetting(glm::vec3 viewPos, glm::vec3 lightPos, glm::vec3 lightColor);
	void addColider(int type);
	void predictPosition(float deltaTime);
	void updateVelocity(float deltaTime);
	void calculateOffset();
	void updatePredictedPosition();
	void shadowSetting(glm::mat4 LSMatrix);
	void timeSetting(float time, glm::vec2 iResolution);
	void addColider();
	template <typename T>
	T* getAsActualType() {
		return dynamic_cast<T*>(this);
	}
};

#endif