#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#include <stdio.h>
#include<GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>
#include<fstream>
#include <iostream>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../lib/GraphNode.h"
#include "../lib/Shader.h"
#include "../lib/GameObject.h"
#include "../lib/SceneManager.h"
#include "../lib/AnimateBody.h"
#include "../lib/Axis.h"
#include "../lib/Camera.h"
#include "../lib/Component.h"
#include "../lib/Input.h"
#include "../lib/RigidBody.h"
#include "../lib/Scene.h"
#include "../lib/Transform.h"
#include "../lib/UI.h"
#include "../lib/CollisionManager.h"
#include "../lib/PlayerMovement.h"
#include "../lib/GameMode.h"

//bool test = true;
bool test = false;
bool frustumTest = false;

void DrawPlane(float scale, Shader* shaders, GameObject* plane, glm::vec3 vektor);
void DrawTree(float scaleT, float scale, Shader* shaders, GameObject* plane, glm::vec3 vektor);
int losujLiczbe(int a, int b);
int losujLiczbe2();

void setupImGui(GLFWwindow* window);
void renderImGui();
void cleanupImGui();


std::string loadShaderSource(const std::string& _filepath);
GLuint compileShader(const GLchar* _source, GLenum _stage, const std::string& _msg);

using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;
PlayerMovement* pm;

Camera* camera = new Camera();
using Duration = std::chrono::duration<float, std::ratio<1, 1>>;

constexpr int wys = 800, szer = 1000;
GLFWwindow* window;
SceneManager* sm;
Input* input;
float boxSpeed = 4.f;

float scale = 5.f;
float scaleT = 1.f;
int sectors = 5;
int sectorsPom = 5;
int a = 3;
int b = 5;
bool buttonPressed;

void Start() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(szer, wys, "Hello World", nullptr, nullptr);
	if (!window) exit(1);

	glfwMakeContextCurrent(window);
	gladLoadGL();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);

	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);

	sm = new SceneManager();
	//Scene * scene = new Scene("main");
	//sm->scenes.push_back(scene);
	//sm->loadScene("first");
	//sm->activeScene = sm->scenes.at(0);
	Scene* scene = new Scene("main");
	sm->scenes.push_back(scene);
	sm->activeScene = sm->scenes.at(0);
	input = new Input(window);
	/*GameObject* go = new GameObject("test object");
	Transform* trans = new Transform();
	Component* comp = new Component();
	UI* ui = new UI();
	AnimateBody* ab = new AnimateBody();
	RigidBody* rb = new RigidBody();
	Axis* axis = new Axis("axis");
	go->getTransform();*/
	pm = new PlayerMovement(sm, input);
}

std::array<glm::vec4, 6> calculateFrustumPlanes(const glm::mat4& viewProjectionMatrix) {
	std::array<glm::vec4, 6> planes;

	// Left plane
	planes[0] = glm::row(viewProjectionMatrix, 3) + glm::row(viewProjectionMatrix, 0);
	// Right plane
	planes[1] = glm::row(viewProjectionMatrix, 3) - glm::row(viewProjectionMatrix, 0);
	// Top plane
	planes[2] = glm::row(viewProjectionMatrix, 3) - glm::row(viewProjectionMatrix, 1);
	// Bottom plane
	planes[3] = glm::row(viewProjectionMatrix, 3) + glm::row(viewProjectionMatrix, 1);
	// Near plane
	planes[4] = glm::row(viewProjectionMatrix, 3) + glm::row(viewProjectionMatrix, 2);
	// Far plane
	planes[5] = glm::row(viewProjectionMatrix, 3) - glm::row(viewProjectionMatrix, 2);

	return planes;
}

