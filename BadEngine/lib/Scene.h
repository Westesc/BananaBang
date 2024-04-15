#ifndef Scene_H
#define Scene_H

#include <string>
#include <vector>
#include "GameObject.h"

class Scene {
public:
	Scene(std::string Name);
	~Scene();

	std::string name;
	std::vector<GameObject*> gameObjects;

	void addObject(GameObject* go);
	GameObject* findByName(std::string name);

protected:
	void Start();
	void Update();
};

#endif
