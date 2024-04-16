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

	//glFrontFace(GL_CW);

	glEnable(GL_BLEND);
	sm = new SceneManager();
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
}

int main() {
	
	Start();
	
	Shader* shaders = new Shader("../../../../src/vs.vert", "../../../../src/fs.frag");
	GameObject* box = new GameObject("box");
	GameObject* plane = new GameObject("plane");
	GameObject* box2 = new GameObject("box2");
	GameObject* capsule = new GameObject("capsule");
	GameObject* capsule2 = new GameObject("capsule2");
	Model* boxmodel = new Model(const_cast<char*>("../../../../res/modelMonk.obj"));
	Model* planemodel = new Model(const_cast<char*>("../../../../res/plane.obj"));
	Model* box2model = new Model(const_cast<char*>("../../../../res/box.obj"));
	Model* capsulemodel = new Model(const_cast<char*>("../../../../res/capsule.obj"));
	Model* capsule2model = new Model(const_cast<char*>("../../../../res/capsule.obj"));
	boxmodel->SetShader(shaders);
	planemodel->SetShader(shaders);
	box2model->SetShader(shaders);
	capsulemodel->SetShader(shaders);
	capsule2model->SetShader(shaders);
	box->addModelComponent(boxmodel);
	plane->addModelComponent(planemodel);
	box2->addModelComponent(box2model);
	capsule->addModelComponent(capsulemodel);
	capsule2->addModelComponent(capsule2model);
	sm->getActiveScene()->addObject(box);
	sm->getActiveScene()->addObject(box2);
	sm->getActiveScene()->addObject(capsule);
	sm->getActiveScene()->addObject(plane);
	sm->getActiveScene()->addObject(capsule2);
	int key, action;
	camera->transform->localPosition = glm::vec3(-1.0f, 2.0f, 6.0f);

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
	sm->getActiveScene()->findByName("box")->getTransform()->localPosition = glm::vec3(-1.f, -1.f, 0.f);
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
	float deltaTime = 0;
	float lastTime = 0;
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

		glm::mat4 P = glm::perspective(glm::radians(45.f), static_cast<float>(szer) / wys, 1.f, 50.f);

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
		if (box->getModelComponent() != nullptr) {
			glm::mat4 M = glm::translate(glm::mat4(1.f), box->getTransform()->localPosition);
			//M = glm::rotate(M, 100.f * glm::radians(time), glm::vec3(0.f, 0.f, 1.f));
			M = glm::scale(M, glm::vec3(0.1f, 0.1f, 0.1f));
			box->getModelComponent()->setTransform(&M);
		}

		if (box2->getModelComponent() != nullptr) {
			glm::mat4 M = glm::translate(glm::mat4(1.f), box2->getTransform()->localPosition);
			M = glm::rotate(M, 100.f * glm::radians(time), glm::vec3(0.f, 0.f, 1.f));
			M = glm::scale(M, glm::vec3(0.1f, 0.1f, 0.1f));
			box2->getModelComponent()->setTransform(&M);
		}

		if (capsule->getModelComponent() != nullptr) {
			glm::mat4 M = glm::translate(glm::mat4(1.f), capsule->getTransform()->localPosition);
			M = glm::rotate(M, 100.f * glm::radians(time), glm::vec3(0.f, 0.f, 1.f));
			M = glm::scale(M, glm::vec3(0.1f, 0.1f, 0.1f));
			capsule->getModelComponent()->setTransform(&M);
		}
		if (capsule2->getModelComponent() != nullptr) {
			glm::mat4 M = glm::translate(glm::mat4(1.f), capsule2->getTransform()->localPosition);
			//M = glm::rotate(M, 100.f * glm::radians(time), glm::vec3(0.f, 0.f, 1.f));
			M = glm::scale(M, glm::vec3(0.1f, 0.1f, 0.1f));
			capsule2->getModelComponent()->setTransform(&M);
		}
		if (box->getModelComponent()->checkCollision(box2->getModelComponent())) {
			std::cout << "KOLIZJA" << std::endl;
			glm::vec3 displacement = box->getModelComponent()->calculateCollisionResponse(box2->getModelComponent())*0.01f;
			std::cout << displacement.x << "," << displacement.y << "," << displacement.z << std::endl;
			if (!(glm::any(glm::isnan(displacement)) || glm::any(glm::isinf(displacement)))) {
				box->getTransform()->localPosition += displacement;
				box2->getTransform()->localPosition -= displacement;
			}
		}
		if (box->getModelComponent()->checkCollision(capsule->getModelComponent())) {
			std::cout << "KOLIZJA2" << std::endl;
			glm::vec3 displacement = box->getModelComponent()->calculateCollisionResponse(capsule->getModelComponent()) * 0.02f;
			std::cout << displacement.x << "," << displacement.y << "," << displacement.z << std::endl;
			if (!(glm::any(glm::isnan(displacement)) || glm::any(glm::isinf(displacement)))) {
				box->getTransform()->localPosition += displacement;
				capsule->getTransform()->localPosition -= displacement;
			}
		}
		if (capsule2->getModelComponent()->checkCollision(capsule->getModelComponent())) {
			std::cout << "KOLIZJA3" << std::endl;
			glm::vec3 displacement = capsule2->getModelComponent()->calculateCollisionResponse(capsule->getModelComponent()) * 0.02f;
			std::cout << displacement.x << "," << displacement.y << "," << displacement.z << std::endl;
			if (!(glm::any(glm::isnan(displacement)) || glm::any(glm::isinf(displacement)))) {
				capsule2->getTransform()->localPosition += displacement;
				capsule->getTransform()->localPosition -= displacement;
			}
		}
		if (capsule2->getModelComponent()->checkCollision(box2->getModelComponent())) {
			std::cout << "KOLIZJA4" << std::endl;
			glm::vec3 displacement = capsule2->getModelComponent()->calculateCollisionResponse(box2->getModelComponent()) * 0.02f;
			std::cout << displacement.x << "," << displacement.y << "," << displacement.z << std::endl;
			if (!(glm::any(glm::isnan(displacement)) || glm::any(glm::isinf(displacement)))) {
				capsule2->getTransform()->localPosition += displacement;
				box2->getTransform()->localPosition -= displacement;
			}
		}
		shaders->use();
		shaders->setMat4("M", *box->getModelComponent()->getTransform());
		shaders->setMat4("view", V);
		shaders->setMat4("projection", P);
		box->getModelComponent()->Draw();
		box->getModelComponent()->DrawBoundingBoxes(shaders, *box->getModelComponent()->getTransform());
		shaders->use();
		shaders->setMat4("M", *box2->getModelComponent()->getTransform());
		shaders->setMat4("view", V);
		shaders->setMat4("projection", P);
		box2->getModelComponent()->Draw();
		box2->getModelComponent()->DrawBoundingBoxes(shaders, *box2->getModelComponent()->getTransform());
		shaders->use();
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
		capsule2->getModelComponent()->UpdateCollider(*capsule2->getModelComponent()->getTransform());
		if (input->IsMove()) {
			glm::vec2 dpos = input->getPosMouse();
			std::cout << "x: " << dpos.x << " y: " << dpos.y << std::endl;
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
		if (plane->getModelComponent() != nullptr) {
			glm::mat4 M2 = glm::rotate(glm::mat4(1.f), 100.f * glm::radians(time), glm::vec3(0.f, 0.f, 1.f));
			M2 = glm::rotate(M2, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			M2 = glm::translate(M2, glm::vec3(0.5f, -1.f, -3.f));
			M2 = glm::scale(M2, glm::vec3(0.1f, 0.1f, 0.1f));
			plane->getModelComponent()->setTransform(&M2);
			shaders->use();
			shaders->setMat4("M", M2);
			shaders->setMat4("view", V);
			shaders->setMat4("projection", P);
			plane->getModelComponent()->Draw();
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
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
