#include "../lib/SceneManager.h"
SceneManager::SceneManager() : activeScene(nullptr) {}

SceneManager::~SceneManager() {}

void SceneManager::loadScene(std::string name)
{
	YAML::Node save = YAML::LoadFile("res/saves/" + name + ".yaml");
	if (save["Scenes"])
	{
		YAML::Node scenesNode = save["Scenes"];
		for (auto sc : scenesNode) {
			scenes.push_back(new Scene(sc));
		}
	}
}

void SceneManager::saveScene(std::string name)
{
	YAML::Node save;
	for (auto sc : scenes) {
		save["Scenes"].push_back(sc->serialize());
	}
	std::ofstream file("res/saves/" + name + ".yaml");
	if (file.is_open())
	{
		file << save;
		file.close();
	}
}

Scene* SceneManager::getActiveScene()
{
	return activeScene;
}
