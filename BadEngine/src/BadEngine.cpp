#include <stdio.h>
#include<GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>
#include<fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <AL/al.h>
#include <AL/alc.h>
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
#include "../lib/SectorSelector.h"
#include "../lib/AudioManager.h"
#include "../lib/ResourceLoader.h"
#include "../lib/Frustum.h"
#include "../lib/AbilityManager.h"

bool test = false;
bool frustumTest = false;
int losujLiczbe(int a, int b);
int losujLiczbe2();
bool checkLocations(float x1, float y1,float x2,float y2,float distance);


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
int sectors = 1;
int sectorsPom = 4;
std::vector fruits = { 2,3 }; //{bananas,mango}
int a = 4;
int b = 6;
bool buttonPressed;
unsigned int maxEnemies = 15;
unsigned int spawnedEnemies = 0;
unsigned int reqEnemies = 10;
bool loaded = false;
bool playerAtention = false;
bool wycinka = false;
CollisionManager cm = CollisionManager(200, 40);
Pathfinder* pathfinder = new Pathfinder();
PBDManager* pbd = new PBDManager(10);
EnemyStateManager* enemyManager;
AbilityManager* ability;
ResourceLoader RL;
std::vector<Transform*> transformsTree;
std::vector<Transform*> transformsLog;
std::vector<Transform*> transformsBranch;
std::vector<Transform*> transformsEnemy;
std::vector<Transform*> transformsEnemyWeapon;
std::vector<Transform*> transformsTreeLow;
std::vector<Transform*> transformsLogLow;
std::vector<Transform*> transformsLeaves;
std::vector<Transform*> transformsBanana;
std::vector<Transform*> transformsMango;
GameObject* lowTree;
GameObject* lowLog;
float spawnerTime = 0;
float sectorSelectorTime = 0;
GameMode gameMode;
SectorSelector* sectorSelector;
GameObject* tutorial1;
GameObject* tutorial2;
GameObject* tutorial3;
GameObject* tutorial4;
GameObject* tutorial5;
GameObject* tutorial6;
GameObject* tutorial7;
glm::mat4 lightSpaceMatrix;
std::shared_ptr<Model> skydomeModel;
float gameTime = 0;
GameObject* playButton;
GameObject* titlescreen;
GameObject* tutorialButton;
GameObject* acknowledgmentsButton;
GameObject* backButton;
GameObject* acknowledgments;
GameObject* acknowledgments2;
GameObject* acknowledgments3;
GameObject* acknowledgments4;
GameObject* acknowledgments5;
bool Lost = false;
bool Won = false;
Shader* LogoShader;
std::array<glm::vec2, 16> sectorCenters;
GameObject* sectorpointer;
GameObject* sectorpointer2;
float radius = 300.0f;


void addAnimation(GameObject* anim, char* path, const char* name, float duration) {
	anim->addAnimation(path, name, duration);
}

