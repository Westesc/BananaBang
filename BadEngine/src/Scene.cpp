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
	ZoneTransientN(zoneName, "Update", true);
	for (auto go : gameObjects) {
		go->Update(view, perspective, time);
	}
}

void Scene::addObject(GameObject* go) {
	if (std::find(gameObjects.begin(), gameObjects.end(), go) == gameObjects.end()) {
		gameObjects.push_back(go);
	}
}

GameObject* Scene::findByName(std::string name) {
	ZoneTransientN(zoneName, "findByName", true);
	for (auto go : gameObjects) {
		if (go->name == name) {
			return go;
		}
	}
	return nullptr;
}

void Scene::Draw(glm::mat4 view, glm::mat4 perspective) {
	ZoneTransientN(zoneName, "Draw", true);
	for (auto go : gameObjects) {
		if (go->isVisible) {
			go->Draw(view, perspective);
		}
	}
}
void Scene::Draw(Shader* shader) {
	for (auto go : gameObjects) {
		if (go->isVisible) {
			go->Draw(shader);
		}
	}
}




/*void Scene::checkResolveCollisions(float deltaTime) {
	for (int i = 0; i < gameObjects.size(); i++) {
		for (int j = i + 1; j < gameObjects.size(); j++) {
			gameObjects.at(i)->checkResolveCollisions(gameObjects.at(j), deltaTime);
		}
	}
}*/

YAML::Node Scene::serialize() {
	YAML::Node node;
	node["name"] = this->name;
	for (auto go : gameObjects) {
		node["gameObjects"].push_back(go->serialize());
	}
	return node;
}

void Scene::lightSetting(glm::vec3 viewPos, glm::vec3 lightPos, glm::vec3 lightColor) {
	for (auto go : gameObjects) {
		if (go->isVisible) {
			go->lightSetting(viewPos, lightPos,  lightColor);
		}
	}
}
void Scene::shadowSetting(glm::mat4 LSMatrix) {
	for (auto go : gameObjects) {
		if (go->isVisible) {
			go->shadowSetting(LSMatrix);
		}
	}
}