bool isBoxInFrustum(const std::array<glm::vec4, 6>& frustumPlanes, BoundingBox& box, glm::mat4* transform) {
	glm::vec3 vertices[] = {
			glm::vec3(*transform * glm::vec4(box.vertices.at(0), 1.0f)),
			glm::vec3(*transform * glm::vec4(box.vertices.at(1), 1.0f)),
			glm::vec3(*transform * glm::vec4(box.vertices.at(2), 1.0f)),
			glm::vec3(*transform * glm::vec4(box.vertices.at(3), 1.0f)),
			glm::vec3(*transform * glm::vec4(box.vertices.at(4), 1.0f)),
			glm::vec3(*transform * glm::vec4(box.vertices.at(5), 1.0f)),
			glm::vec3(*transform * glm::vec4(box.vertices.at(6), 1.0f)),
			glm::vec3(*transform * glm::vec4(box.vertices.at(7), 1.0f))
	};
	for (const auto& plane : frustumPlanes) {
		glm::vec4 normalizedPlane = plane / glm::length(glm::vec3(plane));
		for (const auto& vertex : vertices) {
			float distance = glm::dot(glm::vec3(normalizedPlane), vertex) + normalizedPlane.w;
			if (distance < 0.0f) {
				return false;
			}
		}
	}
	return true;
}

bool isCapsuleInFrustum(const std::array<glm::vec4, 6>& frustumPlanes, CapsuleCollider* capsule, glm::mat4* transform) {
	for (const auto& plane : frustumPlanes) {
		float distance = glm::dot(glm::vec3(plane), glm::vec3(*transform * glm::vec4(capsule->center, 1.0f))) + plane.w;
		if (distance <= -capsule->radius) {
			return false;
		}
		if (distance >= capsule->height + capsule->radius) {
			continue;
		}
	}
	return true;
}

void performFrustumCulling(const std::array<glm::vec4, 6>& frustumPlanes, const std::vector<GameObject*>& objects) {
	for (auto object : objects) {
		if (object->getModelComponent()->boundingBox != nullptr) {
			bool isVisible = isBoxInFrustum(frustumPlanes, *object->getModelComponent()->boundingBox, object->getModelComponent()->getTransform());
			object->setVisible(isVisible);
		}
		else if (object->getModelComponent()->capsuleCollider != nullptr) {
			bool isVisible = isCapsuleInFrustum(frustumPlanes, object->getModelComponent()->capsuleCollider, object->getModelComponent()->getTransform());
			object->setVisible(isVisible);
		}
	}
}