void delay(int milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void generate() {
	std::cout << "przycisk generate" << std::endl;
	playButton->uiComponent->pressed = true;/*
	sectorsPom = 4;
	a = 4;
	b = 6;*/
	std::vector<int> fruitPool;
	GameObject* loading = new GameObject("loading");
	UI* loadingui = new UI(plane);
	loadingui->setTexture("res/loading.png");
	Shader* LogoShader = new Shader("res/shaders/font.vert", "res/shaders/font.frag");
	loadingui->addShader(LogoShader);
	loadingui->input = input;
	loading->uiComponent = loadingui;
	loading->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth * 0.25f, 0.0f / 2, 0.f);
	loading->localTransform->localScale = glm::vec3(10.f, 10.f, 1.f);
	glClearColor(1.0f, 1.0f, 1.0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	loading->Draw(camera->getViewMatrix(), glm::perspective(glm::radians(input->GetZoom()), static_cast<float>(windowGlobals.windowWidth) / windowGlobals.windowHeight, 1.f, 5000.f));

	glfwSwapBuffers(window);
	glfwPollEvents();
	/*for (GameObject* go : sm->getActiveScene()->gameObjects) {
		delete go;
	}*/
	sm->activeScene = new Scene("mainLoop");
	for (auto sect : cm.sections) {
		sect->staticObjects.clear();
		sect->objects.clear();
	}
	pbd->objects.clear();
	sm->getActiveScene()->gameObjects.clear();
	enemyManager->enemies.clear();
	pathfinder->trees.clear();
	spawnedEnemies = 0;
	reqEnemies = 10;
	int sectorcounter = 1;
	for (int i = 0; i < sectorsPom; i++) {
		for (int j = 0; j < sectorsPom; j++) {
			GameObject* planeSector = new GameObject("sector" + std::to_string(sectorcounter));
			planeSector->localTransform->localScale = glm::vec3(2.f, 2.f, 2.f);
			planeSector->addModelComponent(RL.planeSectormodel);
			planeSector->localTransform->localPosition = glm::vec3(i * 20 * planeSector->localTransform->localScale.x, 0.f, j * 20 * planeSector->localTransform->localScale.z);
			int treeCount = losujLiczbe(a, b);
			sectorCenters.at(sectorcounter - 1) = glm::vec2(planeSector->localTransform->localPosition.x + 20, planeSector->localTransform->localPosition.z + 20);
			for (int k = 0; k < fruits.size(); k++) {
				for (int m = 0; m < fruits.at(k); m++) {
					fruitPool.push_back(k);
				}
			}
			while (fruitPool.size() < treeCount)
			{
				fruitPool.push_back(fruits.size());
			}
			
			for (int k = 0; k < treeCount; k++) {
				int treeX = losujLiczbe2() * planeSector->localTransform->localScale.x;
				int treeZ = losujLiczbe2() * planeSector->localTransform->localScale.z;
				/*for (int m = 0; m < planeSector->children.size(); m++) {
					if (!checkLocations(planeSector->children.at(m)->localTransform->getLocalPosition().x, (planeSector->children.at(m)->localTransform->getLocalPosition().z), planeSector->localTransform->localPosition.x + treeX, planeSector->localTransform->localPosition.z + treeZ, 8))
					{
						treeX = losujLiczbe2() * planeSector->localTransform->localScale.x; treeZ = losujLiczbe2() * planeSector->localTransform->localScale.z;
					}
				}*/
				Tree* tree = new Tree("tree_" + std::to_string(k), 100.0f);
				tree->isInstanced = true;
				tree->addModelComponent(RL.treetrunk);
				tree->localTransform->localPosition.x = planeSector->localTransform->localPosition.x + treeX;
				tree->localTransform->localPosition.z = planeSector->localTransform->localPosition.z + treeZ;
				tree->addColider(1);
				GameObject* log = new GameObject("log" + std::to_string(k));
				log->isInstanced = true;
				log->addModelComponent(RL.treelog);
				log->localTransform->localPosition.x = planeSector->localTransform->localPosition.x + treeX;
				log->localTransform->localPosition.z = planeSector->localTransform->localPosition.z + treeZ;
				log->localTransform->localPosition.y = planeSector->localTransform->localPosition.y + 1.2;
				log->localTransform->localRotation.y = losujLiczbe(0, 360);
				log->addColider(1);
				tree->addChild(log);
				planeSector->addChild(tree);
				int branchCount = losujLiczbe(3, 8);
				for (int m = 0; m < branchCount; m++) {
					GameObject* branch = new GameObject("branch" + std::to_string(m));
					branch->isInstanced = true;
					branch->addModelComponent(RL.treebranch1);
					branch->localTransform->localPosition.x = planeSector->localTransform->localPosition.x + treeX;
					branch->localTransform->localPosition.y = float(losujLiczbe((m * 13 / branchCount) + 5, ((m + 1) * 13 / branchCount) + 5));
					branch->localTransform->localScale.x = 6 / branch->localTransform->localPosition.y;
					branch->localTransform->localScale.y = 6 / branch->localTransform->localPosition.y;
					branch->localTransform->localScale.z = 6 / branch->localTransform->localPosition.y;
					branch->localTransform->localPosition.z = planeSector->localTransform->localPosition.z + treeZ;
					branch->localTransform->localRotation.y = float(losujLiczbe(m * 360 / branchCount, (m + 1) * 360 / branchCount));
					//branch->localTransform->localRotation.x =losujLiczbe(10,45) ;
					//branch->localTransform->localRotation.z = losujLiczbe(0, 360);
					branch->addColider(1);
					glm::vec3 bpos = branch->localTransform->localPosition;
					glm::vec3 bscale = branch->localTransform->localScale;
					branch->boundingBox = new BoundingBox(glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(1.1f, 1.2f, 120.0f), 0.0f, true);
					branch->modelComponent.get()->boundingBox = branch->boundingBox;
					GameObject* leafs = new GameObject("leafs");
					leafs->isInstanced = true;
					leafs->localTransform->localPosition = branch->localTransform->localPosition;
					leafs->localTransform->localPosition.x += branch->localTransform->localScale.x * 12.0f * sinf(glm::radians(branch->localTransform->localRotation.y));
					leafs->localTransform->localPosition.z += branch->localTransform->localScale.x * 12.0f * cosf(glm::radians(branch->localTransform->localRotation.y));
					leafs->localTransform->localPosition.y -= 2.0f;
					leafs->localTransform->localRotation = branch->localTransform->localRotation;
					leafs->addModelComponent(RL.leafModel);
					leafs->localTransform->localScale = glm::vec3(2.f);
					branch->addChild(leafs);
					//std::cout << branch->localTransform->localRotation.x << std::endl;
					log->addChild(branch);
				}
				int fruitIndex = losujLiczbe(0, fruitPool.size() - 1);
				if (fruitPool.at(fruitIndex) != fruits.size()) {
					GameObject* fruits = new GameObject("fruit");
					int branchIndex = losujLiczbe(0, log->children.size()-1);
					fruits->localTransform->localPosition = log->children.at(branchIndex)->localTransform->localPosition;
					fruits->localTransform->localPosition.x += log->children.at(branchIndex)->localTransform->localScale.x* 4.0f* sinf(glm::radians(log->children.at(branchIndex)->localTransform->localRotation.y));
					fruits->localTransform->localPosition.z += log->children.at(branchIndex)->localTransform->localScale.x * 4.0f * cosf(glm::radians(log->children.at(branchIndex)->localTransform->localRotation.y));
					switch (fruitPool.at(fruitIndex)) {
					case 0:
						fruits->name = "FruitBanana";
						fruits->addModelComponent(RL.bananaModel);
						fruits->localTransform->localScale = glm::vec3(0.1f);
						fruits->capsuleCollider = new CapsuleCollider(fruits->localTransform->localPosition, 2.0f, 2.0f, 1.0f, true);
						fruits->capsuleCollider->isTriggerOnly = true;
						break;
					case 1:
						fruits->name = "FruitMango";
						fruits->addModelComponent(RL.mangoModel);
						fruits->localTransform->localScale = glm::vec3(2.f);
						fruits->capsuleCollider = new CapsuleCollider(fruits->localTransform->localPosition, 2.0f, 2.0f, 1.0f, true);
						fruits->capsuleCollider->isTriggerOnly = true;
						break;
					}
					fruits->isInstanced = true;
					//log->children.at(branchIndex)->addChild(fruits);
					sm->activeScene->addObject(fruits);
				}
				fruitPool.erase(fruitPool.begin()+fruitIndex);
			}

			sm->activeScene->addObject(planeSector);
			for (auto ch : planeSector->children)
			{
				pathfinder->trees.push_back(std::make_pair(sectorcounter, ch->getAsActualType<Tree>()));
			}
			sectorcounter++;
		}
	}
	GameObject* wall1 = new GameObject("wall1");
	wall1->boundingBox = new BoundingBox(glm::vec3(-8.0f, 0.0f, -8.0f), glm::vec3(152.0f, 100.0f, -8.5f), 0.0f, true);
	GameObject* wall2 = new GameObject("wall2");
	wall2->boundingBox = new BoundingBox(glm::vec3(-8.0f, 0.0f, -8.0f), glm::vec3(-8.5f, 100.0f, 152.0f), 0.0f, true);
	GameObject* wall3 = new GameObject("wall3");
	wall3->boundingBox = new BoundingBox(glm::vec3(152.0f, 0.0f, -8.0f), glm::vec3(151.5f, 100.0f, 152.0f), 0.0f, true);
	GameObject* wall4 = new GameObject("wall4");
	wall4->boundingBox = new BoundingBox(glm::vec3(-8.0f, 0.0f, 152.0f),glm::vec3(152.0f, 100.0f, 151.5f), 0.0f, true);
	GameObject* planeWall1 = new GameObject("planeWall1");
	planeWall1->addModelComponent(RL.planeModel);
	planeWall1->getTransform()->localRotation.y = 180.0f;
	planeWall1->getTransform()->localRotation.x = -90.0f;
	planeWall1->getTransform()->localScale = glm::vec3(10.0f, 4.0f, 1.0f);
	planeWall1->getTransform()->localPosition = glm::vec3(100.0f, 0.0f, -8.0f);
	GameObject* planeWall2 = new GameObject("planeWall2");
	planeWall2->addModelComponent(RL.planeModel);
	planeWall2->getTransform()->localRotation.y = 270.0f;
	planeWall2->getTransform()->localRotation.x = -90.f;
	planeWall2->getTransform()->localScale = glm::vec3(10.0f, 4.0f, 1.0f);
	planeWall2->getTransform()->localPosition = glm::vec3(-8.0f, 0.0f, 0.0f);
	GameObject* planeWall3 = new GameObject("planeWall3");
	planeWall3->addModelComponent(RL.planeModel);
	planeWall3->getTransform()->localRotation.y = 0.0f;
	planeWall3->getTransform()->localRotation.x = -90.0f;
	planeWall3->getTransform()->localScale = glm::vec3(10.0f, 4.0f, 1.0f);
	planeWall3->getTransform()->localPosition = glm::vec3(40.0f, 0.0f, 152.0f);
	GameObject* planeWall4 = new GameObject("planeWall4");
	planeWall4->addModelComponent(RL.planeModel);
	planeWall4->getTransform()->localRotation.y = 90.0f;
	planeWall4->getTransform()->localRotation.x = -90.0f;
	planeWall4->getTransform()->localScale = glm::vec3(10.0f, 4.0f, 1.0f);
	planeWall4->getTransform()->localPosition = glm::vec3(148.0f, 0.0f, 112.0f);
	sm->getActiveScene()->addObject(wall1);
	sm->getActiveScene()->addObject(wall2);
	sm->getActiveScene()->addObject(wall3);
	sm->getActiveScene()->addObject(wall4);
	sm->getActiveScene()->addObject(planeWall1);
	sm->getActiveScene()->addObject(planeWall2);
	sm->getActiveScene()->addObject(planeWall3);
	sm->getActiveScene()->addObject(planeWall4);
	transformsBranch.clear();
	transformsLog.clear();
	transformsTree.clear();
	transformsLeaves.clear();
	transformsBanana.clear();
	transformsMango.clear();
	for (auto tree : pathfinder->trees) {
		transformsTree.push_back(tree.second->getTransform());
		transformsLog.push_back(tree.second->children.at(0)->getTransform());
		for (auto ch : tree.second->children.at(0)->children)
		{
			transformsBranch.push_back(ch->getTransform());
			transformsLeaves.push_back(ch->children.at(0)->getTransform());
		}
	}
	if (pathfinder->trees.size() > 0) {
		pathfinder->trees.at(0).second->getModelComponent().get()->getFirstMesh()->initInstances(transformsTree);
		pathfinder->trees.at(0).second->children.at(0)->getModelComponent().get()->getFirstMesh()->initInstances(transformsLog);
		pathfinder->trees.at(0).second->children.at(0)->children.at(0)->getModelComponent().get()->getFirstMesh()->initInstances(transformsBranch);
		pathfinder->trees.at(0).second->children.at(0)->children.at(0)->children.at(0)->getModelComponent().get()->getFirstMesh()->initInstances(transformsLeaves);
		pathfinder->sortTrees();
	}
		
	if (sectorSelector == nullptr) {
		sectorSelector = new SectorSelector(&sectorsPom);
	}
	sectorSelector->selectSector(1);

	//sm->saveScene("first");
	buttonPressed = false;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
		if (sm->getActiveScene()->gameObjects.at(i)->name.starts_with("wall") || sm->getActiveScene()->gameObjects.at(i)->name.starts_with("Fruit")) {
			cm.addStaticObject(sm->getActiveScene()->gameObjects.at(i));
		}
	}

	buttonPressed = false;
	gameMode.setMode(GameMode::Game);
	GameObject* Button = new GameObject("Button");
	UI* ui = new UI(button);
	Shader* UIShader = new Shader("res/shaders/font.vert", "res/shaders/font.frag");
	ui->addShader(UIShader);
	ui->setTexture("res/chmury.png");
	Button->localTransform->localPosition = glm::vec3(25.f);
	ui->input = input;
	//ui->onClick = std::bind(&typKlasy::nazwafunkcji,&stworzonaKalsa);
	ui->setText("Ala ma kota");
	Button->uiComponent = ui;

	GameObject* HPcount = new GameObject("HPcount");
	UI* ui2 = new UI(plane);
	ui2->addShader(UIShader);
	ui2->setTexture("res/serce5.png");
	HPcount->localTransform->localPosition = glm::vec3(0, windowGlobals.windowHeight - 100.0f, 0.f);
	HPcount->uiComponent = ui2;

	GameObject* dashTime = new GameObject("DashTime");
	UI* uiDash = new UI(writing);
	uiDash->addShader(UIShader);
	uiDash->setText("Dash: 0");
	dashTime->localTransform->localPosition = glm::vec3(300.f, 50.f, 0.f);
	dashTime->uiComponent = uiDash;

	GameObject* bananaPeelCount = new GameObject("bananaPeel");
	UI* uiBananaPeelCount = new UI(writing);
	uiBananaPeelCount->addShader(UIShader);
	uiBananaPeelCount->setText("Banany: 1");
	bananaPeelCount->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth - 950.0f, 50.f, 0.f);
	bananaPeelCount->uiComponent = uiBananaPeelCount;

	GameObject* treeAttackTime = new GameObject("TreeAttack");
	UI* uiTreeAttack = new UI(writing);
	uiTreeAttack->addShader(UIShader);
	uiTreeAttack->setText("Atak z drzewa: 0");
	treeAttackTime->localTransform->localPosition = glm::vec3(550.f, 50.f, 0.f);
	treeAttackTime->uiComponent = uiTreeAttack;


	GameObject* enemyCount = new GameObject("Enemycount");
	UI* uiCount = new UI(writing);
	uiCount->addShader(UIShader);
	uiCount->setText("Pozostali drwale : " + std::to_string(reqEnemies));
	enemyCount->localTransform->localPosition = glm::vec3(0, windowGlobals.windowHeight - 300.0f, 0.f);
	enemyCount->uiComponent = uiCount;

	GameObject* EnemyTime = new GameObject("EnemyTime");
	UI* uiEnemyTime = new UI(writing);
	uiEnemyTime->addShader(UIShader);
	uiEnemyTime->setText("Czas do wycinki sektora : " + std::to_string(sectorSelectorTime));
	EnemyTime->localTransform->localPosition = glm::vec3(0, windowGlobals.windowHeight - 200.0f, 0.f);
	EnemyTime->uiComponent = uiEnemyTime;

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

	ability->ClearAllAbilities();
	ability->AddAbility("tree attack", 11.f);
	ability->AddAbility("dash", 11.f);

	GameObject* anim = new GameObject("player");
	//animodel->SetShader(shaderAnimation);
	anim->addModelComponent(RL.animodel);
	//std::vector<std::thread> threads;

	// Dodawanie animacji w osobnych 
	anim->addAnimation(const_cast<char*>("res/animations/Monkey/monkey_walk.dae"), "walking", 1.f);
	anim->addAnimation(const_cast<char*>("res/animations/Monkey/Monkey_Hit1.dae"), "attack3", 1.f);
	anim->addAnimation(const_cast<char*>("res/animations/Monkey/Monkey_Hit2.dae"), "attack1", 1.f);
	anim->addAnimation(const_cast<char*>("res/animations/Monkey/Monkey_hit.dae"), "attack2", 1.f);
	anim->addAnimation(const_cast<char*>("res/animations/Monkey/monkey_jump_up.dae"), "jumping up", 1.f);
	anim->addAnimation(const_cast<char*>("res/animations/Monkey/monkey_jump_down.dae"), "jumping down", 0.2f);
	anim->addAnimation(const_cast<char*>("res/animations/Monkey/monkry_climb.dae"), "climbing up", 1.f);
	anim->addAnimation(const_cast<char*>("res/animations/Monkey/monkey_climb_down.dae"), "climbing down", 1.f);
	anim->addAnimation(const_cast<char*>("res/animations/Monkey/Monkey_TreeKick.dae"), "tree attack", 1.f);
	anim->addAnimation(const_cast<char*>("res/animations/Monkey/monkey_run.dae"), "sprint", 2.f);
	anim->addAnimation(const_cast<char*>("res/animations/Monkey/Monkey_Dodge.dae"), "dodge", 1.f);
	anim->addAnimation(const_cast<char*>("res/animations/Monkey/Monkey_Dash.dae"), "dash", 1.f);
	anim->addAnimation(const_cast<char*>("res/animations/Monkey/monkey_idle.dae"), "standing", 1.f);

	//threads.push_back(std::thread(addAnimation, anim, const_cast<char*>("res/animations/Monkey/monkey_idle.dae"), "standing", 1.f));
	//threads.push_back(std::thread(addAnimation, anim, const_cast<char*>("res/animations/Monkey/monkey_walk.dae"), "walking", 1.4f));
	//threads.push_back(std::thread(addAnimation, anim, const_cast<char*>("res/animations/Monkey/monkey_jump_up.dae"), "jumping up", 0.9f));
	//threads.push_back(std::thread(addAnimation, anim, const_cast<char*>("res/animations/Monkey/monkey_jump_down.dae"), "jumping down", 0.2f));
	//threads.push_back(std::thread(addAnimation, anim, const_cast<char*>("res/animations/Monkey/Monkey_Hit1.dae"), "attack1", 1.5f));
	////threads.push_back(std::thread(addAnimation, anim, const_cast<char*>("res/animations/Monkey/Monkey_Hit2.dae"), "attack2", 1.5f));
	////threads.push_back(std::thread(addAnimation, anim, const_cast<char*>("res/animations/Monkey/Monkey_Hit3.dae"), "attack3", 1.5f));
	////threads.push_back(std::thread(addAnimation, anim, const_cast<char*>("res/animations/Monkey_Dodge.dae"), "dodge", 1.f));
	//threads.push_back(std::thread(addAnimation, anim, const_cast<char*>("res/animations/Monkey/monkry_climb.dae"), "climbing up", 1.3f));
	//threads.push_back(std::thread(addAnimation, anim, const_cast<char*>("res/animations/Monkey/monkey_climb_down.dae"), "climbing down", 1.3f));
	//threads.push_back(std::thread(addAnimation, anim, const_cast<char*>("res/animations/Monkey/Monkey_TreeKick.dae"), "tree attack", 0.7f));
	//threads.push_back(std::thread(addAnimation, anim, const_cast<char*>("res/animations/Monkey/monkey_run.dae"), "sprint", 1.5f));

	// Łączenie wątków
	//for (auto& thread : threads) {
	//	thread.join();
	//}

	Enemy* basicEnemy = new Enemy("basicEnemy", glm::vec3(2.0f, 2.0f, 3.0f), glm::vec3(120.f), glm::vec3(0.f), std::make_pair(2.0f, 6.f));
	basicEnemy->addModelComponent(RL.animationEnemyModel);
	basicEnemy->addAnimation(const_cast<char*>("res/animations/Lumberjack/Lumberjack_Idle.dae"), "idle", 1.f);
	basicEnemy->addAnimation(const_cast<char*>("res/animations/Lumberjack/Lumberjack_Walking.dae"), "walking", 1.f);
	basicEnemy->addAnimation(const_cast<char*>("res/animations/Lumberjack/Lumberjack_TreeHit.dae"), "hit", 1.f);
	basicEnemy->addAnimation(const_cast<char*>("res/animations/Lumberjack/Lumberjack_Death.dae"), "death", 1.f);

	//basicEnemy->getTransform()->localScale = glm::vec3(120.f);
	//basicEnemy->getTransform()->localPosition = glm::vec3(2.0f, 2.0f, 3.0f);

	sm->getActiveScene()->addObject(basicEnemy);

	//Tutaj działa
	Enemy* basicEnemy2 = new Enemy("basicEnemy2", glm::vec3(2.0f, 2.0f, 0.0f), glm::vec3(120.f), glm::vec3(0.f), std::make_pair(2.0f, 6.f));
	basicEnemy2->addModelComponent(RL.animationEnemyModel);
	basicEnemy2->animPlayer = basicEnemy->animPlayer->clone(RL.animationEnemyModel.get());
	//basicEnemy2->animPlayer = basicEnemy->animPlayer;
	basicEnemy2->getAnimateBody()->setActiveAnimation("hit");
	sm->getActiveScene()->addObject(basicEnemy2);


	glm::vec3 capsuleCenter = anim->getTransform()->getLocalPosition();
	capsuleCenter.y += 0.0025f;
	anim->capsuleCollider = new CapsuleCollider(capsuleCenter, 0.03f, 0.005f, 1.0f, true);
	anim->getTransform()->localScale = glm::vec3(40.f);
	anim->getTransform()->localPosition = glm::vec3(3.0f, 0.0f, 3.0f);
	pm->changeState(PlayerState::walking);

	
	pbd->objects.push_back(anim);
	if (sm->getActiveScene()->findByName("player") == nullptr) {
		sm->getActiveScene()->addObject(anim);
		cm.addObject(anim);
	}
	sm->getActiveScene()->addObject(anim);
	//anim->Move(glm::vec3(3.0f, 2.0f, 3.0f));

	//pm->setGroundPosition(anim->getTransform()->getLocalPosition().y);
	enemyManager->player = anim;
	anim->hp = 5;
	GameObject* fist = new GameObject("fist");
	anim->addChild(fist);
	fist->capsuleCollider = new CapsuleCollider(fist->localTransform->localPosition, 3.0f, 3.0f, 1.0f, true);
	fist->capsuleCollider->isTriggerOnly = true;
	fist->active = false;


	GameObject* skydome = new GameObject("skydome");
	skydome->addModelComponent(skydomeModel);
	skydome->getTransform()->localScale = glm::vec3(100.f);
	sm->getActiveScene()->addObject(skydome);
	sm->getActiveScene()->findByName("skydome")->timeSetting(gameTime / 7, glm::vec2(10, 10));

	pm->bananaModel = RL.bananaModel2;
	//GameObject* bananaPeelObj = new GameObject("bananaPeelObj");
	//RL.bananaModel2->SetShader(RL.diffuseShader);
	//bananaPeelObj->addModelComponent(RL.bananaModel2);
	//bananaPeelObj->getTransform()->localPosition = glm::vec3(0.f, -10.f, -1.f);
	//bananaPeelObj->getTransform()->localScale = glm::vec3(0.1f);
	//sm->getActiveScene()->addObject(bananaPeelObj);


	//sm->getActiveScene()->addObject(Button);
	sm->getActiveScene()->addObject(HPcount);
	sm->getActiveScene()->addObject(enemyCount);
	sm->getActiveScene()->addObject(dashTime);
	sm->getActiveScene()->addObject(treeAttackTime);
	sm->getActiveScene()->addObject(bananaPeelCount);
	sm->getActiveScene()->addObject(EnemyTime);

	sectorpointer = new GameObject("sectorpointer");
	UI* sectorpointerui = new UI(plane);
	sectorpointerui->addShader(UIShader);
	sectorpointerui->setTexture("res/strzalka.png");
	sectorpointer->uiComponent = sectorpointerui;
	sectorpointer->localTransform->localScale = glm::vec3(1.2f);
	sm->getActiveScene()->addObject(sectorpointer);

	sectorpointer2 = new GameObject("sectorpointer2");
	UI* sectorpointerui2 = new UI(plane);
	sectorpointerui2->addShader(UIShader);
	sectorpointerui2->setTexture("res/strzalka.png");
	sectorpointer2->uiComponent = sectorpointerui2;
	sectorpointer2->localTransform->localScale = glm::vec3(1.2f);
	sectorpointer2->localTransform->localPosition = glm::vec3(500.0f, 0.0f, 0.0f);
	sm->getActiveScene()->addObject(sectorpointer2);

	sectorpointer->isVisible = false;
	sectorpointer2->isVisible = false;

	loaded = true;
	spawnerTime = 0;
	sectorSelectorTime = 0;
	std::cout << sectorsPom << std::endl;
	std::cout << pathfinder->trees.size() << std::endl;
	//delete playButton;

	if (input->checkAnyKey()) {
		input->getPressKey();
	}
}

