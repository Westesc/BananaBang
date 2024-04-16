#include "../lib/Scene.h"
Scene::Scene(std::string Name) : name(Name) {}
Scene::~Scene() {}
void Scene::Start() {

}
void Scene::Update(glm::mat4 view, glm::mat4 perspective, float time, Shader* shaders) {
	for (auto go : gameObjects) {
		go->Update(view, perspective, time);
		go->checkResolveCollisions();
		go->Draw(shaders, view, perspective);
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