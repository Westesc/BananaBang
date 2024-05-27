#ifndef Scene_H
#define Scene_H

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>
#include "GameObject.h"
#include "../thirdparty/tracy/public/tracy/Tracy.hpp"

class Scene {
public:
	Scene(std::string Name);
	Scene(YAML::Node Node);
	~Scene();

	std::string name;
	std::vector<GameObject*> gameObjects;
	void addObject(GameObject* go);
	GameObject* findByName(std::string name);
	void Update(glm::mat4 view, glm::mat4 perspective, float time);
	//void checkResolveCollisions(float deltaTime);
	void Draw(glm::mat4 view, glm::mat4 perspective);
	void Draw(Shader* shader);
	YAML::Node serialize();

	void lightSetting(glm::vec3 viewPos, glm::vec3 lightPos, glm::vec3 lightColor);
	void shadowSetting(glm::mat4 LSMatrix);

protected:
	void Start();
	
};

#endif