void showTutorial() {
	sm->getActiveScene()->gameObjects.clear();
	sm->getActiveScene()->addObject(titlescreen);
	sm->getActiveScene()->addObject(playButton);
	sm->getActiveScene()->addObject(tutorial1);
	sm->getActiveScene()->addObject(tutorial2);
	sm->getActiveScene()->addObject(tutorial3);
	sm->getActiveScene()->addObject(tutorial4);
	sm->getActiveScene()->addObject(tutorial5);
	sm->getActiveScene()->addObject(tutorial6);
	sm->getActiveScene()->addObject(tutorial7);
	sm->getActiveScene()->addObject(backButton);
	playButton->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f, 300.0f, 0.0f);
	backButton->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f, 200.0f, 0.0f);
}

void showMain() {
	sm->getActiveScene()->gameObjects.clear();
	sm->getActiveScene()->addObject(titlescreen);
	sm->getActiveScene()->addObject(playButton);
	sm->getActiveScene()->addObject(tutorialButton);
	sm->getActiveScene()->addObject(acknowledgmentsButton);
	playButton->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f - 200.0f, windowGlobals.windowHeight * 0.5f, 0.0f);
	delay(100);
}

void showAcknowledgments() {
	sm->getActiveScene()->gameObjects.clear();
	sm->getActiveScene()->addObject(titlescreen);

	acknowledgments = new GameObject("acknowledgments");
	UI* acknowledgmentsui = new UI(writing);
	acknowledgmentsui->addShader(LogoShader);
	acknowledgmentsui->setText("Splash screen na podstawie obrazu ze zrodla");
	acknowledgments->localTransform->localPosition = glm::vec3(100.0f, windowGlobals.windowHeight - 100.0f, 0.0f);
	acknowledgments->uiComponent = acknowledgmentsui;
	sm->getActiveScene()->addObject(acknowledgments);

	acknowledgments2 = new GameObject("acknowledgments");
	UI* acknowledgmentsui2 = new UI(writing);
	acknowledgmentsui2->addShader(LogoShader);
	acknowledgmentsui2->setText("www.aipromptsdirectory.com/wp-content/uploads/2023/11/");
	acknowledgments2->localTransform->localPosition = glm::vec3(100.0f, windowGlobals.windowHeight - 150.0f, 0.0f);
	acknowledgments2->uiComponent = acknowledgmentsui2;
	sm->getActiveScene()->addObject(acknowledgments2);

	acknowledgments3 = new GameObject("acknowledgments");
	UI* acknowledgmentsui3 = new UI(writing);
	acknowledgmentsui3->addShader(LogoShader);
	acknowledgmentsui3->setText("hard_drive_white_background_simple_colored_sketch_-_7c0b4171-90ce");
	acknowledgments3->localTransform->localPosition = glm::vec3(70.0f, windowGlobals.windowHeight - 200.0f, 0.0f);
	acknowledgments3->uiComponent = acknowledgmentsui3;
	sm->getActiveScene()->addObject(acknowledgments3);

	acknowledgments4 = new GameObject("acknowledgments");
	UI* acknowledgmentsui4 = new UI(writing);
	acknowledgmentsui4->addShader(LogoShader);
	acknowledgmentsui4->setText("-4a92-8351-cb652420715b_0.webp");
	acknowledgments4->localTransform->localPosition = glm::vec3(100.0f, windowGlobals.windowHeight - 250.0f, 0.0f);
	acknowledgments4->uiComponent = acknowledgmentsui4;
	sm->getActiveScene()->addObject(acknowledgments4);

	acknowledgments5 = new GameObject("acknowledgments");
	UI* acknowledgmentsui5 = new UI(writing);
	acknowledgmentsui5->addShader(LogoShader);
	acknowledgmentsui5->setText("tlo ekranu głownego wygenerowano korzystajac z deepai.org");
	acknowledgments5->localTransform->localPosition = glm::vec3(20.0f, windowGlobals.windowHeight - 300.0f, 0.0f);
	acknowledgments5->uiComponent = acknowledgmentsui5;
	//sm->getActiveScene()->addObject(acknowledgments5);
	sm->getActiveScene()->addObject(backButton);
}

