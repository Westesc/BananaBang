#include "../lib/Scene.h"
Scene::Scene(std::string Name) : name(Name) {}
Scene::~Scene() {}
void Scene::Start() {

}

Scene::Scene(YAML::Node node) {
	this->name = node["name"].as<std::string>();
	if (node["gameObjects"]) {
		YAML::Node gameObjectsNode = node["gameObjects"];
		for (auto goNode : gameObjectsNode) {
			gameObjects.push_back(new GameObject(goNode));
		}
	}
}

void Scene::Update(glm::mat4 view, glm::mat4 perspective, float time) {
	for (auto go : gameObjects) {
		go->Update(view, perspective, time);
	}
}

void Scene::addObject(GameObject* go) {
	gameObjects.push_back(go);
}

GameObject* Scene::findByName(std::string name) {
	for (auto go : gameObjects) {
		if (go->name == name) {
			return go;
		}
	}
}

YAML::Node Scene::serialize() {
	YAML::Node node;
	node["name"] = this->name;
	for (auto go : gameObjects) {
		node["gameObjects"].push_back(go->serialize());
	}
	return node;
}
