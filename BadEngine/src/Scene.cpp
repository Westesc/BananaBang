#include "../lib/Scene.h"
Scene::Scene(std::string Name) : name(Name) {}
Scene::~Scene() {
	for (auto go : gameObjects) {
		delete go;
	}
	gameObjects.clear();
}
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

void Scene::Draw(glm::mat4 view, glm::mat4 perspective) {
	for (auto go : gameObjects) {
		if (go->isVisible) {
			go->Draw(view, perspective);
		}
	}
}

void Scene::checkResolveCollisions(float deltaTime) {
	for (int i = 0; i < gameObjects.size(); i++) {
		for (int j = i + 1; j < gameObjects.size(); j++) {
			gameObjects.at(i)->checkResolveCollisions(gameObjects.at(j), deltaTime);
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