int main() {
	
	Start();
	int ilosc = 0;
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	std::vector<int> mojaTablica;
	for (int i = 0; i < sectors * sectors; ++i) {
		mojaTablica.push_back(losujLiczbe(a, b));
		ilosc += mojaTablica[i];
		std::cout << mojaTablica[i] << std::endl;
	}
	std::vector<int> placeX;
	std::vector<int> placeY;
	for (int i = 0; i < ilosc; ++i) {
		placeX.push_back(losujLiczbe2());
		placeY.push_back(losujLiczbe2());
	}

	Shader* shaders = new Shader("../../../../src/shaders/vs.vert", "../../../../src/shaders/fs.frag");
	Shader* skydomeShader = new Shader("../../../../src/shaders/vsS.vert", "../../../../src/shaders/fsS.frag");
	Shader* mapsShader = new Shader("../../../../src/shaders/v_maps.vert", "../../../../src/shaders/f_maps.frag");
	Shader* shaderTree = new Shader("../../../../src/shaders/vsTree.vert", "../../../../src/shaders/fsTree.frag");
	Shader* rampShader = new Shader("../../../../src/shaders/ramp.vert", "../../../../src/shaders/ramp.frag");

	GameObject* box = new GameObject("box");
	GameObject* plane = new GameObject("plane");
	GameObject* box2 = new GameObject("box2");
	GameObject* capsule = new GameObject("capsule");
	GameObject* capsule2 = new GameObject("capsule2");
	GameObject* skydome = new GameObject("skydome");
	GameObject* rampBox = new GameObject("rampBox");

	Model* boxmodel = new Model(const_cast<char*>("../../../../res/box.obj"));
	Model* planemodel = new Model(const_cast<char*>("../../../../res/plane.obj"));
	Model* box2model = new Model(const_cast<char*>("../../../../res/tree.obj"));
	Model* capsulemodel = new Model(const_cast<char*>("../../../../res/capsule.obj"));
	Model* rampModel = new Model(const_cast<char*>("../../../../res/box.obj"));

	Model* capsule2model = new Model(const_cast<char*>("../../../../res/capsule.obj"));
	Mesh* meshSphere = new Mesh();
	meshSphere->createDome(20, 20, 50);
	Model* skydomeModel = new Model(meshSphere);



	boxmodel->SetShader(mapsShader);
	planemodel->SetShader(shaders);
	box2model->SetShader(shaderTree);
	capsulemodel->SetShader(shaders);
	capsule2model->SetShader(shaders);
	skydomeModel->SetShader(skydomeShader);
	rampModel->SetShader(rampShader);
	box->addModelComponent(boxmodel);
	plane->addModelComponent(planemodel);
	box2->addModelComponent(box2model);
	capsule->addModelComponent(capsulemodel);
	capsule2->addModelComponent(capsule2model);
	skydome->addModelComponent(skydomeModel);
	rampBox->addModelComponent(rampModel);

	sm->getActiveScene()->addObject(box);
	sm->getActiveScene()->addObject(box2);
	sm->getActiveScene()->addObject(capsule);
	sm->getActiveScene()->addObject(plane);
	sm->getActiveScene()->addObject(capsule2);
	sm->getActiveScene()->addObject(skydome);
	sm->getActiveScene()->addObject(rampBox);
	int key, action;
	camera->transform->localPosition = glm::vec3(-1.0f, 2.0f, 20.0f);

	const TimePoint tpStart = Clock::now();
	static bool sequenceStarted = false;
	static bool firstKeyPressed = false;
	static bool secondKeyPressed = false;
	std::string name = "src";

	setupImGui(window);

	sm->getActiveScene()->findByName("skydome")->getModelComponent()->AddTexture("../../../../res/chmury1.png","diffuseMap");

	sm->getActiveScene()->findByName("box")->getModelComponent()->AddTexture("../../../../res/cegla.png", "diffuseMap");
	sm->getActiveScene()->findByName("box")->getModelComponent()->AddTexture("../../../../res/specular2.png", "specularMap");
	sm->getActiveScene()->findByName("box")->getModelComponent()->AddTexture("../../../../res/normal2.png", "normalMap");

	glm::vec3 lightPos(0.5f, 10.0f, 0.3f);



	/*box->localTransform->localPosition = glm::vec3(-1.f, -1.f, 0.f);
	box2->localTransform->localPosition = glm::vec3(-4.f, -4.f, 0.f);
	capsule->localTransform->localPosition = glm::vec3(4.f, 4.f, 0.f);
	//box2->getModelComponent()->setCustomBox(glm::vec3(-8.f,-8.f,0.f), glm::vec3(8.f,8.f,8.f));
	box->getModelComponent()->addCollider(1, box->localTransform->localPosition);
	capsule->getModelComponent()->addCollider(2, capsule->localTransform->localPosition);
	box2->getModelComponent()->addCollider(1, box->localTransform->localPosition);*/
	sm->getActiveScene()->findByName("box")->getTransform()->localPosition = glm::vec3(-1.f, -1.f, 0.f);
	sm->getActiveScene()->findByName("box2")->getTransform()->localPosition = glm::vec3(-4.f, -4.f, 0.f);
	sm->getActiveScene()->findByName("capsule")->getTransform()->localPosition = glm::vec3(4.f, 4.f, 0.f);
	sm->getActiveScene()->findByName("capsule2")->getTransform()->localPosition = glm::vec3(8.f, 8.f, 0.f);
	sm->getActiveScene()->findByName("rampBox")->getTransform()->localPosition = glm::vec3(0.f, 5.f, 0.f);
	sm->getActiveScene()->findByName("box")->getModelComponent()->addCollider(1, sm->getActiveScene()->findByName("box")->getTransform()->localPosition);
	sm->getActiveScene()->findByName("box2")->getModelComponent()->addCollider(1, sm->getActiveScene()->findByName("box2")->getTransform()->localPosition);
	sm->getActiveScene()->findByName("capsule")->getModelComponent()->addCollider(2, sm->getActiveScene()->findByName("capsule")->getTransform()->localPosition);
	sm->getActiveScene()->findByName("capsule2")->getModelComponent()->addCollider(2, sm->getActiveScene()->findByName("capsule2")->getTransform()->localPosition);
	sm->getActiveScene()->findByName("box2")->setRotating(true);
	sm->getActiveScene()->findByName("plane")->setRotating(true);
	sm->getActiveScene()->findByName("capsule")->setRotating(true);
	sm->getActiveScene()->findByName("skydome")->setRotating(true, 1.f, glm::vec3(0.f, 1.f, 0.f));

	float deltaTime = 0;
	float deltaTime2 = 0;
	float lastTime = 0;
	GameMode gameMode;
	bool isFromFile = false;
	bool rotating = true;
	bool isBlue = false;


	CollisionManager cm = CollisionManager(1000, 100);
	sm->getActiveScene()->findByName("rampBox")->getModelComponent()->GetShader()->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sm->getActiveScene()->findByName("rampBox")->getModelComponent()->TextureFromFileClamp("../../../../res/gradient.png"));
	glUniform1i(glGetUniformLocation(sm->getActiveScene()->findByName("rampBox")->getModelComponent()->GetShader()->ID,"gradientTexture"), 0);

	sm->loadScene("first");
	sm->activeScene = sm->scenes.at(2);
	sm->getActiveScene()->findByName("sphere")->getModelComponent()->GetShader()->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sm->getActiveScene()->findByName("sphere")->getModelComponent()->TextureFromFileClamp("../../../../res/gradient.png"));
	glUniform1i(glGetUniformLocation(sm->getActiveScene()->findByName("sphere")->getModelComponent()->GetShader()->ID, "gradientTexture"), 0);
	while (!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, 1);
		}
		glClearColor(0.2f, 0.3f, 0.7f, 1.f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		const float time = std::chrono::duration_cast<Duration>(Clock::now() - tpStart).count();
		deltaTime = time - lastTime;
		deltaTime2 += time - lastTime;
		lastTime = time;
		//std::cout << "Delta time: " << deltaTime << std::endl;

		glm::mat4 V = camera->getViewMatrix();

		glm::mat4 P = glm::perspective(glm::radians(45.f), static_cast<float>(szer) / wys, 1.f, 5000.f);
		std::array<glm::vec4, 6> frustumPlanes = calculateFrustumPlanes(glm::perspective(glm::radians(60.f), static_cast<float>(szer) / wys, 1.f, 500.f) * camera->getViewMatrix());

		/*if (skydome->getModelComponent() != nullptr) {
			glm::mat4 Mm = glm::translate(glm::mat4(1.f), glm::vec3(20.f, 0.f, 18.f));
			Mm = glm::scale(Mm, glm::vec3(50.f, 50.0f, 50.0f));
			Mm = glm::rotate(Mm, glm::radians(time), glm::vec3(0.f, 1.f, 0.f));
			skydomeShader->use();
			skydomeShader->setMat4("M", Mm);
			skydomeShader->setMat4("view", V);
			skydomeShader->setMat4("projection", P);
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, texture);
			skydomeModel->Draw();
		}*/
		if (input->checkAnyKey())
		{
			if (input->checkKey(GLFW_KEY_TAB) && input->checkKey(GLFW_KEY_1))
			{
				gameMode.setMode(GameMode::Debug);
			}
			else if (input->checkKey(GLFW_KEY_TAB) && input->checkKey(GLFW_KEY_2))
			{
				gameMode.setMode(GameMode::Start);
			}
			else if (input->checkKey(GLFW_KEY_TAB) && input->checkKey(GLFW_KEY_3))
			{
				gameMode.setMode(GameMode::Game);
			}
			else if (input->checkKey(GLFW_KEY_TAB) && input->checkKey(GLFW_KEY_4))
			{
				gameMode.setMode(GameMode::Menu);
			}
		}

		if (gameMode.getMode() == GameMode::Game) {
			pm->ManagePlayer(deltaTime, deltaTime2);
		}
		if (sm->getActiveScene()->findByName("box")!=nullptr) {
			if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("box")->Move(glm::vec3(0.0f, 0.0f, boxSpeed * deltaTime));
			}
			if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("box")->Move(glm::vec3(0.0f, 0.0f, -boxSpeed * deltaTime));
			}
			if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("box")->Move(glm::vec3(-boxSpeed * deltaTime, 0.0f, 0.0f));
			}
			if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("box")->Move(glm::vec3(boxSpeed * deltaTime, 0.0f, 0.0f));
			}
			if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("box")->Move(glm::vec3(0.0f, boxSpeed * deltaTime, 0.0f));
			}
			if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("box")->Move(glm::vec3(0.0f, -boxSpeed * deltaTime, 0.0f));
			}
		}

		if (sm->getActiveScene()->findByName("sphere") != nullptr) {
			if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("sphere")->Move(glm::vec3(0.0f, 0.0f, boxSpeed * deltaTime));
			}
			if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("sphere")->Move(glm::vec3(0.0f, 0.0f, -boxSpeed * deltaTime));
			}
			if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("sphere")->Move(glm::vec3(-boxSpeed * deltaTime, 0.0f, 0.0f));
			}
			if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("sphere")->Move(glm::vec3(boxSpeed * deltaTime, 0.0f, 0.0f));
			}
			if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("sphere")->Move(glm::vec3(0.0f, boxSpeed * deltaTime, 0.0f));
			}
			if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("sphere")->Move(glm::vec3(0.0f, -boxSpeed * deltaTime, 0.0f));
			}
		}

		if (sm->getActiveScene()->findByName("capsule2") != nullptr) {
			if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("capsule2")->Move(glm::vec3(0.0f, 0.0f, boxSpeed * deltaTime));
			}
			if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("capsule2")->Move(glm::vec3(0.0f, 0.0f, -boxSpeed * deltaTime));
			}
			if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("capsule2")->Move(glm::vec3(-boxSpeed * deltaTime, 0.0f, 0.0f));
			}
			if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("capsule2")->Move(glm::vec3(boxSpeed * deltaTime, 0.0f, 0.0f));
			}
			if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("capsule2")->Move(glm::vec3(0.0f, boxSpeed * deltaTime, 0.0f));
			}
			if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("capsule2")->Move(glm::vec3(0.0f, -boxSpeed * deltaTime, 0.0f));
			}
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			frustumTest = !frustumTest;
		}
		sm->getActiveScene()->Update(V, P, time);
		//sm->getActiveScene()->findByName("skydome")->getModelComponent()->setTransform(glm::translate(*sm->getActiveScene()->findByName("skydome")->getModelComponent()->getTransform(), glm::vec3(20.f, 0.f, 18.f)));
		//sm->getActiveScene()->findByName("skydome")->getModelComponent()->setTransform(glm::scale(*sm->getActiveScene()->findByName("skydome")->getModelComponent()->getTransform(), glm::vec3(50.f, 50.0f, 50.0f)));
		//sm->getActiveScene()->findByName("skydome")->getModelComponent()->setTransform(glm::rotate(*sm->getActiveScene()->findByName("skydome")->getModelComponent()->getTransform(), glm::radians(time), glm::vec3(0.f, 1.f, 0.f)));
		//sm->getActiveScene()->findByName("plane")->getModelComponent()->setTransform(glm::rotate(*sm->getActiveScene()->findByName("plane")->getModelComponent()->getTransform(), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f)));
		for (int i = 0; i < sm->getActiveScene()->gameObjects.size(); i++) {
			if (sm->getActiveScene()->gameObjects.at(i)->getModelComponent()->boundingBox != nullptr || sm->getActiveScene()->gameObjects.at(i)->getModelComponent()->capsuleCollider != nullptr)
			{
				cm.addObject(sm->getActiveScene()->gameObjects.at(i));
			}
			if (sm->getActiveScene()->gameObjects.at(i)->modelComponent != nullptr) {
				sm->getActiveScene()->gameObjects.at(i)->getModelComponent()->GetShader()->use();
				sm->getActiveScene()->gameObjects.at(i)->getModelComponent()->GetShader()->setVec3("viewPos", camera->transform->getLocalPosition());
				sm->getActiveScene()->gameObjects.at(i)->getModelComponent()->GetShader()->setVec3("lightPos", lightPos);
				sm->getActiveScene()->gameObjects.at(i)->getModelComponent()->GetShader()->setVec3("lightColor", glm::vec3(1.0f));
			}
		}
		cm.checkResolveCollisions(deltaTime);
		performFrustumCulling(frustumPlanes, sm->getActiveScene()->gameObjects);
		if (frustumTest) {
			for (int i = 0; i < sm->getActiveScene()->gameObjects.size(); i++) {
				if (sm->getActiveScene()->gameObjects.at(i)->isVisible) {
					std::cout << sm->getActiveScene()->gameObjects.at(i)->name << " " << std::endl;
				}
			}
		}
		sm->getActiveScene()->Draw(V, P);

		mapsShader->use();
		mapsShader->setVec3("viewPos", camera->transform->getLocalPosition());
		mapsShader->setVec3("lightPos", lightPos);
		mapsShader->setMat4("M", *box->getModelComponent()->getTransform());
		mapsShader->setMat4("view", V);
		mapsShader->setMat4("projection", P);
		//box->getModelComponent()->Draw();
		//box->getModelComponent()->DrawBoundingBoxes(mapsShader, *box->getModelComponent()->getTransform());

		shaderTree->use();
		//shaders->setMat4("M", *box2->getModelComponent()->getTransform());
		shaderTree->setMat4("view", V);
		shaderTree->setMat4("projection", P);
		//box2->getModelComponent()->Draw();
		//box2->getModelComponent()->DrawBoundingBoxes(shaders, *box2->getModelComponent()->getTransform());

		/*shaders->use();
		shaders->setMat4("M", *capsule->getModelComponent()->getTransform());
		shaders->setMat4("view", V);
		shaders->setMat4("projection", P);
		capsule->getModelComponent()->Draw();
		capsule->getModelComponent()->UpdateCollider(*capsule->getModelComponent()->getTransform());
		shaders->use();
		shaders->setMat4("M", *capsule2->getModelComponent()->getTransform());
		shaders->setMat4("view", V);
		shaders->setMat4("projection", P);
		capsule2->getModelComponent()->Draw();
		capsule2->getModelComponent()->UpdateCollider(*capsule2->getModelComponent()->getTransform());*/
		if (input->IsMove()) {
			glm::vec2 dpos = input->getPosMouse();
			if (glfwGetInputMode(window, GLFW_CURSOR) != 212993) {
				camera->updateCamera(dpos);
			}
		}
		if (buttonPressed) {
			sectors = sectorsPom;
			mojaTablica.clear();
			for (int i = 0; i < sectors * sectors; ++i) {
				mojaTablica.push_back(losujLiczbe(a, b));
				ilosc += mojaTablica[i];
				std::cout << mojaTablica[i] << std::endl;
			}
			placeX.clear();
			placeY.clear();
			for (int i = 0; i < ilosc; ++i) {
				placeX.push_back(losujLiczbe2());
				placeY.push_back(losujLiczbe2());
			}
			buttonPressed = false;
		}
		if (input->IsKeobarodAction(window)) {
			input->GetMessage(key, action);

			if (gameMode.getMode() == GameMode::Debug) {
				// Obsługa sekwencji klawiszy
				if (key == GLFW_KEY_W && action == GLFW_PRESS) {
					if (!sequenceStarted) {
						sequenceStarted = true;
						secondKeyPressed = false;
						firstKeyPressed = true;
						std::cout << "W" << std::endl;
						camera->ProcessKeyboard(FORWARD, time);
					}
					else if (firstKeyPressed && !secondKeyPressed) {
						secondKeyPressed = true;
						sequenceStarted = false;
						firstKeyPressed = false;
						std::cout << "Sekwencja klawiszy W + W została wykryta!" << std::endl;
					}
				}
				else if (key == GLFW_KEY_W && action == GLFW_REPEAT) {
					camera->ProcessKeyboard(FORWARD, time);
				}
				else if (key == GLFW_KEY_S && action == GLFW_REPEAT) {
					camera->ProcessKeyboard(BACKWARD, time);
				}
				else if (key == GLFW_KEY_D && action == GLFW_REPEAT) {
					camera->ProcessKeyboard(RIGHT, time);
				}
				else if (key == GLFW_KEY_A && action == GLFW_REPEAT) {
					camera->ProcessKeyboard(LEFT, time);
				}
				else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
				else if (key == GLFW_KEY_X && action == GLFW_PRESS) {
					std::cout << "KLAWISZ X " << key << std::endl;
				}
				else if (key == GLFW_KEY_Z && action == GLFW_REPEAT) {
					std::cout << "KLAWISZ Z " << key << std::endl;
				}
				//jednorazowe
				else if (key == GLFW_MOUSE_BUTTON_RIGHT) {
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					std::cout << "LEFT MOUSE " << key << std::endl;
				}
				else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
					std::cout << "Wyłączono klawisz S " << key << std::endl;
				}
				else if (action == GLFW_REPEAT) {
					std::cout << "Nacisnieto klawisz " << key << std::endl;
				}
				else if (action == GLFW_RELEASE) {
					std::cout << "Puszczono klawisz " << key << std::endl;
				}
			}
		}
		int pom = 0;
		if (plane->getModelComponent() != nullptr) {
			for (int i = 0; i < sectors; i++)
			{
				for (int j = 0; j < sectors; j++) {
					DrawPlane(scale, shaders, plane, glm::vec3(i * 20, 0.f, j * 20));
					for (int z = 0; z < mojaTablica[i * sectors + j]; z++) {
						DrawTree(scaleT, scale, shaderTree, box2, glm::vec3(i * 20 + placeX[pom] - 1.5, 0.f, j * 20 + placeY[pom]));
						pom++;
					}
				}
			}
		}
		renderImGui();
		glfwSwapBuffers(window);
		glfwPollEvents();
		if (test) {

			sm->saveScene("first");
			test = false;
		}
	}
	cleanupImGui();
	glfwTerminate();
	return 0;
}

