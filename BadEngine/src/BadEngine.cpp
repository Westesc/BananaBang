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

bool test = false;

std::string loadShaderSource(const std::string& _filepath);
GLuint compileShader(const GLchar* _source, GLenum _stage, const std::string& _msg);

using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;

Camera* camera = new Camera();
using Duration = std::chrono::duration<float, std::ratio<1, 1>>;

constexpr int wys = 800, szer = 1000;
GLFWwindow* window;
SceneManager* sm;
Input* input;
float boxSpeed = 4.f;

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
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	sm = new SceneManager();
	//sm->saveScene("first");
	//Scene* scene = new Scene("main");
	//sm->scenes.push_back(scene);
	sm->loadScene("first");
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
}

int main() {
	
	Start();
	/*
	Shader* shaders = new Shader("../../../../src/vs.vert", "../../../../src/fs.frag");
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
	meshSphere->createSphere(20, 20, 50);
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
	camera->transform->localPosition = glm::vec3(-1.0f, 2.0f, 6.0f);

	const TimePoint tpStart = Clock::now();
	static bool sequenceStarted = false;
	static bool firstKeyPressed = false;
	static bool secondKeyPressed = false;
	std::string name = "src";

	//sm->getActiveScene()->findByName("skydome")->getModelComponent()->TextureFromFile("../../../../res/chmury1.png");
	//sm->getActiveScene()->findByName("skydome")->getModelComponent()->setTexturePath("../../../../res/chmury1.png");
	//sm->getActiveScene()->findByName("capsule")->getModelComponent()->TextureFromFile("../../../../res/zdj.png", name);

	/*box->localTransform->localPosition = glm::vec3(-1.f, -1.f, 0.f);
	box2->localTransform->localPosition = glm::vec3(-4.f, -4.f, 0.f);
	capsule->localTransform->localPosition = glm::vec3(4.f, 4.f, 0.f);
	//box2->getModelComponent()->setCustomBox(glm::vec3(-8.f,-8.f,0.f), glm::vec3(8.f,8.f,8.f));
	box->getModelComponent()->addCollider(1, box->localTransform->localPosition);
	capsule->getModelComponent()->addCollider(2, capsule->localTransform->localPosition);
	box2->getModelComponent()->addCollider(1, box->localTransform->localPosition);*/
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
	sm->getActiveScene()->findByName("capsule")->setRotating(true);*/

	float deltaTime = 0;
	float lastTime = 0;

	bool isFromFile = false;
	bool rotating = true;
	bool isBlue = false;

	while (!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, 1);
		}
		glClearColor(0.2f, 0.3f, 0.7f, 1.f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		const float time = std::chrono::duration_cast<Duration>(Clock::now() - tpStart).count();
		deltaTime = time - lastTime;
		lastTime = time;

		glm::mat4 V = camera->getViewMatrix();

		glm::mat4 P = glm::perspective(glm::radians(45.f), static_cast<float>(szer) / wys, 1.f, 5000.f);

		if (sm->getActiveScene()->findByName("skydome")->getModelComponent() != nullptr) {
			glm::mat4 Mm = glm::translate(glm::mat4(1.f), glm::vec3(20.f, 0.f, 18.f));
			Mm = glm::scale(Mm, glm::vec3(50.f, 50.0f, 50.0f));
			Mm = glm::rotate(Mm, glm::radians(time), glm::vec3(0.f, 1.f, 0.f));
			sm->getActiveScene()->findByName("skydome")->getModelComponent()->GetShader()->use();
			sm->getActiveScene()->findByName("skydome")->getModelComponent()->GetShader()->setMat4("M", Mm);
			sm->getActiveScene()->findByName("skydome")->getModelComponent()->GetShader()->setMat4("view", V);
			sm->getActiveScene()->findByName("skydome")->getModelComponent()->GetShader()->setMat4("projection", P);
			sm->getActiveScene()->findByName("skydome")->getModelComponent()->Draw();
		}

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
		//sm->getActiveScene()->Update(V, P, time);
		if (sm->getActiveScene()->findByName("box")->getModelComponent() != nullptr) {
			glm::mat4 M = glm::translate(glm::mat4(1.f), sm->getActiveScene()->findByName("box")->getTransform()->localPosition);
			//M = glm::rotate(M, 100.f * glm::radians(time), glm::vec3(0.f, 0.f, 1.f));
			M = glm::scale(M, glm::vec3(0.1f, 0.1f, 0.1f));
			sm->getActiveScene()->findByName("box")->getModelComponent()->setTransform(&M);
		}

		if (sm->getActiveScene()->findByName("box2")->getModelComponent() != nullptr) {
			glm::mat4 M = glm::translate(glm::mat4(1.f), sm->getActiveScene()->findByName("box2")->getTransform()->localPosition);
			M = glm::rotate(M, 100.f * glm::radians(time), glm::vec3(0.f, 0.f, 1.f));
			M = glm::scale(M, glm::vec3(0.1f, 0.1f, 0.1f));
			sm->getActiveScene()->findByName("box2")->getModelComponent()->setTransform(&M);
		}

		if (sm->getActiveScene()->findByName("capsule")->getModelComponent() != nullptr) {
			glm::mat4 M = glm::translate(glm::mat4(1.f), sm->getActiveScene()->findByName("capsule")->getTransform()->localPosition);
			M = glm::rotate(M, 100.f * glm::radians(time), glm::vec3(0.f, 0.f, 1.f));
			M = glm::scale(M, glm::vec3(0.1f, 0.1f, 0.1f));
			sm->getActiveScene()->findByName("capsule")->getModelComponent()->setTransform(&M);
		}
		if (sm->getActiveScene()->findByName("capsule2")->getModelComponent() != nullptr) {
			glm::mat4 M = glm::translate(glm::mat4(1.f), sm->getActiveScene()->findByName("capsule2")->getTransform()->localPosition);
			//M = glm::rotate(M, 100.f * glm::radians(time), glm::vec3(0.f, 0.f, 1.f));
			M = glm::scale(M, glm::vec3(0.1f, 0.1f, 0.1f));
			sm->getActiveScene()->findByName("capsule2")->getModelComponent()->setTransform(&M);
		}
		if (sm->getActiveScene()->findByName("box")->getModelComponent()->checkCollision(sm->getActiveScene()->findByName("box2")->getModelComponent())) {
			std::cout << "KOLIZJA" << std::endl;
			glm::vec3 displacement = sm->getActiveScene()->findByName("box")->getModelComponent()->calculateCollisionResponse(sm->getActiveScene()->findByName("box2")->getModelComponent())*0.01f;
			std::cout << displacement.x << "," << displacement.y << "," << displacement.z << std::endl;
			if (!(glm::any(glm::isnan(displacement)) || glm::any(glm::isinf(displacement)))) {
				sm->getActiveScene()->findByName("box")->getTransform()->localPosition += displacement;
				sm->getActiveScene()->findByName("box2")->getTransform()->localPosition -= displacement;
			}
		}
		if (sm->getActiveScene()->findByName("box")->getModelComponent()->checkCollision(sm->getActiveScene()->findByName("capsule")->getModelComponent())) {
			std::cout << "KOLIZJA2" << std::endl;
			glm::vec3 displacement = sm->getActiveScene()->findByName("box")->getModelComponent()->calculateCollisionResponse(sm->getActiveScene()->findByName("capsule")->getModelComponent()) * 0.02f;
			std::cout << displacement.x << "," << displacement.y << "," << displacement.z << std::endl;
			if (!(glm::any(glm::isnan(displacement)) || glm::any(glm::isinf(displacement)))) {
				sm->getActiveScene()->findByName("box")->getTransform()->localPosition += displacement;
				sm->getActiveScene()->findByName("capsule")->getTransform()->localPosition -= displacement;
			}
		}
		if (sm->getActiveScene()->findByName("capsule2")->getModelComponent()->checkCollision(sm->getActiveScene()->findByName("capsule")->getModelComponent())) {
			std::cout << "KOLIZJA3" << std::endl;
			glm::vec3 displacement = sm->getActiveScene()->findByName("capsule2")->getModelComponent()->calculateCollisionResponse(sm->getActiveScene()->findByName("capsule")->getModelComponent()) * 0.02f;
			std::cout << displacement.x << "," << displacement.y << "," << displacement.z << std::endl;
			if (!(glm::any(glm::isnan(displacement)) || glm::any(glm::isinf(displacement)))) {
				sm->getActiveScene()->findByName("capsule2")->getTransform()->localPosition += displacement;
				sm->getActiveScene()->findByName("capsule")->getTransform()->localPosition -= displacement;
			}
		}
		if (sm->getActiveScene()->findByName("capsule2")->getModelComponent()->checkCollision(sm->getActiveScene()->findByName("box2")->getModelComponent())) {
			std::cout << "KOLIZJA4" << std::endl;
			glm::vec3 displacement = sm->getActiveScene()->findByName("capsule2")->getModelComponent()->calculateCollisionResponse(sm->getActiveScene()->findByName("box2")->getModelComponent()) * 0.02f;
			std::cout << displacement.x << "," << displacement.y << "," << displacement.z << std::endl;
			if (!(glm::any(glm::isnan(displacement)) || glm::any(glm::isinf(displacement)))) {
				sm->getActiveScene()->findByName("capsule2")->getTransform()->localPosition += displacement;
				sm->getActiveScene()->findByName("box2")->getTransform()->localPosition -= displacement;
			}
		}
		//box
		sm->getActiveScene()->findByName("box")->getModelComponent()->GetShader()->use();
		sm->getActiveScene()->findByName("box")->getModelComponent()->GetShader()->setMat4("M", *sm->getActiveScene()->findByName("box")->getModelComponent()->getTransform());
		sm->getActiveScene()->findByName("box")->getModelComponent()->GetShader()->setMat4("view", V);
		sm->getActiveScene()->findByName("box")->getModelComponent()->GetShader()->setMat4("projection", P);
		sm->getActiveScene()->findByName("box")->getModelComponent()->Draw();
		sm->getActiveScene()->findByName("box")->getModelComponent()->DrawBoundingBoxes(sm->getActiveScene()->findByName("box")->getModelComponent()->GetShader(), *sm->getActiveScene()->findByName("box")->getModelComponent()->getTransform());
		//box2
		sm->getActiveScene()->findByName("box2")->getModelComponent()->GetShader()->use();
		sm->getActiveScene()->findByName("box2")->getModelComponent()->GetShader()->setMat4("M", *sm->getActiveScene()->findByName("box2")->getModelComponent()->getTransform());
		sm->getActiveScene()->findByName("box2")->getModelComponent()->GetShader()->setMat4("view", V);
		sm->getActiveScene()->findByName("box2")->getModelComponent()->GetShader()->setMat4("projection", P);
		sm->getActiveScene()->findByName("box2")->getModelComponent()->Draw();
		sm->getActiveScene()->findByName("box2")->getModelComponent()->DrawBoundingBoxes(sm->getActiveScene()->findByName("box2")->getModelComponent()->GetShader(), *sm->getActiveScene()->findByName("box2")->getModelComponent()->getTransform());
		//capsule
		sm->getActiveScene()->findByName("capsule")->getModelComponent()->GetShader()->use();
		sm->getActiveScene()->findByName("capsule")->getModelComponent()->GetShader()->setMat4("M", *sm->getActiveScene()->findByName("capsule")->getModelComponent()->getTransform());
		sm->getActiveScene()->findByName("capsule")->getModelComponent()->GetShader()->setMat4("view", V);
		sm->getActiveScene()->findByName("capsule")->getModelComponent()->GetShader()->setMat4("projection", P);
		sm->getActiveScene()->findByName("capsule")->getModelComponent()->Draw();
		sm->getActiveScene()->findByName("capsule")->getModelComponent()->UpdateCollider(*sm->getActiveScene()->findByName("capsule")->getModelComponent()->getTransform());
		//capsule2
		sm->getActiveScene()->findByName("capsule2")->getModelComponent()->GetShader()->use();
		sm->getActiveScene()->findByName("capsule2")->getModelComponent()->GetShader()->setMat4("M", *sm->getActiveScene()->findByName("capsule2")->getModelComponent()->getTransform());
		sm->getActiveScene()->findByName("capsule2")->getModelComponent()->GetShader()->setMat4("view", V);
		sm->getActiveScene()->findByName("capsule2")->getModelComponent()->GetShader()->setMat4("projection", P);
		sm->getActiveScene()->findByName("capsule2")->getModelComponent()->Draw();
		sm->getActiveScene()->findByName("capsule2")->getModelComponent()->UpdateCollider(*sm->getActiveScene()->findByName("capsule2")->getModelComponent()->getTransform());
		if (input->IsMove()) {
			glm::vec2 dpos = input->getPosMouse();
			//std::cout << "x: " << dpos.x << " y: " << dpos.y << std::endl;
			camera->updateCamera(dpos);
		}

		if (input->IsKeobarodAction(window)) {
			input->GetMessage(key, action);
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
			else if (key == GLFW_MOUSE_BUTTON_LEFT) {
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
		if (sm->getActiveScene()->findByName("plane")->getModelComponent() != nullptr) {
			glm::mat4 M2 = glm::rotate(glm::mat4(1.f), 100.f * glm::radians(time), glm::vec3(0.f, 0.f, 1.f));
			M2 = glm::rotate(M2, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			M2 = glm::translate(M2, glm::vec3(0.5f, -1.f, -3.f));
			M2 = glm::scale(M2, glm::vec3(0.1f, 0.1f, 0.1f));
			sm->getActiveScene()->findByName("plane")->getModelComponent()->setTransform(&M2);
			sm->getActiveScene()->findByName("plane")->getModelComponent()->GetShader()->use();
			sm->getActiveScene()->findByName("plane")->getModelComponent()->GetShader()->setMat4("M", M2);
			sm->getActiveScene()->findByName("plane")->getModelComponent()->GetShader()->setMat4("view", V);
			sm->getActiveScene()->findByName("plane")->getModelComponent()->GetShader()->setMat4("projection", P);
			sm->getActiveScene()->findByName("plane")->getModelComponent()->Draw();
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
		if (test) {

			sm->saveScene("first");
			test = false;
			sm->loadScene("first");
			
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