void Start() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	windowGlobals.windowHeight = mode->height;
	windowGlobals.windowWidth = mode->width;

	//window = glfwCreateWindow(windowGlobals.windowWidth, windowGlobals.windowHeight, "Primal Guardian", monitor, nullptr);
	window = glfwCreateWindow(windowGlobals.windowWidth, windowGlobals.windowHeight, "Primal Guardian", NULL, nullptr);
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
	ability = new AbilityManager(tm);
	pm = new PlayerMovement(sm, input, camera, tm, ability);
}

bool CheckFrustumCollision(const std::array<Plane, 6>& planes, BoundingBox* box) {
	for (const auto& plane : planes) {
		glm::vec3 positiveVertex = box->min;
		if (plane.normal.x >= 0) positiveVertex.x = box->max.x;
		if (plane.normal.y >= 0) positiveVertex.y = box->max.y;
		if (plane.normal.z >= 0) positiveVertex.z = box->max.z;

		if (glm::dot(plane.normal, positiveVertex) + plane.distance < 0) {
			return false;
		}
	}
	return true;
}

void performFrustumCulling(const std::array<Plane, 6>& planes, std::vector<Section*> sections) {
	ZoneTransientN(zoneName, "performFrustumCulling", true);
	for (auto section : sections) {
		bool isVisible = CheckFrustumCollision(planes, section->bounds);

		for (auto obj : section->objects) {
			obj->isVisible = isVisible;
		}
		for (auto obj : section->staticObjects) {
			obj->isVisible = isVisible;
			for (auto ch : obj->children) {
				ch->isVisible = isVisible;
			}
		}
	}
}

