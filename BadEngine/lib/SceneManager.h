#ifndef SceneManager_H
#define SceneManager_H

#include <string>
#include <vector>
#include "Scene.h"

class SceneManager {
public:
	SceneManager();
	~SceneManager();

	std::vector<Scene*> scenes;
	Scene* activeScene;

	void loadScene(std::string name);
	Scene* getActiveScene();
};

#endif