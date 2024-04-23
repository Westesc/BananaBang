#ifndef Scene_H
#define Scene_H

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>
#include "GameObject.h"

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
	YAML::Node serialize();
	
protected:
	void Start();
	
};

#endif