int main() {
	Start();
	std::srand(static_cast<unsigned int>(std::time(nullptr)));


	AudioManager* audioManager = new AudioManager();
	audioManager->loadSound("jungle_music", "res/media/jungle_music.wav");
	audioManager->playSound("jungle_music",true);
	//audioManager->loadSound("test", "res/media/test.wav",true);
	//audioManager->setSoundPosition("test", 10, 0, 0);


	GameObject* logo = new GameObject("logo");
	UI* logoui = new UI(plane);
	logoui->setTexture("res/logo.png");
	LogoShader = new Shader("res/shaders/font.vert", "res/shaders/font.frag");
	logoui->addShader(LogoShader);
	logoui->input = input;
	logo->uiComponent = logoui;
	logo->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth *0.25f, 0.0f / 2, 0.f);
	logo->localTransform->localScale = glm::vec3(10.f, 10.f, 1.f);
	glClearColor(1.0f, 1.0f, 1.0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	logo->Draw(camera->getViewMatrix(), glm::perspective(glm::radians(input->GetZoom()), static_cast<float>(windowGlobals.windowWidth) / windowGlobals.windowHeight, 1.f, 5000.f));
	
	glfwSwapBuffers(window);
	glfwPollEvents();

	RL.load();

	GameObject* anim = new GameObject("player");
	RL.animationEnemyModel->SetShader(RL.shaderAnimation);
	RL.animodel ->SetShader(RL.shaderAnimation);
	//RL.animodel->SetOutlineShader(RL.outlineShader);
	anim->addModelComponent(RL.animodel);
	anim->addAnimation(const_cast<char*>("res/animations/monkey_walk.dae"), "walking", 1.4f);
	anim->addAnimation(const_cast<char*>("res/animations/monkey_idle.dae"), "standing", 1.f);
	//anim->addAnimation(const_cast<char*>("res/animations/Walking.dae"), "walking", 1.f);
	//anim->addAnimation(const_cast<char*>("res/animations/Briefcase Idle.dae"), "standing", 1.f);
	//anim->addAnimation(const_cast<char*>("res/animations/Jumping Up.dae"), "jumping up", 0.9f);
	//anim->addAnimation(const_cast<char*>("res/animations/Jumping Down.dae"), "jumping down", 0.2f);
	//anim->addAnimation(const_cast<char*>("res/animations/Punching.dae"), "attack1", 1.f);
	//anim->addAnimation(const_cast<char*>("res/animations/Dodge.dae"), "dodge", 1.f);
	sm->getActiveScene()->addObject(anim);
	RL.enemyModel->SetShader(RL.phongInstancedShader);
	RL.enemyModel.get()->AddTexture("res/textures/Lumberjack_BaseColor.png", "diffuseMap");
	RL.enemyModel.get()->AddTexture("res/textures/Lumberjack_normal.png", "normalMap");

	RL.animationEnemyModel.get()->AddTexture("res/textures/Lumberjack_BaseColor.png", "diffuseMap");
	
	GameObject* outlineObj = new GameObject("outline");

	RL.bananaModel2->SetShader(RL.diffuseShader);
	RL.bananaModel2->AddTexture("res/textures/Banana.png", "diffuseMap");

	RL.bananaModel->AddTexture("res/textures/Banana.png", "diffuseMap");
	RL.bananaModel->SetShader(RL.diffuseInstancedShader);

	RL.outlinemodel->SetShader(RL.shaderTree);
	RL.outlinemodel->SetOutlineShader(RL.outlineShader);
	RL.outlinemodel->SetFillingShader(RL.fillingShader);

	outlineObj->addModelComponent(RL.outlinemodel);
	sm->getActiveScene()->addObject(outlineObj);

	sm->getActiveScene()->findByName("outline")->getTransform()->localPosition = glm::vec3(0.f, 0.f, 0.f);
	sm->getActiveScene()->findByName("outline")->getTransform()->localScale = glm::vec3(1.f, 1.f, 1.f);
	sm->getActiveScene()->findByName("outline")->getTransform()->localRotation = glm::vec3(0.f, 0.f, 0.f);

	GameObject* skydome = new GameObject("skydome");

	Mesh* meshSphere = new Mesh();
	meshSphere->createDome(10, 10, 50);
	skydomeModel = std::make_shared<Model>(meshSphere);

	//Fruit
	RL.mangoModel->SetShader(RL.diffuseInstancedShader);
	RL.mangoModel->AddTexture("res/textures/mango.jpg", "diffuseMap");
	
	//drzewa
	RL.treelog->AddTexture("res/textures/bark.jpg", "diffuseMap");
	RL.treetrunk->AddTexture("res/textures/bark.jpg", "diffuseMap");
	RL.treebranch1->AddTexture("res/textures/bark.jpg", "diffuseMap");

	RL.treelog->AddTexture("res/textures/Tree2_normal.png", "normalMap");
	RL.treetrunk->AddTexture("res/textures/Tree1_normal.png", "normalMap");
	RL.treebranch1->AddTexture("res/textures/Tree3_normal.png", "normalMap");

	RL.treebranch1->SetShader(RL.phongInstancedShader);
	RL.planeSectormodel->AddTexture("res/textures/bark.jpg", "diffuseMap");
	RL.planeSectormodel->AddTexture("res/floor_texture.jpg", "diffuseMap2");
	RL.treetrunk->SetShader(RL.phongInstancedShader);
	RL.treelog->SetShader(RL.phongInstancedShader);
	RL.planeSectormodel->SetShader(RL.groundShader);
	RL.treetrunk.get()->AddTexture("res/textures/bark.jpg", "diffuseMap");
	RL.treelog.get()->AddTexture("res/textures/bark.jpg", "diffuseMap");
	RL.phongInstancedShader->use();
	RL.phongInstancedShader->setVec3("lightPos", lightPos);

	RL.treeloglow.get()->AddTexture("res/textures/bark.jpg", "diffuseMap");
	RL.treetrunklow.get()->AddTexture("res/textures/bark.jpg", "diffuseMap");

	RL.treeloglow.get()->AddTexture("res/textures/Tree2_normal.png", "normalMap");
	RL.treetrunklow.get()->AddTexture("res/textures/Tree1_normal.png", "normalMap");

	RL.treetrunklow.get()->SetShader(RL.phongInstancedShader);
	RL.treeloglow.get()->SetShader(RL.phongInstancedShader);

	RL.box2model->SetShader(RL.shaderTree);
	skydomeModel->SetShader(RL.skydomeShader);
	RL.enemyWeaponmodel.get()->SetShader(RL.phongInstancedShader);

	RL.leafModel->AddTexture("res/textures/nic.jpg", "diffuseMap");
	RL.leafModel->SetShader(RL.phongInstancedShader);

	RL.planeModel.get()->AddTexture("res/junglewall.png", "diffuseMap");
	RL.planeModel.get()->SetShader(RL.diffuseShader);

	skydome->addModelComponent(skydomeModel);

	skydome->localTransform->localScale=glm::vec3(100.f);

	sm->getActiveScene()->addObject(skydome);
	int key, action;
	camera->transform->localPosition = glm::vec3(-1.0f, 2.0f, 20.0f);

	const TimePoint tpStart = Clock::now();

	glm::mat4 V(1.f);
	glm::mat4 P;
	std::array<Plane, 6> frustumPlanes;
	glm::mat4 lightProjection, lightView;
	glm::vec2 PlayerPosv2;

	RL.animodel.get()->AddTexture("res/textures/modelMonk_BaseColor.png", "diffuseMap");
	//sm->getActiveScene()->findByName("player")->getModelComponent()->AddTexture("res/textures/modelMonk_BaseColor.png", "diffuseMap");
	//sm->getActiveScene()->findByName("player")->getTransform()->localPosition = glm::vec3(7.f, 1.f, 1.f);
	//sm->getActiveScene()->findByName("player")->getTransform()->localScale = glm::vec3(1.f, 1.f, 1.f);

	sm->getActiveScene()->findByName("skydome")->getModelComponent().get()->AddTexture("res/chmury1.png", "diffuseMap");

	
	//glm::vec3 lightPos(0.5f, 20.0f, 0.3f);
	glm::vec3* lightColor = new glm::vec3(1.f, 1.0f, 1.f);
	RL.phongInstancedShader->use();
	RL.phongInstancedShader->setVec3("lightColor", *lightColor);

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

	sm->getActiveScene()->findByName("skydome")->setRotating(true, 1.f, glm::vec3(0.f, 1.f, 0.f));

	float deltaTime = 0;
	float deltaTime2 = 0;
	float lastTime = 0;
	float staticUpdateTime = 0;
	int shadowsFrameCounter = -1;
	bool isFromFile = false;
	bool rotating = true;
	bool isBlue = false;

	cm.pm = pm;
	SectorSelector* sectorSelector = new SectorSelector(&sectorsPom);
	sm->loadScene("first");
	sm->activeScene = sm->scenes.at(0);
	sm->getActiveScene()->addObject(anim);
	enemyManager = new EnemyStateManager(pathfinder, &cm, pm, sectorSelector);
	bool regenInstances = false;
	bool regenInstancesEnemy = false;
	bool regenInstancesEnemyWeapon = false;

	tutorial1 = new GameObject("tutorial1");
	UI* tutorialui1 = new UI(writing);
	tutorialui1->addShader(LogoShader);
	tutorial1->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f - 200.0f, windowGlobals.windowHeight - 100.f, 0.0f);
	tutorialui1->setText("WASD - ruch");
	tutorial1->uiComponent = tutorialui1;
	sm->getActiveScene()->addObject(tutorial1);

	tutorial2 = new GameObject("tutorial2");
	UI* tutorialui2 = new UI(writing);
	tutorialui2->addShader(LogoShader);
	tutorial2->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f - 200.0f, windowGlobals.windowHeight - 150.f, 0.0f);
	tutorialui2->setText("spacja - skok");
	tutorial2->uiComponent = tutorialui2;
	sm->getActiveScene()->addObject(tutorial2);

	tutorial3 = new GameObject("tutorial3");
	UI* tutorialui3 = new UI(writing);
	tutorialui3->addShader(LogoShader);
	tutorial3->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f - 200.0f, windowGlobals.windowHeight - 200.f, 0.0f);
	tutorialui3->setText("lewy przycisk myszy - atak, prawy przycisk - unik");
	tutorial3->uiComponent = tutorialui3;
	sm->getActiveScene()->addObject(tutorial3);

	tutorial4 = new GameObject("tutorial4");
	UI* tutorialui4 = new UI(writing);
	tutorialui4->addShader(LogoShader);
	tutorial4->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f - 200.0f, windowGlobals.windowHeight - 250.f, 0.0f);
	tutorialui4->setText("E - atak z drzewa/podlozenie banana");
	tutorial4->uiComponent = tutorialui4;
	sm->getActiveScene()->addObject(tutorial4);

	tutorial5 = new GameObject("tutorial5");
	UI* tutorialui5 = new UI(writing);
	tutorialui5->addShader(LogoShader);
	tutorial5->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f - 200.0f, windowGlobals.windowHeight - 300.f, 0.0f);
	tutorialui5->setText("R przy drwalach - prowokacja");
	tutorial5->uiComponent = tutorialui5;
	sm->getActiveScene()->addObject(tutorial5);

	tutorial6 = new GameObject("tutorial6");
	UI* tutorialui6 = new UI(writing);
	tutorialui6->addShader(LogoShader);
	tutorial6->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f - 200.0f, windowGlobals.windowHeight - 350.f, 0.0f);
	tutorialui6->setText("Pokonuj drwali dopoki nie uciekna!");
	tutorial6->uiComponent = tutorialui6;
	sm->getActiveScene()->addObject(tutorial6);

	tutorial7 = new GameObject("tutorial7");
	UI* tutorialui7 = new UI(writing);
	tutorialui7->addShader(LogoShader);
	tutorial7->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f - 200.0f, windowGlobals.windowHeight - 400.f, 0.0f);
	tutorialui7->setText("Korzystaj z drzew, aby zyskac przewage!");
	tutorial7->uiComponent = tutorialui7;
	sm->getActiveScene()->addObject(tutorial7);

	titlescreen = new GameObject("titlescreen");
	UI* titleui = new UI(plane);
	titleui->addShader(LogoShader);
	titleui->setTexture("res/junglewall.png");
	titlescreen->uiComponent = titleui;
	titlescreen->localTransform->localScale = glm::vec3(22.f, 12.f, 1.f);
	sm->getActiveScene()->addObject(titlescreen);

	playButton = new GameObject("playButton");
	UI* playui = new UI(button);
	playui->addShader(LogoShader);
	playui->setTexture("res/button.png");
	playui->setSize(glm::vec2(150.0f, 60.f));
	playButton->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f - 200.0f, windowGlobals.windowHeight * 0.5f, 0.0f);
	playui->input = input;
	playui->onClick = generate;
	playui->setText("PLAY");
	playButton->uiComponent = playui;
	sm->getActiveScene()->addObject(playButton);

	tutorialButton = new GameObject("tutorialButton");
	UI* tutorialbuttonui = new UI(button);
	tutorialbuttonui->addShader(LogoShader);
	tutorialbuttonui->setTexture("res/button.png");
	tutorialbuttonui->setSize(glm::vec2(260.0f, 60.f));
	tutorialButton->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f - 200.0f, windowGlobals.windowHeight * 0.5f - 100.0f, 0.0f);
	tutorialbuttonui->input = input;
	tutorialbuttonui->onClick = showTutorial;
	tutorialbuttonui->setText("TUTORIAL");
	tutorialButton->uiComponent = tutorialbuttonui;
	sm->getActiveScene()->addObject(tutorialButton);

	backButton = new GameObject("backButton");
	UI* backbuttonui = new UI(button);
	backbuttonui->addShader(LogoShader);
	backbuttonui->setTexture("res/button.png");
	backbuttonui->setSize(glm::vec2(250.0f, 60.f));
	backButton->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f, 100.0f, 0.0f);
	backbuttonui->input = input;
	backbuttonui->onClick = showMain;
	backbuttonui->setText("BACK");
	backButton->uiComponent = backbuttonui;

	acknowledgmentsButton = new GameObject("acknowledgmentsButton");
	UI* acknowledgmentsButtonui = new UI(button);
	acknowledgmentsButtonui->addShader(LogoShader);
	acknowledgmentsButtonui->setTexture("res/button.png");
	acknowledgmentsButtonui->setSize(glm::vec2(520.0f, 60.f));
	acknowledgmentsButton->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f - 200.0f, windowGlobals.windowHeight * 0.5f - 200.0f, 0.0f);
	acknowledgmentsButtonui->input = input;
	acknowledgmentsButtonui->onClick = showAcknowledgments;
	acknowledgmentsButtonui->setText("ACKNOWLEDGMENTS");
	acknowledgmentsButton->uiComponent = acknowledgmentsButtonui;
	sm->getActiveScene()->addObject(acknowledgmentsButton);

	//audioManager->playSound("test", true);
	Scene* LoseScene = new Scene("LoseScreen");
	GameObject* loseScreen = new GameObject("loseScreen");
	UI* loseScreenUI = new UI(writing);
	loseScreenUI->addShader(LogoShader);
	loseScreenUI->setText("Nie udało ci się powstrzymać wycinki lasu");
	loseScreen->uiComponent = loseScreenUI;
	loseScreen->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f - 100.0f, windowGlobals.windowHeight * 0.5f, 0.0f);
	LoseScene->addObject(loseScreen);
	GameObject* returnButton = new GameObject("returnButton");
	UI* returnButtonUI = new UI(button);
	returnButtonUI->addShader(LogoShader);
	returnButtonUI->setTexture("res/button.png");
	returnButtonUI->setSize(glm::vec2(200.0f, 60.f));
	returnButton->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f - 200.0f, windowGlobals.windowHeight * 0.5f - 100.0f, 0.0f);
	returnButtonUI->input = input;
	returnButtonUI->onClick = showMain;
	returnButtonUI->setText("RETURN");
	returnButton->uiComponent = returnButtonUI;
	LoseScene->addObject(returnButton);

	Scene* WinScene = new Scene("WinScreen");
	GameObject* winScreen = new GameObject("winScreen");
	UI* winScreenUI = new UI(writing);
	winScreenUI->addShader(LogoShader);
	winScreenUI->setText("Drwale pokonani");
	winScreen->uiComponent = winScreenUI;
	winScreen->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f - 100.0f, windowGlobals.windowHeight * 0.5f, 0.0f);
	WinScene->addObject(winScreen);
	WinScene->addObject(returnButton);

	Scene* menuScene = new Scene("menuScene");
	GameObject* menu = new GameObject("menu");
	UI* menuUi = new UI(writing);
	menuUi->addShader(LogoShader);
	menuUi->setText("To jest menu, xd");
	menu->uiComponent = menuUi;
	menu->localTransform->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f - 100.0f, windowGlobals.windowHeight * 0.5f, 0.0f);
	menuScene->addObject(menu);
	menuScene->addObject(returnButton);

	bool fruitsrenderd = false;
	glm::vec2 playerPos = glm::vec2(0.f);
	while (!glfwWindowShouldClose(window)) {
		FrameMark;
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, 1);
		}
		glClearColor(0.2f, 0.3f, 0.7f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		gameTime = std::chrono::duration_cast<Duration>(Clock::now() - tpStart).count();
		shadowsFrameCounter++;
		deltaTime = gameTime - lastTime;
		deltaTime2 += gameTime - lastTime;
		lastTime = gameTime;
		spawnerTime += deltaTime;
		staticUpdateTime += deltaTime;
		sectorSelectorTime += deltaTime;
		//std::cout << "Delta time: " << deltaTime << std::endl;


		RL.phongShader->use();
		//RL.phongShader->setVec3("lightPos", lightPos);
		RL.phongShader->setVec3("viewPos", camera->transform->getLocalPosition());
		RL.phongInstancedShader->use();
		//RL.phongInstancedShader->setVec3("lightPos", lightPos);
		RL.phongInstancedShader->setVec3("viewPos", camera->transform->getLocalPosition());


		tm->setTime(deltaTime);
		if (gameMode.getMode() == GameMode::Game) {
			pm->ManagePlayer(deltaTime2, deltaTime);
			V = camera->getViewMatrixPlayer();
			//V = glm::scale(V, glm::vec3(30.f));
			sm->getActiveScene()->findByName("skydome")->timeSetting(gameTime / 7, glm::vec2(10, 10));
		}
		else if (gameMode.getMode() != GameMode::Game) {

			V = camera->getViewMatrix();
		}
		audioManager->setListenerPosition(camera->transform->localPosition.x, camera->transform->localPosition.y, camera->transform->localPosition.z);
		audioManager->setListenerOrientation(V);
		//animacje
		//animPlayer->UpdateAnimation(deltaTime);

		P = glm::perspective(glm::radians(input->GetZoom()), static_cast<float>(windowGlobals.windowWidth) / windowGlobals.windowHeight, 1.f, 5000.f);
		frustumPlanes = Plane::calculateFrustumPlanes(glm::perspective(glm::radians(120.f), static_cast<float>(windowGlobals.windowWidth) / windowGlobals.windowHeight, 0.1f, 500.f) * V);

		if (input->checkAnyKey())
		{
			//if (input->checkKey(GLFW_KEY_ESCAPE)) {
			//	if (gameMode.getMode() == GameMode::Game) {
			//		//sm->saveScene("mainLoop");
			//		sm->activeScene = menuScene;
			//		gameMode.setMode(GameMode::Menu);
			//		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			//	}
			//	else if(gameMode.getMode() == GameMode::Menu){
			//		//sm->loadScene("mainLoop");
			//	}
			}
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
			pm->ManagePlayer(deltaTime2, deltaTime);
		}

		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
			//sm->getActiveScene()->findByName("outline")->getModelComponent()->SetOutlineShader(nullptr);
		}
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
			//sm->getActiveScene()->findByName("outline")->getModelComponent()->SetOutlineShader(outlineShader);
		}

		if (sm->getActiveScene()->findByName("player")) {
			if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
				staticUpdateTime = 0;
				playerAtention = true;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
			frustumTest = !frustumTest;
		}

		if (staticUpdateTime > 1.5f) {
			staticUpdateTime = 0.f;
			playerAtention = false;
		}
		enemyManager->update(deltaTime, playerAtention);
		for (auto object : sm->getActiveScene()->gameObjects) {
			if (object->name.starts_with("sector")) {
				for (auto tree : object->children) {
					Tree* treeActual = tree->getAsActualType<Tree>();
					treeActual->updateHealth(deltaTime);
					if (treeActual->health <= 0) {
						object->children.erase(std::remove(object->children.begin(), object->children.end(), tree), object->children.end());
						for (auto sect : cm.sections) {
							sect->staticObjects.erase(std::remove(sect->staticObjects.begin(), sect->staticObjects.end(), tree), sect->staticObjects.end());
							sect->staticObjects.erase(std::remove(sect->staticObjects.begin(), sect->staticObjects.end(), tree->children.at(0)), sect->staticObjects.end());
							for (auto ch : tree->children.at(0)->children)
							{
								sect->staticObjects.erase(std::remove(sect->staticObjects.begin(), sect->staticObjects.end(), ch), sect->staticObjects.end());
							}
						}
						pathfinder->trees.erase(std::remove_if(pathfinder->trees.begin(), pathfinder->trees.end(), [treeActual](std::pair<int, Tree*> pair) {return pair.second == treeActual; }), pathfinder->trees.end());
						for (auto enemy : enemyManager->enemies) {
							if (enemy->chosenTree == treeActual) {
								enemy->chosenTree = nullptr;
							}
						}
						regenInstances = true;
						delete treeActual;
						if (object->name.ends_with(std::to_string(sectorSelector->selectedSector)) && object->children.size() == 0 && sectorSelectorTime > 30.0f) {
							sectorSelector->selectSector(1);
						}
						else if (object->name.ends_with(std::to_string(sectorSelector->selectedSector2)) && object->children.size() == 0 && sectorSelectorTime > 30.0f) {
							sectorSelector->selectSector(2);
						}
						sectorSelectorTime = 0;
					}
				}
			}
		}
		if (loaded && pathfinder->trees.size() == 0) {
			loaded = false;
			Lost = true;
		}
		//std::cout << glm::to_string(sm->getActiveScene()->findByName("player")->getTransform()->getMatrix()) << std::endl;
		//sm->getActiveScene()->findByName("player")->getTransform()->localPosition = glm::vec3(3.0f, 2.0f, 3.0f);
		//std::cout << glm::to_string(sm->getActiveScene()->findByName("player")->getTransform()->getMatrix()) << std::endl;
		sm->getActiveScene()->Update(V, P, deltaTime);
		

		RL.diffuseShader->use();
		RL.diffuseShader->setMat4("view", V);
		RL.diffuseShader->setMat4("projection", P);
		RL.diffuseInstancedShader->use();
		RL.diffuseInstancedShader->setMat4("view", V);
		RL.diffuseInstancedShader->setMat4("projection", P);
		//generating shadows
		if (sm->activeScene->findByName("player")) {
			lightPos = sm->activeScene->findByName("player")->localTransform->localPosition + glm::vec3(10.f, 80.0f, 0.f);
			lightPos.y = 80.f;

			lightView = glm::lookAt(lightPos, sm->activeScene->findByName("player")->localTransform->localPosition, glm::vec3(0.0, 1.0, 0.0));
		}
		else {

			lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		}
		float near_plane = 1.0f, far_plane = 100.f;
		lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		if (shadowsFrameCounter % 2 == 0) {
			shadowsFrameCounter = 0;
			RL.depthShader->use();
			RL.depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
			RL.depthAnimationShader->use();
			RL.depthAnimationShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
			RL.depthInstancedShader->use();
			RL.depthInstancedShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);


			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);

			transformsBranch.clear();
			transformsLog.clear();
			transformsTree.clear();
			transformsLogLow.clear();
			transformsTreeLow.clear();
			transformsLeaves.clear();
			transformsBanana.clear();
			transformsMango.clear();
			
			PlayerPosv2 = glm::vec2(0.f, 0.f);
			if (sm->getActiveScene()->findByName("player")) {
				PlayerPosv2 = glm::vec2(sm->getActiveScene()->findByName("player")->getTransform()->localPosition.x, sm->getActiveScene()->findByName("player")->getTransform()->localPosition.z);
			}
			lowTree = nullptr;
			lowLog = nullptr;
			for (auto object : sm->getActiveScene()->gameObjects) {
				if (object->name.starts_with("sector")) {
					for (auto tree : object->children) {
						if (tree->name.starts_with("tree") && tree->isVisible) {
							if (glm::distance(PlayerPosv2, glm::vec2(tree->getTransform()->localPosition.x, tree->getTransform()->localPosition.z)) < 150.0f) {
								transformsTree.push_back(tree->getTransform());
								transformsLog.push_back(tree->children.at(0)->getTransform());
								tree->modelComponent = RL.treetrunk;
								tree->children.at(0)->modelComponent = RL.treelog;
								for (auto ch : tree->children.at(0)->children)
								{
									transformsBranch.push_back(ch->getTransform());
								}
							}
							else {
								transformsTreeLow.push_back(tree->getTransform());
								transformsLogLow.push_back(tree->children.at(0)->getTransform());
								lowTree = tree;
								lowLog = tree->children.at(0);
								lowTree->modelComponent = RL.treetrunklow;
								lowLog->modelComponent = RL.treeloglow;
							}
							transformsLeaves.push_back(tree->children.at(0)->children.at(0)->children.at(0)->getTransform());
						}
					}
				}
				else if (object->name.starts_with("Fruit")) {
					if (object->name.ends_with("Banana")) {
						transformsBanana.push_back(object->getTransform());
					}
					else if (object->name.ends_with("Mango")) {
						transformsMango.push_back(object->getTransform());
					}
				}
			}

			for (auto sector : sm->getActiveScene()->gameObjects) {
				if (sector->name.starts_with("sector") && transformsTree.size() > 0 && sector->children.size() > 0) {
					for (auto mesh : sector->children.at(0)->getModelComponent().get()->meshes) {
						mesh->initInstances(transformsTree);
					}
					sector->children.at(0)->getModelComponent().get()->drawInstances(RL.depthInstancedShader);
					for (auto mesh : sector->children.at(0)->children.at(0)->getModelComponent().get()->meshes) {
						mesh->initInstances(transformsLog);
					}
					sector->children.at(0)->children.at(0)->getModelComponent().get()->drawInstances(RL.depthInstancedShader);
					for (auto mesh : sector->children.at(0)->children.at(0)->children.at(0)->getModelComponent().get()->meshes) {
						mesh->initInstances(transformsBranch);
					}
					sector->children.at(0)->children.at(0)->children.at(0)->getModelComponent().get()->drawInstances(RL.depthInstancedShader);
					for (auto mesh : sector->children.at(0)->children.at(0)->children.at(0)->children.at(0)->getModelComponent().get()->meshes) {
						mesh->initInstances(transformsLeaves);
					}
					sector->children.at(0)->children.at(0)->children.at(0)->children.at(0)->getModelComponent().get()->drawInstances(RL.depthInstancedShader);
					if (lowTree) {
						lowTree->getModelComponent().get()->getFirstMesh()->initInstances(transformsTreeLow);
						lowTree->getModelComponent().get()->drawInstances(RL.depthInstancedShader);
						lowLog->getModelComponent().get()->getFirstMesh()->initInstances(transformsLogLow);
						lowLog->getModelComponent().get()->drawInstances(RL.depthInstancedShader);
					}
					break;
				}
			}
			for (auto fruit : sm->getActiveScene()->gameObjects) {
				if (fruit->name == "FruitBanana" && transformsBanana.size() > 0) {
					for (auto mesh : fruit->getModelComponent().get()->meshes) {
						mesh->initInstances(transformsBanana);
					}
					fruit->getModelComponent().get()->drawInstances(RL.depthInstancedShader);
					fruitsrenderd = true;
					break;
				}
			}
			for (auto fruit : sm->getActiveScene()->gameObjects) {
				if (fruit->name == "FruitMango" && transformsMango.size() > 0) {
					for (auto mesh : fruit->getModelComponent().get()->meshes) {
						mesh->initInstances(transformsMango);
					}
					fruit->getModelComponent().get()->drawInstances(RL.depthInstancedShader);
					fruitsrenderd = true;
					break;
				}
			}
			transformsEnemy.clear();
			for (auto enemy : enemyManager->enemies) {
				if (enemy->isVisible) {
					transformsEnemy.push_back(enemy->getTransform());
				}
			}
			transformsEnemyWeapon.clear();
			for (auto enemy : enemyManager->enemies) {
				if (enemy->children.at(0)->active && enemy->isVisible) {
					transformsEnemyWeapon.push_back(enemy->children.at(0)->getTransform());
				}
			}
			if (transformsEnemy.size() > 0) {
				for (auto mesh : enemyManager->enemies.at(0)->getModelComponent().get()->meshes) {
					mesh->initInstances(transformsEnemy);
				}
				enemyManager->enemies.at(0)->getModelComponent().get()->drawInstances(RL.depthInstancedShader);
				if (transformsEnemyWeapon.size() > 0) {
					for (auto mesh : enemyManager->enemies.at(0)->children.at(0)->getModelComponent().get()->meshes) {
						mesh->initInstances(transformsEnemyWeapon);
					}
					enemyManager->enemies.at(0)->children.at(0)->getModelComponent().get()->drawInstances(RL.depthInstancedShader);
				}
			}

			sm->getActiveScene()->Draw(RL.depthShader, RL.depthAnimationShader);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, windowGlobals.windowWidth, windowGlobals.windowHeight);
			RL.planeSectormodel->AddTexture(depthMap, "depthMap");
			RL.treetrunk.get()->AddTexture(depthMap, "depthMap");
			RL.treelog.get()->AddTexture(depthMap, "depthMap");
			RL.treebranch1.get()->AddTexture(depthMap, "depthMap");

			sm->getActiveScene()->lightSetting(camera->transform->getLocalPosition(), lightPos, glm::vec3(1.0f));
			sm->getActiveScene()->shadowSetting(lightSpaceMatrix);
		}

		if (sm->getActiveScene()->findByName("player")) {
			if (sm->getActiveScene()->findByName("HPcount")) {
				if (sm->getActiveScene()->findByName("player")->hp > 0) {
					sm->getActiveScene()->findByName("HPcount")->uiComponent->setTexture("res/serce" + std::to_string(sm->getActiveScene()->findByName("player")->hp) + ".png");
				}
				else {
					sm->getActiveScene()->findByName("HPcount")->uiComponent->setTexture("res/serce0.png");
					Lost = true;
				}
			}
			cm.addObject(sm->getActiveScene()->findByName("player"));
		}
		for (auto enemy : enemyManager->enemies) {
			if (enemy->hp <= 0) {
				pbd->objects.erase(std::remove(pbd->objects.begin(), pbd->objects.end(), enemy), pbd->objects.end());
				for (auto sect : cm.sections) {
					sect->objects.erase(std::remove(sect->objects.begin(), sect->objects.end(), enemy), sect->objects.end());
				}
				sm->getActiveScene()->gameObjects.erase(std::remove(sm->getActiveScene()->gameObjects.begin(), sm->getActiveScene()->gameObjects.end(), enemy), sm->getActiveScene()->gameObjects.end());
				enemyManager->enemies.erase(std::remove(enemyManager->enemies.begin(), enemyManager->enemies.end(), enemy), enemyManager->enemies.end());
				if (enemy->chosenTree) {
					if (enemy->chosenTree->getAsActualType<Tree>()) {
						enemy->chosenTree->getAsActualType<Tree>()->removeChopper(enemy);
					}
				}
				regenInstancesEnemy = true;
				/*if (enemyManager->enemies.size() > 0) {
					std::vector<Transform*> transformsEnemy;
					for (auto enemy : enemyManager->enemies) {
						transformsEnemy.push_back(enemy->getTransform());
					}
					enemyManager->enemies.at(0)->getModelComponent().get()->getFirstMesh()->initInstances(transformsEnemy);
				}*/
				reqEnemies--;
				if (reqEnemies < 1) {
					Won = true;
				}
				sm->getActiveScene()->findByName("Enemycount")->uiComponent->setText("Pozostali drwale : " + std::to_string(reqEnemies));
				delete enemy;
			}
			else {
				cm.addObject(enemy);
			}
		}

		pbd->simulateB4Collisions(deltaTime);
		cm.simulate(deltaTime);
		pbd->simulateAfterCollisions(deltaTime);
		performFrustumCulling(frustumPlanes, cm.sections);
		if (sm->getActiveScene()->findByName("player")) {
			sm->getActiveScene()->findByName("player")->isVisible = true;
		}
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
		if (sectorSelector) {
			if(sectorpointer){
				if (sectorSelector->selectedSector != 0) {
					playerPos = glm::vec2(sm->getActiveScene()->findByName("player")->getTransform()->localPosition.x, sm->getActiveScene()->findByName("player")->getTransform()->localPosition.z);
					if (glm::distance(playerPos, sectorCenters[sectorSelector->selectedSector - 1]) > 35.0f) {
						sectorpointer->isVisible = true;
						glm::vec2 direction = glm::normalize(sectorCenters[sectorSelector->selectedSector - 1] - playerPos);
						glm::vec2 cameraFront = glm::normalize(glm::vec2(camera->getFront().x, camera->getFront().z));
						float angle = glm::atan(direction.y, direction.x) - glm::atan(cameraFront.y, cameraFront.x);
						sectorpointer->getTransform()->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f - 100.f + radius * cos(angle), windowGlobals.windowHeight * 0.5f - radius * sin(angle), 0.0f);
						sectorpointer->getTransform()->localRotation = glm::vec3(0.0f, 0.0f, -glm::degrees(angle));
					}
					else {
						sectorpointer->isVisible = false;
					}
				}
			}
			if (sectorpointer2) {
				if (sectorSelector->selectedSector2 != 0) {
					playerPos = glm::vec2(sm->getActiveScene()->findByName("player")->getTransform()->localPosition.x, sm->getActiveScene()->findByName("player")->getTransform()->localPosition.z);
					if (glm::distance(playerPos, sectorCenters[sectorSelector->selectedSector2 - 1]) > 35.0f) {
						sectorpointer2->isVisible = true;
						glm::vec2 direction = glm::normalize(sectorCenters[sectorSelector->selectedSector2 - 1] - playerPos);
						glm::vec2 cameraFront = glm::normalize(glm::vec2(camera->getFront().x, camera->getFront().z));
						float angle = glm::atan(direction.y, direction.x) - glm::atan(cameraFront.y, cameraFront.x);
						sectorpointer2->getTransform()->localPosition = glm::vec3(windowGlobals.windowWidth * 0.5f - 100.f + radius * cos(angle), windowGlobals.windowHeight * 0.5f - radius * sin(angle), 0.0f);
						angle = atan2(direction.y, direction.x) - atan2(camera->getFront().z, camera->getFront().x);
						sectorpointer2->getTransform()->localRotation = glm::vec3(0.0f, 0.0f, glm::degrees(angle));
					}
					else {
						sectorpointer2->isVisible = false;
					}
				}
			}
		}
		if (sm->getActiveScene()->findByName("skydome")) {
			sm->getActiveScene()->findByName("skydome")->getTransform()->localPosition = camera->transform->localPosition;
		}
		//sm->getActiveScene()->Draw(V, P);
		transformsBranch.clear();
		transformsLog.clear();
		transformsTree.clear();
		transformsLogLow.clear();
		transformsTreeLow.clear();
		transformsLeaves.clear();
		transformsBanana.clear();
		transformsMango.clear();
		PlayerPosv2 = glm::vec2(0.f, 0.f);
		if (sm->getActiveScene()->findByName("player")) {
			PlayerPosv2 = glm::vec2(sm->getActiveScene()->findByName("player")->getTransform()->localPosition.x, sm->getActiveScene()->findByName("player")->getTransform()->localPosition.z);
		}
		lowTree = nullptr;
		lowLog = nullptr;
		for (auto object : sm->getActiveScene()->gameObjects) {
			if (object->name.starts_with("sector")) {
				for (auto tree : object->children) {
					if (tree->name.starts_with("tree") && tree->isVisible) {
						if (glm::distance(PlayerPosv2, glm::vec2(tree->getTransform()->localPosition.x, tree->getTransform()->localPosition.z)) < 75.0f) {
							transformsTree.push_back(tree->getTransform());
							transformsLog.push_back(tree->children.at(0)->getTransform());
							tree->modelComponent = RL.treetrunk;
							tree->children.at(0)->modelComponent = RL.treelog;
							for (auto ch : tree->children.at(0)->children)
							{
								transformsBranch.push_back(ch->getTransform());
							}
						}
						else {
							transformsTreeLow.push_back(tree->getTransform());
							transformsLogLow.push_back(tree->children.at(0)->getTransform());
							lowTree = tree;
							lowLog = tree->children.at(0);
							lowTree->modelComponent = RL.treetrunklow;
							lowLog->modelComponent = RL.treeloglow;
						}
						transformsLeaves.push_back(tree->children.at(0)->children.at(0)->children.at(0)->getTransform());
					}
				}
			}
			else if (object->name.starts_with("Fruit")) {
				if (object->name.ends_with("Banana")) {
					transformsBanana.push_back(object->getTransform());
				}
				else if (object->name.ends_with("Mango")) {
					transformsMango.push_back(object->getTransform());
				}
			}
		}
		for (auto sector : sm->getActiveScene()->gameObjects) {
			if (sector->name.starts_with("sector")) {
				for (auto tree : sector->children) {
					tree->getModelComponent().get()->GetShader()->use();
					tree->getModelComponent().get()->GetShader()->setVec3("viewPos", camera->transform->getLocalPosition());
					tree->getModelComponent().get()->GetShader()->setMat4("LSMatrix", lightSpaceMatrix);
					tree->getModelComponent().get()->GetShader()->setMat4("view", V);
					tree->getModelComponent().get()->GetShader()->setMat4("projection", P);
				}
			}
		}
		for (auto sector : sm->getActiveScene()->gameObjects) {
			if (sector->name.starts_with("sector") && transformsTree.size() > 0 && sector->children.size()>0) {
				for (auto mesh : sector->children.at(0)->getModelComponent().get()->meshes) {
					mesh->initInstances(transformsTree);
				}
				sector->children.at(0)->getModelComponent().get()->drawInstances();
				for (auto mesh : sector->children.at(0)->children.at(0)->getModelComponent().get()->meshes) {
					mesh->initInstances(transformsLog);
				}
				sector->children.at(0)->children.at(0)->getModelComponent().get()->drawInstances();
				for (auto mesh : sector->children.at(0)->children.at(0)->children.at(0)->getModelComponent().get()->meshes) {
					mesh->initInstances(transformsBranch);
				}
				sector->children.at(0)->children.at(0)->children.at(0)->getModelComponent().get()->drawInstances();
				for (auto mesh : sector->children.at(0)->children.at(0)->children.at(0)->children.at(0)->getModelComponent().get()->meshes) {
					mesh->initInstances(transformsLeaves);
				}
				sector->children.at(0)->children.at(0)->children.at(0)->children.at(0)->getModelComponent().get()->drawInstances();
				if (lowTree) {
					lowTree->getModelComponent().get()->getFirstMesh()->initInstances(transformsTreeLow);
					lowTree->getModelComponent().get()->drawInstances();
					lowLog->getModelComponent().get()->getFirstMesh()->initInstances(transformsLogLow);
					lowLog->getModelComponent().get()->drawInstances();
				}
				break;
			}
		}
		for (auto fruit : sm->getActiveScene()->gameObjects) {
			if (fruit->name == "FruitBanana" && transformsBanana.size() > 0) {
				for (auto mesh : fruit->getModelComponent().get()->meshes) {
					mesh->initInstances(transformsBanana);
				}
				fruit->getModelComponent().get()->drawInstances();
				fruitsrenderd = true;
				break;
			}
		}
		for (auto fruit : sm->getActiveScene()->gameObjects) {
			if (fruit->name == "FruitMango" && transformsMango.size() > 0) {
				for (auto mesh : fruit->getModelComponent().get()->meshes) {
					mesh->initInstances(transformsMango);
				}
				fruit->getModelComponent().get()->drawInstances();
				fruitsrenderd = true;
				break;
			}
		}
		transformsEnemy.clear();
		for (auto enemy : enemyManager->enemies) {
			if (enemy->isVisible) {
				transformsEnemy.push_back(enemy->getTransform());
			}
		}
		transformsEnemyWeapon.clear();
		for (auto enemy : enemyManager->enemies) {
			if (enemy->children.at(0)->active && enemy->isVisible) {
				transformsEnemyWeapon.push_back(enemy->children.at(0)->getTransform());
			}
		}
	/*	if (transformsEnemy.size() > 0) {
			RL.enemyShader->use();
			RL.enemyShader->setMat4("view", V);
			RL.enemyShader->setMat4("projection", P);
			for (auto mesh : enemyManager->enemies.at(0)->getModelComponent().get()->meshes) {
				mesh->initInstances(transformsEnemy);
			}
			enemyManager->enemies.at(0)->getModelComponent().get()->drawInstances();
			enemyManager->enemies.at(0)->children.at(0)->getModelComponent().get()->GetShader()->use();
			enemyManager->enemies.at(0)->children.at(0)->getModelComponent().get()->GetShader()->setMat4("view", V);
			enemyManager->enemies.at(0)->children.at(0)->getModelComponent().get()->GetShader()->setMat4("projection", P);
			if (transformsEnemyWeapon.size() > 0) {
				for (auto mesh : enemyManager->enemies.at(0)->children.at(0)->getModelComponent().get()->meshes) {
					mesh->initInstances(transformsEnemyWeapon);
				}
				enemyManager->enemies.at(0)->children.at(0)->getModelComponent().get()->drawInstances();
			}
		}*/


		/*if (enemyManager->enemies.size() > 0) {
			for (auto enemy : enemyManager->enemies) {
				enemy->getModelComponent().get()->GetShader()->use();
				enemy->getModelComponent().get()->GetShader()->setMat4("M", *enemy->getModelComponent().get()->getTransform());
				enemy->getModelComponent().get()->GetShader()->setMat4("view", V);
				enemy->getModelComponent().get()->GetShader()->setMat4("projection", P);
			}
			enemyManager->enemies.at(0)->getModelComponent().get()->getFirstMesh()->drawInstances();
		}*/

		//ability
		ability->UpdateTime(deltaTime);
		if (sm->getActiveScene()->findByName("DashTime") && sm->getActiveScene()->findByName("TreeAttack")) {
			sm->getActiveScene()->findByName("DashTime")->uiComponent->setText("Dash: " + std::to_string(ability->getTimeToRefresh("dash")));
			sm->getActiveScene()->findByName("TreeAttack")->uiComponent->setText("Atak z drzewa: " + std::to_string(ability->getTimeToRefresh("tree attack")));
			sm->getActiveScene()->findByName("bananaPeel")->uiComponent->setText("Banana: " + std::to_string(ability->bananaCount));
			if (sectorSelectorTime < 30) {
				wycinka = true;
				sm->getActiveScene()->findByName("EnemyTime")->uiComponent->setText("Czas do wycinki nowego sektora : " + std::to_string(30 - static_cast<int>(sectorSelectorTime)));
			}
			else {
				sm->getActiveScene()->findByName("EnemyTime")->uiComponent->setText("Drwale pojawili sie w jednym z sektorow");
			}
		}

		sm->getActiveScene()->Draw(V, P);

		RL.shaderTree->use();
		RL.shaderTree->setMat4("view", V);
		RL.shaderTree->setMat4("projection", P);

		while (input->IsMove()) {
			glm::vec2 dpos = input->getPosMouse();
			if (glfwGetInputMode(window, GLFW_CURSOR) != 212993) {
				camera->updateCamera(dpos);
			}
		}
		
		while (input->IsKeobarodAction(window)) {
			input->getMessage(key, action);

			if (gameMode.getMode() == GameMode::Debug) {
				input->getPressKey();
				if (key == GLFW_KEY_W && action == GLFW_REPEAT) {
					camera->ProcessKeyboard(FORWARD, gameTime);
				}
				else if (key == GLFW_KEY_S && action == GLFW_REPEAT) {
					camera->ProcessKeyboard(BACKWARD, gameTime);
				}
				else if (key == GLFW_KEY_D && action == GLFW_REPEAT) {
					camera->ProcessKeyboard(RIGHT, gameTime);
				}
				else if (key == GLFW_KEY_A && action == GLFW_REPEAT) {
					camera->ProcessKeyboard(LEFT, gameTime);
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
		if (sectorSelectorTime > 30.0f) {
			bool selected = false;
			if (sectorSelector->selectedSector == 0) {
				sectorSelector->selectSector(1);
				selected = true;
			}
			else if (sectorSelector->selectedSector2 == 0) {
				sectorSelector->selectSector(2);
				selected = true;
			}
			if (selected) {
				sectorSelectorTime = 0;
			}
		}
		if (spawnerTime > 8.f && loaded && spawnedEnemies <= maxEnemies) {
			spawnerTime = 0;
			int sector = rand() % 2 + 1;
			if (sectorSelector->selectedSector2 != 0) {
				if (sector == 1) {
					sector = sectorSelector->selectedSector;
				}
				else if (sector == 2) {
					sector = sectorSelector->selectedSector2;
				}
			}
			else {
				sector = sectorSelector->selectedSector;
			}
			std::cout << sectorSelector->selectedSector << ", " << sectorSelector->selectedSector2 << std::endl;
			if (sm->getActiveScene()->findByName("sector" + std::to_string(sector))) {
				Enemy* enemy = new Enemy("enemy" + std::to_string(spawnedEnemies), sm->getActiveScene()->findByName("sector" + std::to_string(sector))->localTransform->localPosition, glm::vec3(0.1f), glm::vec3(0.f), std::make_pair(2.0f, 6.f));
				enemy->Move(glm::vec3(5.0f));
				enemy->sector = sector;
				enemy->addModelComponent(RL.enemyModel2);
				pbd->objects.push_back(enemy);
				enemy->addColider(2);
				enemy->capsuleCollider = new CapsuleCollider(glm::vec3(0.0f), enemy->capsuleCollider->radius * 0.8f * 0.35f, enemy->capsuleCollider->height * 0.35f, 1.0f, true);
				/*glm::vec3 min = glm::vec3(0.0f);
				min.x += enemy->capsuleCollider->radius;
				min.z += enemy->capsuleCollider->radius;
				glm::vec3 max = glm::vec3(0.0f);
				max.x -= enemy->capsuleCollider->radius;
				max.z -= enemy->capsuleCollider->radius;
				max.y += enemy->capsuleCollider->height;
				enemy->boundingBox = new BoundingBox(min, max, 1.0f, true);
				enemy->capsuleCollider = nullptr;*/
				enemy->capsuleCollider->center.y += enemy->capsuleCollider->height * 0.5f;
				
				//enemy->modelComponent = RL.enemyModel;
				enemy->modelComponent = RL.animationEnemyModel;
				enemy->animPlayer = sm->getActiveScene()->findByName("basicEnemy")->animPlayer->clone(RL.animationEnemyModel.get());

				enemy->modelComponent.get()->capsuleCollider = enemy->capsuleCollider;
				enemy->getTransform()->localScale = glm::vec3(3.0f);
				sm->getActiveScene()->addObject(enemy);
				cm.addObject(enemy);
				spawnedEnemies++;
				std::pair<glm::vec3, Tree*> tree = pathfinder->decideInitalDestination(enemy->sector);
				enemy->chosenTreePos = tree.first;
				enemy->chosenTree = tree.second;
				enemy->velocity = enemy->chosenTreePos - enemy->localTransform->localPosition;
				enemy->state = EnemyState::Walking;
				enemyManager->addEnemy(enemy);
				enemy->hp = 30;
				GameObject* enemyWeapon = new GameObject("enemyWeapon" + std::to_string(spawnedEnemies));
				enemyWeapon->addModelComponent(RL.box2model);
				enemyWeapon->getTransform()->localScale = glm::vec3(0.3f);
				enemyWeapon->active = false;
				enemy->addChild(enemyWeapon);
				enemyWeapon->addColider(1);
				enemyWeapon->boundingBox->isTriggerOnly = true;
				enemyWeapon->colliders.push_back(enemyWeapon->boundingBox);
				enemyWeapon->isInstanced = true;
				enemyWeapon->modelComponent = RL.enemyWeaponmodel;
				enemyWeapon = nullptr;
				transformsEnemy.clear();
				transformsEnemyWeapon.clear();
				for (auto enemy : enemyManager->enemies) {
					transformsEnemy.push_back(enemy->getTransform());
					transformsEnemyWeapon.push_back(enemy->children.at(0)->getTransform());
				}
				//enemyManager->enemies.at(0)->getModelComponent().get()->getFirstMesh()->initInstances(transformsEnemy);
				//enemy->isInstanced = true;
			}
		}
		if (Lost) {
			sm->activeScene = LoseScene;
			gameMode.setMode(GameMode::Debug);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			Lost = false;
		}
		if (Won) {
			sm->activeScene = WinScene;
			gameMode.setMode(GameMode::Debug);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			Won = false;
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
		if (test) {

			sm->saveScene("first");
			test = false;
		}
	}
	delete audioManager;
	glfwTerminate();
	return 0;
}



int losujLiczbe(int a, int b) {
	return std::rand() % (b - a + 1) + a;
}

//-2, 15
int losujLiczbe2() {
	return std::rand() % 18 - 2;
}

bool checkLocations(float x1, float y1, float x2, float y2,float distance) {
	if (((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) > (distance * distance)) {
		return true;
	}
	return false;
}
