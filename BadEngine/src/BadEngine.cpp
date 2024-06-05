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
#include "../thirdparty/tracy/public/tracy/Tracy.hpp"
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
#include "../lib/Pathfinder.h"
#include "../lib/Enemy.h"
#include "../lib/Tree.h"
#include "../lib/PBD.h"
#include "../lib/TimeManager.h"
#include "../lib/animation/Animator.h"
#include "../lib/Globals.h"
#include "../lib/EnemyStateManager.h"


bool test = false;
bool frustumTest = false;

void DrawPlane(float scale, Shader* shaders, GameObject* plane, glm::vec3 vektor);
void DrawTree(float scaleT, float scale, Shader* shaders, GameObject* plane, glm::vec3 vektor);
int losujLiczbe(int a, int b);
int losujLiczbe2();
bool checkLocations(float x1, float y1,float x2,float y2,float distance);

void setupImGui(GLFWwindow* window);
void renderImGui();
void cleanupImGui();


std::string loadShaderSource(const std::string& _filepath);
GLuint compileShader(const GLchar* _source, GLenum _stage, const std::string& _msg);

using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;

using Duration = std::chrono::duration<float, std::ratio<1, 1>>;

constexpr int wys = 800, szer = 1000;
GLFWwindow* window;
SceneManager* sm;
Input* input;
PlayerMovement* pm;
Camera* camera;
TimeManager* tm = new TimeManager();
float boxSpeed = 3.f;
glm::vec3 lightPos(0.5f, 20.0f, 0.3f);
float scale = 5.f;
float scaleT = 1.f;
int sectors = 1;
int sectorsPom = 1;
int a = 0;
int b = 0;
bool buttonPressed;
unsigned int maxEnemies = 5;
unsigned int spawnedEnemies = 0;
bool loaded = false;
bool playerAtention = false;

void Start() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(Window::windowWidth, Window::windowHeight, "Monke", nullptr, nullptr);
	if (!window) exit(1);

	glfwMakeContextCurrent(window);
	gladLoadGL();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);

	glFrontFace(GL_CCW);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	sm = new SceneManager();
	Scene* scene = new Scene("main");
	sm->scenes.push_back(scene);
	sm->activeScene = sm->scenes.at(0);
	input = new Input(window);
	camera = new Camera(sm);
	pm = new PlayerMovement(sm, input, camera, tm);
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

