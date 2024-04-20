#include "../lib/SceneManager.h"
SceneManager::SceneManager() : activeScene(nullptr) {}

SceneManager::~SceneManager() {}

void SceneManager::loadScene(std::string name)
{
	YAML::Node save = YAML::LoadFile("../../../../saves/" + name + ".yaml");
}

void SceneManager::saveScene(std::string name)
{
	YAML::Node save;
	for (auto sc : scenes) {
		save["Scenes"].push_back(sc->serialize());
	}
	std::ofstream file("../../../../saves/" + name + ".yaml");
	if (file.is_open())
	{
		file << save;
		printf("wyswietla sie");
		file.close();
	}
}

Scene* SceneManager::getActiveScene()
{
	return activeScene;
}