void renderImGui() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("System Generatywny");
	ImGui::SliderInt("Sectors", &sectorsPom, 1, 10);
	ImGui::SliderInt("Min drzew", &a, 0, 10);
	ImGui::SliderInt("Max drzew", &b, a, 20);
	if (ImGui::Button("Generate")) {
		buttonPressed = true;
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void setupImGui(GLFWwindow* window) {
	// Konfiguracja ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImGui::StyleColorsDark();
}

void cleanupImGui() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void DrawPlane(float scale, Shader* shaders, GameObject* plane, glm::vec3 vektor) {
	glm::mat4 M2 = glm::translate(glm::mat4(1.f), scale * vektor);
	M2 = glm::scale(M2, glm::vec3(scale, scale, scale));
	plane->getModelComponent()->setTransform(M2);
	shaders->use();
	shaders->setMat4("M", M2);
	plane->getModelComponent()->Draw();
}
void DrawTree(float scaleT, float scale, Shader* shaders, GameObject* plane, glm::vec3 vektor) {
	glm::mat4 M2 = glm::translate(glm::mat4(1.f), scale * vektor);
	M2 = glm::scale(M2, glm::vec3(scaleT, scaleT, scaleT));
	plane->getModelComponent()->setTransform(M2);
	shaders->use();
	shaders->setMat4("M", M2);
	plane->getModelComponent()->Draw();
}

int losujLiczbe(int a, int b) {
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	return std::rand() % (b - a + 1) + a;
}
//-2, 15
int losujLiczbe2() {
	return std::rand() % 18 - 2;
}


std::string loadShaderSource(const std::string& _filepath)
{
	std::ifstream f(_filepath, std::ios::ate | std::ios::in);
	const size_t len = f.tellg();
	f.seekg(0);

	std::string shdr;
	shdr.resize(len);

	f.read(&shdr.front(), len);

	return shdr;
}

GLuint compileShader(const GLchar* _source, GLenum _stage, const std::string& _msg)
{
	GLuint shdr = glCreateShaderProgramv(_stage, 1, &_source);
	std::string log;
	log.resize(1024);
	glGetProgramInfoLog(shdr, log.size(), nullptr, &log.front());
	printf("%s: %s\n", _msg.c_str(), log.c_str());

	return shdr;
}

