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



void init_imgui();
void imgui_begin();
void imgui_render();
void imgui_end();

std::string loadShaderSource(const std::string& _filepath);
GLuint compileShader(const GLchar* _source, GLenum _stage, const std::string& _msg);

using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;

using Duration = std::chrono::duration<float, std::ratio<1, 1>>;

constexpr int wys = 800, szer = 1000;
GLFWwindow* window;
SceneManager* sm;
Camera* camera;
Input* input;
PlayerMovement* pm;
float boxSpeed = 4.f;
bool test = true;
bool frustumTest = false;

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

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	init_imgui();
	sm = new SceneManager();
	//Scene* scene = new Scene("main");
	//sm->scenes.push_back(scene);
	sm->loadScene("first");
	sm->activeScene = sm->scenes.at(0);
	input = new Input(window);
	camera = new Camera(sm);
	pm = new PlayerMovement(sm, input);
	/*GameObject* go = new GameObject("test object");
	Transform* trans = new Transform();
	Component* comp = new Component();
	UI* ui = new UI();
	AnimateBody* ab = new AnimateBody();
	RigidBody* rb = new RigidBody();
	Axis* axis = new Axis("axis");
	
	go->getTransform();*/
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
	/*Shader* shader = new Shader("../../../../res/shaders/vs_player.vert", "../../../../res/shaders/fs_player.frag");
	GameObject* player = new GameObject("player");
	Model* playermodel = new Model(const_cast<char*>("../../../../res/objects/player.obj"));
	playermodel->SetShader(shader);
	player->addModelComponent(playermodel);
	sm->getActiveScene()->addObject(player);
	/*Shader* shaders = new Shader("../../../../src/vs.vert", "../../../../src/fs.frag");
	Shader* skydomeShader = new Shader("../../../../src/vsS.vert", "../../../../src/fsS.frag");
	GameObject* box = new GameObject("box");
	GameObject* plane = new GameObject("plane");
	GameObject* box2 = new GameObject("box2");
	GameObject* capsule = new GameObject("capsule");
	GameObject* capsule2 = new GameObject("capsule2");
	GameObject* skydome = new GameObject("skydome");
	Model* boxmodel = new Model(const_cast<char*>("../../../../res/box.obj"));
	Model* planemodel = new Model(const_cast<char*>("../../../../res/plane.obj"));
	Model* box2model = new Model(const_cast<char*>("../../../../res/box.obj"));
	Model* capsulemodel = new Model(const_cast<char*>("../../../../res/capsule.obj"));
	Model* capsule2model = new Model(const_cast<char*>("../../../../res/capsule.obj"));
	Mesh* meshSphere = new Mesh();
	meshSphere->createDome(20, 20, 500);
	Model* skydomeModel = new Model(meshSphere);

	boxmodel->SetShader(shaders);
	planemodel->SetShader(shaders);
	box2model->SetShader(shaders);
	capsulemodel->SetShader(shaders);
	capsule2model->SetShader(shaders);
	skydomeModel->SetShader(skydomeShader);
	box->addModelComponent(boxmodel);
	plane->addModelComponent(planemodel);
	box2->addModelComponent(box2model);
	capsule->addModelComponent(capsulemodel);
	capsule2->addModelComponent(capsule2model);
	skydome->addModelComponent(skydomeModel);
	sm->getActiveScene()->addObject(box);
	sm->getActiveScene()->addObject(box2);
	sm->getActiveScene()->addObject(capsule);
	sm->getActiveScene()->addObject(plane);
	sm->getActiveScene()->addObject(capsule2);
	sm->getActiveScene()->addObject(skydome);*/
	int key, action;
	camera->transform->localPosition = glm::vec3(-1.0f, 2.0f, 20.0f);

	const TimePoint tpStart = Clock::now();
	static bool sequenceStarted = false;
	static bool firstKeyPressed = false;
	static bool secondKeyPressed = false;

	/*box->localTransform->localPosition = glm::vec3(-1.f, -1.f, 0.f);
	box2->localTransform->localPosition = glm::vec3(-4.f, -4.f, 0.f);
	capsule->localTransform->localPosition = glm::vec3(4.f, 4.f, 0.f);
	//box2->getModelComponent()->setCustomBox(glm::vec3(-8.f,-8.f,0.f), glm::vec3(8.f,8.f,8.f));
	box->getModelComponent()->addCollider(1, box->localTransform->localPosition);
	capsule->getModelComponent()->addCollider(2, capsule->localTransform->localPosition);
	box2->getModelComponent()->addCollider(1, box->localTransform->localPosition);*/

	//sm->getActiveScene()->findByName("player")->getTransform()->localPosition = glm::vec3(-1.f, -1.f, 1.f);
	/*sm->getActiveScene()->findByName("box")->getTransform()->localPosition = glm::vec3(-1.f, -1.f, 0.f);
	sm->getActiveScene()->findByName("box2")->getTransform()->localPosition = glm::vec3(-4.f, -4.f, 0.f);
	sm->getActiveScene()->findByName("capsule")->getTransform()->localPosition = glm::vec3(4.f, 4.f, 0.f);
	sm->getActiveScene()->findByName("capsule2")->getTransform()->localPosition = glm::vec3(8.f, 8.f, 0.f);
	
	sm->getActiveScene()->findByName("box")->getModelComponent()->addCollider(1, sm->getActiveScene()->findByName("box")->getTransform()->localPosition);
	sm->getActiveScene()->findByName("box2")->getModelComponent()->addCollider(1, sm->getActiveScene()->findByName("box2")->getTransform()->localPosition);
	sm->getActiveScene()->findByName("capsule")->getModelComponent()->addCollider(2, sm->getActiveScene()->findByName("capsule")->getTransform()->localPosition);
	sm->getActiveScene()->findByName("capsule2")->getModelComponent()->addCollider(2, sm->getActiveScene()->findByName("capsule2")->getTransform()->localPosition);
	sm->getActiveScene()->findByName("box2")->setRotating(true);
	sm->getActiveScene()->findByName("plane")->setRotating(true);
	sm->getActiveScene()->findByName("capsule")->setRotating(true);
	sm->getActiveScene()->findByName("skydome")->setRotating(true,100.f,glm::vec3(0.f,1.f,0.f));

	sm->getActiveScene()->findByName("skydome")->getModelComponent()->TextureFromFile("../../../../res/chmury1.png");
	sm->getActiveScene()->findByName("skydome")->getModelComponent()->setTexturePath("../../../../res/chmury1.png");
	sm->activeScene = sm->scenes.at(1);*/
	//sm->getActiveScene()->findByName("player")->getTransform()->localScale = glm::vec3(0.1f, 0.1f, 0.1f);
	float deltaTime = 0;
	float deltaTime2 = 0;
	float lastTime = 0;
	GameMode gameMode;
	CollisionManager cm = CollisionManager(1000, 100);

	while (!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, 1);
		}
		glClearColor(0.2f, 0.3f, 0.3f, 1.f);


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		const float time = std::chrono::duration_cast<Duration>(Clock::now() - tpStart).count();
		deltaTime = time - lastTime;
		deltaTime2 += time - lastTime;
		lastTime = time;
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
		glm::mat4 V(1.f);
		if (gameMode.getMode() == GameMode::Game) {
			pm->ManagePlayer(deltaTime, deltaTime2);
			V = camera->GetViewMatrix();
		}
		else if (gameMode.getMode() != GameMode::Game) {

			V = camera->getViewMatrix();
		}

		glm::mat4 P = glm::perspective(glm::radians(45.f), static_cast<float>(szer) / wys, 1.f, 500.f);
		std::array<glm::vec4, 6> frustumPlanes = calculateFrustumPlanes(glm::perspective(glm::radians(60.f), static_cast<float>(szer) / wys, 1.f, 500.f) * camera->getViewMatrix());

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
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			frustumTest = !frustumTest;
		}
		sm->getActiveScene()->Update(V, P, time);
		//sm->getActiveScene()->findByName("plane")->getModelComponent()->setTransform(glm::rotate(*sm->getActiveScene()->findByName("plane")->getModelComponent()->getTransform(), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f)));
		for (int i = 0; i < sm->getActiveScene()->gameObjects.size(); i++) {
			if (sm->getActiveScene()->gameObjects.at(i)->getModelComponent()->boundingBox != nullptr || sm->getActiveScene()->gameObjects.at(i)->getModelComponent()->capsuleCollider != nullptr)
			{
				cm.addObject(sm->getActiveScene()->gameObjects.at(i));
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
		if (input->IsMove()) {
			glm::vec2 dpos = input->getPosMouse();
			if (glfwGetInputMode(window, GLFW_CURSOR) != 212993) {
				camera->updateCamera(dpos);
			}
		}

		if (input->IsKeobarodAction(window)) {
			input->GetMessage(key, action);
			if (gameMode.getMode() == GameMode::Debug) {

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
		imgui_begin();
		imgui_render(); 
		imgui_end();
		glfwSwapBuffers(window);
		glfwPollEvents();
		if (test) {
			sm->saveScene("first");
			test = false;
			//sm->loadScene("first");
		}
		
	}

	glfwTerminate();
	return 0;
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

void init_imgui()
{
	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// Setup style
	ImGui::StyleColorsDark();
}
void imgui_begin()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void imgui_render()
{
	/// Add new ImGui controls here


	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{

		ImGui::Begin("Opcje");                          // Create a window called "Hello, world!" and append into it.
		for (auto go : sm->activeScene->gameObjects) {
			ImGui::Text(go->name.c_str());
			ImGui::Text("localPosition x: %.2f,y: %.2f,z: %.2f", go->localTransform->getLocalPosition().x, go->localTransform->getLocalPosition().y, go->localTransform->getLocalPosition().z);
			//ImGui::Text("x: %.2f,y: %.2f,z: %.2f",go->localTransform->localPosition.x, go->localTransform->localPosition.y, go->localTransform->localPosition.z);
			if (go->modelComponent->boundingBox != nullptr) {
				//ImGui::Text("localPosition x: %.2f,y: %.2f,z: %.2f", go->localTransform->getLocalPosition().x, go->localTransform->getLocalPosition().y, go->localTransform->getLocalPosition().z);
				ImGui::Text("Min x: %.2f,y: %.2f,z: %.2f", go->modelComponent->boundingBox->min.x, go->modelComponent->boundingBox->min.y, go->modelComponent->boundingBox->min.z);
				ImGui::Text("Max x: %.2f,y: %.2f,z: %.2f", go->modelComponent->boundingBox->max.x, go->modelComponent->boundingBox->max.y, go->modelComponent->boundingBox->max.z);
			}
		}
			
		ImGui::End();
	}
}

void imgui_end()
{
	ImGui::Render();
	int display_w, display_h;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &display_w, &display_h);

	glViewport(0, 0, display_w, display_h);


	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}