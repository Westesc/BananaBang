#include "../lib/Scene.h"
Scene::Scene(std::string Name) : name(Name) {}
Scene::~Scene() {}
void Scene::Start() {

}
void Scene::Update() {

}

void Scene::addObject(GameObject* go) {
	gameObjects.push_back(go);
}

GameObject* Scene::findByName(std::string name) {
	for (int i = 0; i < gameObjects.size(); i++) {
		if (gameObjects.at(i)->name == name) {
			return gameObjects.at(i);
		}
	}
}