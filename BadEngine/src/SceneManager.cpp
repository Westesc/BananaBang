#include "../lib/SceneManager.h"
SceneManager::SceneManager() : activeScene(nullptr) {}

SceneManager::~SceneManager() {}

void SceneManager::loadScene(std::string name)
{

}

Scene* SceneManager::getActiveScene()
{
	return activeScene;
}