bool isBoxInFrustum(const std::array<glm::vec4, 6>& frustumPlanes, BoundingBox& box, glm::mat4 transform) {
	glm::vec3 vertices[] = {
			glm::vec3(transform * glm::vec4(box.vertices.at(0), 1.0f)),
			glm::vec3(transform * glm::vec4(box.vertices.at(1), 1.0f)),
			glm::vec3(transform * glm::vec4(box.vertices.at(2), 1.0f)),
			glm::vec3(transform * glm::vec4(box.vertices.at(3), 1.0f)),
			glm::vec3(transform * glm::vec4(box.vertices.at(4), 1.0f)),
			glm::vec3(transform * glm::vec4(box.vertices.at(5), 1.0f)),
			glm::vec3(transform * glm::vec4(box.vertices.at(6), 1.0f)),
			glm::vec3(transform * glm::vec4(box.vertices.at(7), 1.0f))
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

bool isCapsuleInFrustum(const std::array<glm::vec4, 6>& frustumPlanes, CapsuleCollider& capsule, glm::mat4 transform) {
	for (const auto& plane : frustumPlanes) {
		float distance = glm::dot(glm::vec3(plane), glm::vec3(transform * glm::vec4(capsule.center, 1.0f))) + plane.w;
		if (distance <= -capsule.radius) {
			return false;
		}
		if (distance >= capsule.height + capsule.radius) {
			continue;
		}
	}
	return true;
}

void performFrustumCulling(const std::array<glm::vec4, 6>& frustumPlanes, const std::vector<GameObject*>& objects) {
	ZoneTransientN(zoneName, "performFrustumCulling", true);
	bool isVisible = true;
	for (auto object : objects) {
		if (object->boundingBox) {
			isVisible = isBoxInFrustum(frustumPlanes, *object->boundingBox, object->getTransform()->getMatrix());
			object->setVisible(isVisible);
		}
		else if (object->capsuleCollider) {
			isVisible = isCapsuleInFrustum(frustumPlanes, *object->capsuleCollider, object->getTransform()->getMatrix());
			object->setVisible(isVisible);
		}
		for (auto child : object->children) {
			child->setVisible(isVisible);
			for (auto grandchild : child->children) {
				grandchild->setVisible(isVisible);
			}
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
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	auto animodel = std::make_shared<Model>(const_cast<char*>("../../../../res/animations/Walking.dae"), true);
	//auto animodel2 = std::make_shared<Model>(const_cast<char*>("../../../../res/animations/Briefcase Idle.dae"),true);

	//AnimateBody* animPlayer = new AnimateBody(animodel.get());
	//animPlayer->addAnimation(const_cast<char*>("../../../../res/animations/Walking.dae"), "walking", 1.f);
	//animPlayer->addAnimation(const_cast<char*>("../../../../res/animations/Briefcase Idle.dae"), "standing", 1.f);
	//animPlayer->addAnimation(const_cast<char*>("../../../../res/animations/Jumping Up.dae"), "jumping up", 0.9f);
	//animPlayer->addAnimation(const_cast<char*>("../../../../res/animations/Jumping Down.dae"), "jumping down", 0.2f);
	//animPlayer->addAnimation(const_cast<char*>("../../../../res/animations/Punching.dae"), "attack1", 1.f);
	//animPlayer->addAnimation(const_cast<char*>("../../../../res/animations/Dodge.dae"), "dodge", 1.f);
	//pm->addAnimationPlayer(animPlayer);

	Shader* shaderAnimation = new Shader("../../../../src/shaders/vs_animation.vert", "../../../../src/shaders/fs_animation.frag");
	GameObject* anim = new GameObject("player");
	animodel->SetShader(shaderAnimation);
	anim->addModelComponent(animodel);
	anim->addAnimation(const_cast<char*>("../../../../res/animations/Walking.dae"), "walking", 1.f);
	anim->addAnimation(const_cast<char*>("../../../../res/animations/Briefcase Idle.dae"), "standing", 1.f);
	anim->addAnimation(const_cast<char*>("../../../../res/animations/Jumping Up.dae"), "jumping up", 0.9f);
	anim->addAnimation(const_cast<char*>("../../../../res/animations/Jumping Down.dae"), "jumping down", 0.2f);
	anim->addAnimation(const_cast<char*>("../../../../res/animations/Punching.dae"), "attack1", 1.f);
	anim->addAnimation(const_cast<char*>("../../../../res/animations/Dodge.dae"), "dodge", 1.f);
	sm->getActiveScene()->addObject(anim);

	Shader* fillingShader = new Shader("../../../../src/shaders/vs_filling.vert", "../../../../src/shaders/fs_filling.frag");
	Shader* outlineShader = new Shader("../../../../src/shaders/vs_outline.vert", "../../../../src/shaders/fs_outline.frag");
	Shader* shaders = new Shader("../../../../src/shaders/vs.vert", "../../../../src/shaders/fs.frag");
	Shader* skydomeShader = new Shader("../../../../src/shaders/vsS.vert", "../../../../src/shaders/fsS.frag");
	Shader* mapsShader = new Shader("../../../../src/shaders/v_maps.vert", "../../../../src/shaders/f_maps.frag");
	Shader* shaderTree = new Shader("../../../../src/shaders/vsTree.vert", "../../../../src/shaders/fsTree.frag");
	Shader* rampShader = new Shader("../../../../src/shaders/ramp.vert", "../../../../src/shaders/ramp.frag");
	Shader* enemyShader = new Shader("../../../../src/shaders/enemy.vert", "../../../../src/shaders/enemy.frag");
	auto enemyModel = std::make_shared<Model>(const_cast<char*>("../../../../res/capsule.obj"), false);
	enemyModel->SetShader(enemyShader);

	//depth shader
	Shader* depthShader = new Shader("../../../../src/shaders/depthShader.vert", "../../../../src/shaders/depthShader.frag");
	Shader* depthAnimationShader = new Shader("../../../../src/shaders/depthAnimationShader.vert", "../../../../src/shaders/depthAnimationShader.frag");

	GameObject* box = new GameObject("box");
	GameObject* plane = new GameObject("plane");
	GameObject* box2 = new GameObject("box2");
	GameObject* capsule = new GameObject("capsule");
	GameObject* capsule2 = new GameObject("capsule2");
	GameObject* skydome = new GameObject("skydome");
	GameObject* rampBox = new GameObject("rampBox");

	auto boxmodel = std::make_shared<Model>(const_cast<char*>("../../../../res/box.obj"), false);
	auto planemodel = std::make_shared<Model>(const_cast<char*>("../../../../res/plane.obj"), false);
	auto box2model = std::make_shared<Model>(const_cast<char*>("../../../../res/tree.obj"), false);
	auto capsulemodel = std::make_shared<Model>(const_cast<char*>("../../../../res/capsule.obj"), false);
	auto rampModel = std::make_shared<Model>(const_cast<char*>("../../../../res/box.obj"), false);

	auto capsule2model = std::make_shared<Model>(const_cast<char*>("../../../../res/capsule.obj"), false);
	Mesh* meshSphere = new Mesh();
	meshSphere->createDome(20, 20, 50);
	auto skydomeModel = std::make_shared<Model>(meshSphere);

	Mesh* meshFruit = new Mesh();
	meshFruit->createSphere(20, 20, 50);
	auto FruitModel = std::make_shared<Model>(meshFruit);


	Shader* hudShader = new Shader("../../../../src/shaders/hud.vert", "../../../../src/shaders/hud.frag");

	//drzewa
	auto treelog = std::make_shared<Model>(const_cast<char*>("../../../../res/objects/trees/tree_log.obj"), false);
    treelog->AddTexture("../../../../res/textures/bark.jpg", "diffuseMap");
	auto treetrunk = std::make_shared<Model>(const_cast<char*>("../../../../res/objects/trees/tree_trunk.obj"), false);
    treetrunk->AddTexture("../../../../res/textures/bark.jpg", "diffuseMap");
	Shader* phongShader = new Shader("../../../../src/shaders/phong.vert", "../../../../src/shaders/phong.frag");
	auto treebranch1= std::make_shared<Model>(const_cast<char*>("../../../../res/objects/trees/tree_branch_1.obj"), false);
    treebranch1->AddTexture("../../../../res/textures/bark.jpg", "diffuseMap");
	treebranch1->SetShader(phongShader);
	auto planeSectormodel = std::make_shared<Model>(const_cast<char*>("../../../../res/plane.obj"), false);
	planeSectormodel->AddTexture("../../../../res/drewno.png", "diffuseMap");
	treetrunk->SetShader(phongShader);
	treelog->SetShader(phongShader);
	planeSectormodel->SetShader(phongShader);

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

	skydome->localTransform->localScale=glm::vec3(100.f);

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

	std::string name = "src";

	setupImGui(window);

	sm->getActiveScene()->findByName("player")->getModelComponent()->AddTexture("../../../../res/bialy.png", "diffuseMap");
	sm->getActiveScene()->findByName("player")->getTransform()->localPosition = glm::vec3(7.f, 1.f, 1.f);
	sm->getActiveScene()->findByName("player")->getTransform()->localScale = glm::vec3(1.f, 1.f, 1.f);

	sm->getActiveScene()->findByName("skydome")->getModelComponent()->AddTexture("../../../../res/chmury1.png","diffuseMap");

	sm->getActiveScene()->findByName("box")->getModelComponent()->AddTexture("../../../../res/cegla.png", "diffuseMap");
	sm->getActiveScene()->findByName("box")->getModelComponent()->AddTexture("../../../../res/specular2.png", "specularMap");
	sm->getActiveScene()->findByName("box")->getModelComponent()->AddTexture("../../../../res/normal2.png", "normalMap");
	
	//glm::vec3 lightPos(0.5f, 20.0f, 0.3f);
	glm::vec3* lightColor = new glm::vec3(1.f, 1.0f, 1.f);

	//Depth map to generate shadows
	const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	//creating depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	sm->getActiveScene()->findByName("box")->getTransform()->localPosition = glm::vec3(-1.f, -1.f, 0.f);
	sm->getActiveScene()->findByName("box2")->getTransform()->localPosition = glm::vec3(-4.f, -4.f, 0.f);
	sm->getActiveScene()->findByName("capsule")->getTransform()->localPosition = glm::vec3(4.f, 4.f, 0.f);
	sm->getActiveScene()->findByName("capsule2")->getTransform()->localPosition = glm::vec3(8.f, 8.f, 0.f);
	sm->getActiveScene()->findByName("rampBox")->getTransform()->localPosition = glm::vec3(0.f, 5.f, 0.f);
	sm->getActiveScene()->findByName("box")->getModelComponent()->addCollider(1, sm->getActiveScene()->findByName("box")->getTransform()->localPosition,1.0f);
	sm->getActiveScene()->findByName("box2")->getModelComponent()->addCollider(1, sm->getActiveScene()->findByName("box2")->getTransform()->localPosition, 1.0f);
	sm->getActiveScene()->findByName("capsule")->getModelComponent()->addCollider(2, sm->getActiveScene()->findByName("capsule")->getTransform()->localPosition, 1.0f);
	sm->getActiveScene()->findByName("capsule2")->getModelComponent()->addCollider(2, sm->getActiveScene()->findByName("capsule2")->getTransform()->localPosition, 1.0f);
	sm->getActiveScene()->findByName("box2")->setRotating(true);
	sm->getActiveScene()->findByName("plane")->setRotating(true);
	sm->getActiveScene()->findByName("capsule")->setRotating(true);
	sm->getActiveScene()->findByName("skydome")->setRotating(true, 1.f, glm::vec3(0.f, 1.f, 0.f));

	float deltaTime = 0;
	float deltaTime2 = 0;
	float lastTime = 0;
	float spawnerTime = 0;
	float staticUpdateTime = 0;
	GameMode gameMode;
	bool isFromFile = false;
	bool rotating = true;
	bool isBlue = false;


	CollisionManager cm = CollisionManager(1000, 100);
	cm.pm = pm;
	sm->getActiveScene()->findByName("rampBox")->getModelComponent()->GetShader()->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sm->getActiveScene()->findByName("rampBox")->getModelComponent()->TextureFromFile("../../../../res/gradient.png"));
	glUniform1i(glGetUniformLocation(sm->getActiveScene()->findByName("rampBox")->getModelComponent()->GetShader()->ID,"gradientTexture"), 0);

	sm->loadScene("first");
	sm->activeScene = sm->scenes.at(3);
	sm->getActiveScene()->addObject(anim);
	//std::cout << "player:" << sm->getActiveScene()->findByName("box") << std::endl;
	//sm->getActiveScene()->findByName("tree_1")->addChild(new GameObject("log"));

	Pathfinder* pathfinder = new Pathfinder();
	PBDManager* pbd = new PBDManager(10);
	EnemyStateManager* enemyManager = new EnemyStateManager(pathfinder, &cm);
	while (!glfwWindowShouldClose(window)) {
		FrameMark;
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, 1);
		}
		glClearColor(0.2f, 0.3f, 0.7f, 1.f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		const float time = std::chrono::duration_cast<Duration>(Clock::now() - tpStart).count();
		deltaTime = time - lastTime;
		deltaTime2 += time - lastTime;
		lastTime = time;
		spawnerTime += deltaTime;
		staticUpdateTime += deltaTime;
		//std::cout << "Delta time: " << deltaTime << std::endl;

		tm->setTime(deltaTime);
		glm::mat4 V(1.f);
		if (gameMode.getMode() == GameMode::Game) {
			pm->ManagePlayer(deltaTime2, deltaTime);
			V = camera->getViewMatrixPlayer();
		}
		else if (gameMode.getMode() != GameMode::Game) {

			V = camera->getViewMatrix();
		}
		//animacje
		//animPlayer->UpdateAnimation(deltaTime);
		sm->getActiveScene()->findByName("skydome")->timeSetting(time / 7, glm::vec2(10, 10));

		glm::mat4 P = glm::perspective(glm::radians(input->GetZoom()), static_cast<float>(Window::windowWidth) / Window::windowHeight, 1.f, 5000.f);
		std::array<glm::vec4, 6> frustumPlanes = calculateFrustumPlanes(glm::perspective(glm::radians(60.f), static_cast<float>(Window::windowWidth) / Window::windowHeight, 1.f, 500.f) * camera->getViewMatrix());

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
		if (sm->getActiveScene()->findByName("rampBox")) {
			if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("rampBox")->Move(glm::vec3(0.0f, 0.0f, boxSpeed * deltaTime));
			}
			if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("rampBox")->Move(glm::vec3(0.0f, 0.0f, -boxSpeed * deltaTime));
			}
			if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("rampBox")->Move(glm::vec3(-boxSpeed * deltaTime, 0.0f, 0.0f));
			}
			if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("rampBox")->Move(glm::vec3(boxSpeed * deltaTime, 0.0f, 0.0f));
			}
			if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("rampBox")->Move(glm::vec3(0.0f, boxSpeed * deltaTime, 0.0f));
			}
			if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("rampBox")->Move(glm::vec3(0.0f, -boxSpeed * deltaTime, 0.0f));
			}
		}

		if (gameMode.getMode() == GameMode::Game) {
			pm->ManagePlayer(deltaTime2, deltaTime);
		}

		if (sm->getActiveScene()->findByName("player")) {
			if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
				//sm->getActiveScene()->findByName("player")->Move(glm::vec3(0.0f, 0.0f, boxSpeed * deltaTime));
				sm->getActiveScene()->findByName("player")->velocity+=glm::vec3(0.0f, 0.0f, boxSpeed);
			}
			if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
				//sm->getActiveScene()->findByName("player")->Move(glm::vec3(0.0f, 0.0f, -boxSpeed * deltaTime));
				sm->getActiveScene()->findByName("player")->velocity += glm::vec3(0.0f, 0.0f, -boxSpeed);
			}
			if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
				//sm->getActiveScene()->findByName("player")->Move(glm::vec3(-boxSpeed * deltaTime, 0.0f, 0.0f));
				sm->getActiveScene()->findByName("player")->velocity += glm::vec3(-boxSpeed, 0.0f, 0.0f);
			}
			if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
				//sm->getActiveScene()->findByName("player")->Move(glm::vec3(boxSpeed * deltaTime, 0.0f, 0.0f));
				sm->getActiveScene()->findByName("player")->velocity += glm::vec3(boxSpeed, 0.0f, 0.0f);
			}
			if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
				//sm->getActiveScene()->findByName("player")->Move(glm::vec3(0.0f, boxSpeed * deltaTime, 0.0f));
				if (sm->getActiveScene()->findByName("player")->getTransform()->localPosition.y < 0.1f) {
					sm->getActiveScene()->findByName("player")->velocity += glm::vec3(0.0f, boxSpeed, 0.0f);
				}
			}
			if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
				//sm->getActiveScene()->findByName("player")->Move(glm::vec3(0.0f, -boxSpeed * deltaTime, 0.0f));
				sm->getActiveScene()->findByName("player")->velocity += glm::vec3(0.0f, -boxSpeed, 0.0f);
			}

			if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
				staticUpdateTime = 0;
				playerAtention = true;
			}
		}

		if (sm->getActiveScene()->findByName("rampBox")) {
			if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("rampBox")->Move(glm::vec3(0.0f, 0.0f, boxSpeed * deltaTime));
			}
			if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("rampBox")->Move(glm::vec3(0.0f, 0.0f, -boxSpeed * deltaTime));
			}
			if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("rampBox")->Move(glm::vec3(-boxSpeed * deltaTime, 0.0f, 0.0f));
			}
			if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("rampBox")->Move(glm::vec3(boxSpeed * deltaTime, 0.0f, 0.0f));
			}
			if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("rampBox")->Move(glm::vec3(0.0f, boxSpeed * deltaTime, 0.0f));
			}
			if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("rampBox")->Move(glm::vec3(0.0f, -boxSpeed * deltaTime, 0.0f));
			}
		}
		if (sm->getActiveScene()->findByName("capsule2")) {
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
		/*for (auto object : sm->getActiveScene()->gameObjects) {
			if (object->name.starts_with("enemy")) {
				Enemy* enemy = static_cast<Enemy*>(object);
				switch (enemy->state) {
				case EnemyState::Idle: {
					enemy->velocity = glm::vec3(0.0f);
					std::pair<glm::vec3, Tree*> tree = pathfinder->decideInitalDestination(enemy->sector);
					if (tree.second != nullptr) {
						enemy->chosenTreePos = tree.first;
						enemy->chosenTree = tree.second;
						enemy->state = EnemyState::Walking;
					}
					break;
				}
					case EnemyState::Walking: {
						if (glm::distance(enemy->getTransform()->localPosition, enemy->chosenTreePos) > 5.f) {
							enemy->state = EnemyState::Walking;
							enemy->timeSinceDirChange += deltaTime;
							glm::vec3 destination = pathfinder->decideDestination(enemy->chosenTreePos, enemy->localTransform->getLocalPosition(), enemy->sector);
							glm::vec3 direction = glm::normalize(destination - enemy->localTransform->getLocalPosition());
							enemy->setVel(direction);
							glm::vec3 tmpMove = glm::vec3(enemy->velocity.x, 0.0f, enemy->velocity.z);
							enemy->Move(tmpMove * deltaTime);
							cm.addObjectPredict(enemy);
							std::vector<GameObject*> collisions = cm.checkPrediction();
							enemy->Move(-tmpMove * deltaTime);
							enemy->setVel2(collisions);
							if ((glm::any(glm::isnan(enemy->localTransform->localPosition)) || glm::any(glm::isinf(enemy->localTransform->localPosition)))) {
								enemy->localTransform->localPosition = glm::vec3(5.0f);
							}
							cm.addObject(enemy);
							enemy->timeSpentWalking += deltaTime;
							if (enemy->timeSpentWalking > 15.f) {
								std::pair<glm::vec3, Tree*> tree = pathfinder->decideInitalDestination(enemy->sector);
								if (tree.second == nullptr) {
									enemy->state = EnemyState::Idle;
								}
								else {
									enemy->chosenTreePos = tree.first;
									enemy->chosenTree = tree.second;
									enemy->timeSpentWalking = 0.f;
								}
							}
						}
						else if (enemy->state == EnemyState::Walking) {
							enemy->state = EnemyState::Chopping;
							//enemy->setVel(glm::vec3(0.0f));
							enemy->velocity = glm::vec3(0.0f);
							enemy->timeSpentWalking = 0.f;
						}
						break;
					}
					case EnemyState::Chopping:
						if (glm::distance(enemy->localTransform->localPosition, enemy->chosenTreePos) > 5.f) {
							enemy->state = EnemyState::Walking;
							if (enemy->chosenTree != nullptr) {
								enemy->chosenTree->getAsActualType<Tree>()->removeChopper(enemy);
							}
						}
						else {
							if (std::find(enemy->chosenTree->getAsActualType<Tree>()->choppers.begin(), enemy->chosenTree->getAsActualType<Tree>()->choppers.end(), enemy) == enemy->chosenTree->getAsActualType<Tree>()->choppers.end()) {
								enemy->chosenTree->getAsActualType<Tree>()->addChopper(enemy);
							}
						}
						break;
					case EnemyState::Attacking:
						break;
				}
			}
		}*/
	
		if (staticUpdateTime > 1.5f) {
			staticUpdateTime = 0.f;
			playerAtention = false;
		}
		enemyManager->update(deltaTime, playerAtention);
		for (auto object : sm->getActiveScene()->gameObjects) {
			if (object->name.starts_with("sector")) {
				for (auto tree : object->children) {
					Tree* treeActual = tree->getAsActualType<Tree>();
					//treeActual->updateHealth(deltaTime);
					if (treeActual->health <= 0) {
						object->children.erase(std::remove(object->children.begin(), object->children.end(), tree), object->children.end());
						delete treeActual;
					}
				}
			}
		}
		sm->getActiveScene()->Update(V, P, deltaTime);


		//generating shadows

		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 100.f;
		lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		depthShader->use();
		depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		depthAnimationShader->use();
		depthAnimationShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);


		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		sm->getActiveScene()->Draw(depthShader, depthAnimationShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, Window::windowWidth, Window::windowHeight);
		planeSectormodel->AddTexture(depthMap, "depthMap");


		sm->getActiveScene()->lightSetting(camera->transform->getLocalPosition(), lightPos, glm::vec3(1.0f));
		sm->getActiveScene()->shadowSetting(lightSpaceMatrix);

		if (sm->getActiveScene()->findByName("player")) {
			//cm.addObject(sm->getActiveScene()->findByName("player"));
		}
		pbd->simulateB4Collisions(deltaTime);
		cm.simulate(deltaTime);
		pbd->simulateAfterCollisions(deltaTime);
		//performFrustumCulling(frustumPlanes, sm->getActiveScene()->gameObjects);
		if (frustumTest) {
			for (int i = 0; i < sm->getActiveScene()->gameObjects.size(); i++) {
				if (sm->getActiveScene()->gameObjects.at(i)->isVisible) {
					std::cout << sm->getActiveScene()->gameObjects.at(i)->name << " " << std::endl;
					for (int j = 0; j < sm->getActiveScene()->gameObjects.at(i)->children.size(); j++) {
						std::cout << sm->getActiveScene()->gameObjects.at(i)->children.at(j)->name << " " << std::endl;
					}
				}
			}
		}
		
		//skydome->getTransform()->localPosition = camera->transform->localPosition;
		if (sm->getActiveScene()->findByName("skydome")) {
			sm->getActiveScene()->findByName("skydome")->getTransform()->localPosition = camera->transform->localPosition;
		}
		sm->getActiveScene()->Draw(V, P);

		//std::cout<<sm->getActiveScene()->findByName("player")->getVertex(1)->verticies.x;

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

		while (input->IsMove()) {
			glm::vec2 dpos = input->getPosMouse();
			if (glfwGetInputMode(window, GLFW_CURSOR) != 212993) {
				camera->updateCamera(dpos);
			}
		}
		if (buttonPressed) {
			for (auto go : sm->getActiveScene()->gameObjects) {
					delete go;
			}
			for (auto sect : cm.sections) {
				sect->staticObjects.clear();
				sect->objects.clear();
			}
			pbd->objects.clear();
			sm->getActiveScene()->gameObjects.clear();
			enemyManager->enemies.clear();
			pathfinder->trees.clear();
			spawnedEnemies = 0;

			for (int i = 0; i < sectorsPom; i++) {
				for (int j = 0; j < sectorsPom; j++) {
					GameObject* planeSector = new GameObject("sector"+std::to_string((i+1)*(j+1)));
					planeSector->localTransform->localScale = glm::vec3(2.f, 2.f, 2.f);
					planeSector->addModelComponent(planeSectormodel);
					planeSector->localTransform->localPosition = glm::vec3(i * 20* planeSector->localTransform->localScale.x, 0.f, j * 20*planeSector->localTransform->localScale.z);
					int treeCount = losujLiczbe(a, b);
					for (int k = 0; k < treeCount; k++) {
							int treeX = losujLiczbe2()* planeSector->localTransform->localScale.x;
							int treeZ = losujLiczbe2()* planeSector->localTransform->localScale.z;
							/*for (int m = 0; m < planeSector->children.size(); m++) {
								if (!checkLocations(planeSector->children.at(m)->localTransform->getLocalPosition().x, (planeSector->children.at(m)->localTransform->getLocalPosition().z), planeSector->localTransform->localPosition.x + treeX, planeSector->localTransform->localPosition.z + treeZ, 8))
								{
									treeX = losujLiczbe2() * planeSector->localTransform->localScale.x; treeZ = losujLiczbe2() * planeSector->localTransform->localScale.z;
								}
							}*/
						Tree* tree = new Tree("tree_"+std::to_string(k), 100.0f);
						tree->addModelComponent(treetrunk);
						tree->localTransform->localPosition.x = planeSector->localTransform->localPosition.x +treeX ;
						tree->localTransform->localPosition.z = planeSector->localTransform->localPosition.z +treeZ;
						tree->addColider(1);
						GameObject* log = new GameObject("log"+std::to_string(k));
						log->addModelComponent(treelog);
						log->localTransform->localPosition.x = planeSector->localTransform->localPosition.x +treeX;
						log->localTransform->localPosition.z = planeSector->localTransform->localPosition.z + treeZ;
						log->localTransform->localPosition.y = planeSector->localTransform->localPosition.y+ 1.2;
						log->localTransform->localRotation.y = losujLiczbe(0, 360);
						log->addColider(1);
						tree->addChild(log);
						planeSector->addChild(tree);
						int branchCount = losujLiczbe(3, 8);
						for (int m = 0; m < branchCount; m++) {
							GameObject* branch = new GameObject("branch" + std::to_string(m));
							branch->addModelComponent(treebranch1);
							branch->localTransform->localPosition.x = planeSector->localTransform->localPosition.x + treeX;
							branch->localTransform->localPosition.y = float(losujLiczbe((m * 13 / branchCount)+5, ((m + 1) * 13 / branchCount)+5));
							branch->localTransform->localScale.x = 12 / branch->localTransform->localPosition.y;
							branch->localTransform->localScale.y = 12 / branch->localTransform->localPosition.y;
							branch->localTransform->localScale.z = 12 / branch->localTransform->localPosition.y;
							branch->localTransform->localPosition.z = planeSector->localTransform->localPosition.z + treeZ;
							branch->localTransform->localRotation.y = float(losujLiczbe(m * 360 / branchCount, (m + 1) * 360 / branchCount));
							//branch->localTransform->localRotation.x =losujLiczbe(10,45) ;
							//branch->localTransform->localRotation.z = losujLiczbe(0, 360);
							branch->addColider(1);
							glm::vec3 bpos = branch->localTransform->localPosition;
							glm::vec3 bscale = branch->localTransform->localScale;
							branch->boundingBox = new BoundingBox(glm::vec3(bpos.x * 1.1f, bpos.y ,bpos.z * 1.1f),
								glm::vec3(bpos.x * 1.1f, bpos.y, (bpos.z +4.5f) * 1.1f), 0.0f, true);
							//std::cout << branch->localTransform->localRotation.x << std::endl;
							log->addChild(branch);
						}
					}

					sm->activeScene->addObject(planeSector);
					for(auto ch : planeSector->children)
					{
						pathfinder->trees.push_back(std::make_pair((i + 1)* j, ch->getAsActualType<Tree>()));
					}
					pathfinder->sortTrees();
				}
				loaded = true;
				spawnerTime = 0;
			}
			
				//sm->saveScene("first");
			buttonPressed = false;
			for (int i = 0; i < sm->getActiveScene()->gameObjects.size(); i++) {
				for (auto go : sm->getActiveScene()->gameObjects.at(i)->children)
				{
					cm.addStaticObject(go);
					cm.addStaticObject(go->children.at(0));
					for (auto ch : go->children.at(0)->children)
					{
						cm.addStaticObject(ch);
					}
				}
				sm->getActiveScene()->gameObjects.at(i)->lightSetting(camera->transform->getLocalPosition(), lightPos, glm::vec3(1.0f));

			}
			
			//GameObject* player = new GameObject("player");
			//player->addModelComponent(box2model);
			//player->modelComponent->SetShader(phongShader);
			//player->localTransform->localPosition = glm::vec3(0.f);
			//player->localTransform->localScale = glm::vec3(0.1f);
			//player->addColider();
			//sm->getActiveScene()->addObject(anim);
				//sm->saveScene("first");
			buttonPressed = false;
			GameObject* HPcount = new GameObject("HPcount");
			UI* ui = new UI(writing);
			Shader* UIShader = new Shader("../../../../src/shaders/font.vert", "../../../../src/shaders/font.frag");
			ui->addShader(UIShader);
			HPcount->localTransform->localPosition = glm::vec3(25.f);
			ui->setText("Ala ma kota");
			HPcount->uiComponent = ui;

			for (int i = 0; i < sm->getActiveScene()->gameObjects.size(); i++) {
				for (auto go : sm->getActiveScene()->gameObjects.at(i)->children)
				{
					cm.addStaticObject(go);
					cm.addStaticObject(go->children.at(0));
					for (auto ch : go->children.at(0)->children)
					{
						cm.addStaticObject(ch);
					}
				}
				sm->getActiveScene()->gameObjects.at(i)->lightSetting(camera->transform->getLocalPosition(), lightPos, glm::vec3(1.0f));
				sm->getActiveScene()->gameObjects.at(i)->shadowSetting(lightSpaceMatrix);
			}
			
			GameObject* anim = new GameObject("player");
			//animodel->SetShader(shaderAnimation);
			anim->addModelComponent(animodel);
			anim->addAnimation(const_cast<char*>("../../../../res/animations/Walking.dae"), "walking", 1.2f);
			anim->addAnimation(const_cast<char*>("../../../../res/animations/Briefcase Idle.dae"), "standing", 1.f);
			anim->addAnimation(const_cast<char*>("../../../../res/animations/Jumping Up.dae"), "jumping up", 0.9f);
			anim->addAnimation(const_cast<char*>("../../../../res/animations/Jumping Down.dae"), "jumping down", 0.2f);
			anim->addAnimation(const_cast<char*>("../../../../res/animations/Punching.dae"), "attack1", 1.5f);
			anim->addAnimation(const_cast<char*>("../../../../res/animations/Punching2.dae"), "attack2", 1.5f);
			anim->addAnimation(const_cast<char*>("../../../../res/animations/Standing Melee Punch.dae"), "attack3", 1.5f);
			anim->addAnimation(const_cast<char*>("../../../../res/animations/Dodge.dae"), "dodge", 1.f);
			anim->addAnimation(const_cast<char*>("../../../../res/animations/Climbing Up Wall.dae"), "climbing up", 1.f);

			anim->capsuleCollider = new CapsuleCollider(anim->localTransform->localPosition, 0.5f, 2.0f, 1.0f, true);
			pbd->objects.push_back(anim);
			if (sm->getActiveScene()->findByName("player") == nullptr) {
				sm->getActiveScene()->addObject(anim);
				cm.addObject(anim);
			}
			sm->getActiveScene()->addObject(anim);
			sm->getActiveScene()->findByName("player")->Move(glm::vec3(0.f, 2.f, 0.f));
			sm->getActiveScene()->findByName("player")->getTransform()->localScale = glm::vec3(2.f, 2.f, 2.f);
			enemyManager->player = anim;

			/*GameObject* anim2 = new GameObject("player2");
			anim2->addModelComponent(animodel);
			anim2->addAnimation(const_cast<char*>("../../../../res/animations/Walking.dae"), "walking", 1.f);
			anim2->addAnimation(const_cast<char*>("../../../../res/animations/Briefcase Idle.dae"), "standing", 1.f);
			anim2->addAnimation(const_cast<char*>("../../../../res/animations/Jumping Up.dae"), "jumping up", 0.9f);
			anim2->addAnimation(const_cast<char*>("../../../../res/animations/Jumping Down.dae"), "jumping down", 0.2f);
			anim2->addAnimation(const_cast<char*>("../../../../res/animations/Punching.dae"), "attack1", 1.f);
			anim2->addAnimation(const_cast<char*>("../../../../res/animations/Dodge.dae"), "dodge", 1.f);

			anim2->capsuleCollider = new CapsuleCollider(anim2->localTransform->localPosition, 0.5f, 2.0f, 1.0f, true);
			pbd->objects.push_back(anim2);
			if (sm->getActiveScene()->findByName("player2") == nullptr) {
				sm->getActiveScene()->addObject(anim2);
				cm.addObject(anim2);
			}
			sm->getActiveScene()->addObject(anim2);
			sm->getActiveScene()->findByName("player2")->Move(glm::vec3(0.f, 2.f, 0.f));
			sm->getActiveScene()->findByName("player2")->getTransform()->localScale = glm::vec3(2.f, 2.f, 2.f);*/

			GameObject* skydome = new GameObject("skydome");
			skydome->addModelComponent(skydomeModel);
			skydome->getTransform()->localScale = glm::vec3(100.f);
			sm->getActiveScene()->addObject(skydome);
			sm->getActiveScene()->addObject(HPcount);
			sm->getActiveScene()->findByName("skydome")->timeSetting(time / 7, glm::vec2(10, 10));
		}
		while (input->IsKeobarodAction(window)) {
			input->getMessage(key, action);

			if (gameMode.getMode() == GameMode::Debug) {
				input->getPressKey();
				if (key == GLFW_KEY_W && action == GLFW_REPEAT) {
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
		for (auto object : sm->getActiveScene()->gameObjects) {
			for (auto ch : object->children) {
				if (glm::distance(ch->getTransform()->localPosition, camera->transform->localPosition) > 100.f) {
					if (ch->modelComponent != box2model) {
						ch->modelComponent = box2model;
						ch->children.at(0)->modelComponent = nullptr;
						for (auto branch : ch->children.at(0)->children)
						{
							branch->modelComponent = nullptr;
						}
					}
				}
				else {
					if (ch->modelComponent != treetrunk)
					{
						ch->modelComponent = treetrunk;
						ch->children.at(0)->modelComponent = treelog;
						for (auto branch : ch->children.at(0)->children)
						{
							branch->modelComponent = treebranch1;
						}
					}
				}
			}
		}
		int pom = 0;
		//skydome->getTransform()->localPosition = camera->transform->localPosition;
		/*if (plane->getModelComponent() != nullptr) {
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
		}*/
		if (spawnerTime > 8.f && loaded && spawnedEnemies <= maxEnemies) {
			spawnerTime = 0;
			Enemy* enemy = new Enemy("enemy" + std::to_string(spawnedEnemies),glm::vec3(5.f,5.f,0.f), glm::vec3(0.1f), glm::vec3(0.f), std::make_pair(2.0f, 14.f));
			enemy->addModelComponent(enemyModel);
			pbd->objects.push_back(enemy);
			enemy->addColider(2);
			enemy->capsuleCollider = new CapsuleCollider(enemy->capsuleCollider->center, enemy->capsuleCollider->radius * 0.8f, enemy->capsuleCollider->height, 1.0f, true);
			sm->getActiveScene()->addObject(enemy);
			cm.addObject(enemy);
			spawnedEnemies++;
			std::pair<glm::vec3, Tree*> tree = pathfinder->decideInitalDestination(enemy->sector);
			enemy->chosenTreePos = tree.first;
			enemy->chosenTree = tree.second;
			enemy->velocity = enemy->chosenTreePos - enemy->localTransform->localPosition;
			enemy->state = EnemyState::Walking;
			enemyManager->addEnemy(enemy);
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
	for (auto go : sm->getActiveScene()->gameObjects) {
		ImGui::Text(go->name.c_str());
		ImGui::Text("x: %.2f, y: %.2f, z: %.2f",go->localTransform->localPosition.x, go->localTransform->localPosition.y, go->localTransform->localPosition.z);
		
	}
	ImGui::SliderFloat("light x", &lightPos.x, -100, 100); 
	ImGui::SliderFloat("light y", &lightPos.y, -100, 100);
	ImGui::SliderFloat("light z", &lightPos.z, -100, 100);
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

bool checkLocations(float x1, float y1, float x2, float y2,float distance) {
	if (((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) > (distance * distance)) {
		return true;
	}
	return false;
}
