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

uint16_t f32tof16(float _32)
{
	if (_32 == 0)
	{
		return 0u;
	}
	uint32_t f32;
	memcpy(&f32, &_32, 4);

	uint16_t f16;
	f16 = (f32 >> (23 - 10)) & 0x3ffu;
	uint16_t exp = (f32 >> 23) & 0xff;
	exp -= (127u - 15u);
	exp &= 0x1fu;
	f16 |= (exp << 10);
	f16 |= (((f32 >> 31) & 1u) << 15);
	return f16;
}

/*struct Vertex
{
	uint16_t pos[3];
	uint16_t padding1;
	uint16_t color[3];
	uint16_t padding2;
};

Vertex f32tof16Vertex(float* _v32)
{
	Vertex v;
	v.pos[0] = f32tof16(_v32[0]);
	v.pos[1] = f32tof16(_v32[1]);
	v.pos[2] = f32tof16(_v32[2]);
	v.color[0] = f32tof16(_v32[3]);
	v.color[1] = f32tof16(_v32[4]);
	v.color[2] = f32tof16(_v32[5]);
	return v;
}*/

using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;

using Duration = std::chrono::duration<float, std::ratio<1, 1>>;

constexpr int wys = 800, szer = 1000;
GLFWwindow* window;
SceneManager* sm;
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

	glFrontFace(GL_CW);

	glEnable(GL_BLEND);
	sm = new SceneManager();
	Scene* scene = new Scene("main");
	GameObject* go = new GameObject("test object");
	Camera* cam = new Camera();
	Transform* trans = new Transform();
	Component* comp = new Component();
	UI* ui = new UI();
	AnimateBody* ab = new AnimateBody();
	RigidBody* rb = new RigidBody();
	Axis* axis = new Axis("axis");
	Input* input = new Input();
	go->getTransform();
}

int main() {
	
	Start();
	GameObject* box = new GameObject("box");
	GameObject* plane = new GameObject("plane");
	Model* boxmodel = new Model(const_cast<char*>("../../../../res/box.obj"));
	Model* planemodel = new Model(const_cast<char*>("../../../../res/plane.obj"));
	box->addModelComponent(boxmodel);
	plane->addModelComponent(planemodel);
	GLuint vs = compileShader(loadShaderSource("../../../../src/vs.vert").c_str(), GL_VERTEX_SHADER, "vs log");
	GLuint fs = compileShader(loadShaderSource("../../../../src/fs.frag").c_str(), GL_FRAGMENT_SHADER, "fs log");

	GLuint pipeline;
	glCreateProgramPipelines(1, &pipeline);
	glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vs);
	glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fs);

	/*
	float verticles[]{
		//pozycja     //kolor
		// front
		-0.5f, 0.5f, 0.5f, 1.f, 1.f, 1.f,
		0.5f, 0.5f, 0.5f, 0.f, 0.5f, 1.f,
		0.5f, -0.5f, 0.5f, 0.f, 0.f, 1.f,
		-0.5f, -0.5f, 0.5f, 0.f, 1.f, 0.5f,
		// right
		0.5f, 0.5f, 0.5f, 1.f, 0.f, 0.f,
		0.5f, 0.5f, -0.5f, 0.f, 1.f, 0.f,
		0.5f, -0.5f, -0.5f, 0.f, 0.f, 1.f,
		0.5f, -0.5f, 0.5f, 1.f, 1.f, 0.f,
		//top
		-0.5f, 0.5f, -0.5f, 1.f, 0.f, 0.f,
		0.5f, 0.5f, -0.5f, 0.f, 1.f, 0.f,
		0.5f, 0.5f, 0.5f, 0.f, 0.f, 1.f,
		-0.5f, 0.5f, 0.5f, 1.f, 1.f, 0.f,
		// back
		-0.5f, 0.5f, -0.5f, 1.f, 0.f, 0.f,
		0.5f, 0.5f, -0.5f, 0.f, 1.f, 0.f,
		0.5f, -0.5f, -0.5f, 0.f, 0.f, 1.f,
		-0.5f, -0.5f, -0.5f, 1.f, 1.f, 0.f,
		// left
		-0.5f, 0.5f, 0.5f, 1.f, 0.f, 0.f,
		-0.5f, 0.5f, -0.5f, 0.f, 1.f, 0.f,
		-0.5f, -0.5f, -0.5f, 0.f, 0.f, 1.f,
		-0.5f, -0.5f, 0.5f, 1.f, 1.f, 0.f,
		//bottom
		-0.5f, -0.5f, -0.5f, 1.f, 0.f, 0.f,
		0.5f, -0.5f, -0.5f, 0.f, 1.f, 0.f,
		0.5f, -0.5f, 0.5f, 0.f, 0.f, 1.f,
		-0.5f, -0.5f, 0.5f, 1.f, 1.f, 0.f
	};
	float verticles2[]{
		//pozycja     //kolor
		-0.6f, 0.1f, 1.0f, 1.f, 0.2f, 0.f,
		0.4f, 0.1f, 1.0f, 1.f, 0.2f, 0.f,
		0.4f, -0.1f, 1.0f, 1.f, 0.2f, 1.f,
		-0.6f, -0.1f, 1.0f, 1.f, 0.2f, 0.f,
	};
	Vertex verticles16[24];
	for (size_t i = 0u; i < 24u; i++) {
		verticles16[i] = f32tof16Vertex(verticles + 6 * i);
	}
	//1 bajt
	GLubyte indices[6 * 6]{
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7,
		8, 9, 10,
		8, 10, 11,
		14, 13, 12,
		15, 14, 12,
		18, 17, 16,
		19, 18, 16,
		22, 21, 20,
		23, 22, 20
	};
	GLubyte indices2[6 * 6]{
	0, 1, 2,
	0, 2, 3,
	};
	GLuint vao;
	glCreateVertexArrays(1, &vao);

	GLuint vertexBuffer;
	glCreateBuffers(1, &vertexBuffer);
	glNamedBufferStorage(vertexBuffer, sizeof(verticles16), verticles16, 0);

	GLuint indexBuffer;
	glCreateBuffers(1, &indexBuffer);
	glNamedBufferStorage(indexBuffer, sizeof(indices), indices, 0);


	constexpr GLuint ATTR_POS = 0u;
	glEnableVertexArrayAttrib(vao, ATTR_POS);
	glVertexArrayAttribFormat(vao, ATTR_POS, 3, GL_HALF_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(vao, ATTR_POS, vertexBuffer, offsetof(Vertex, pos), sizeof(Vertex));
	glVertexArrayAttribBinding(vao, ATTR_POS, ATTR_POS);

	constexpr GLuint ATTR_COLOR = 1u;
	glEnableVertexArrayAttrib(vao, ATTR_COLOR);
	glVertexArrayAttribFormat(vao, ATTR_COLOR, 3, GL_HALF_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(vao, ATTR_COLOR, vertexBuffer, offsetof(Vertex, color), sizeof(Vertex));
	glVertexArrayAttribBinding(vao, ATTR_COLOR, ATTR_COLOR);

	glVertexArrayElementBuffer(vao, indexBuffer);

	GLuint vao2;
	glCreateVertexArrays(1, &vao2);

	GLuint vertexBuffer2;
	glCreateBuffers(1, &vertexBuffer2);
	glNamedBufferStorage(vertexBuffer2, sizeof(verticles16), verticles16, 0);

	GLuint indexBuffer2;
	glCreateBuffers(1, &indexBuffer2);
	glNamedBufferStorage(indexBuffer2, sizeof(indices2), indices, 0);


	//constexpr GLuint ATTR_POS = 3u;
	glEnableVertexArrayAttrib(vao2, ATTR_POS);
	glVertexArrayAttribFormat(vao2, ATTR_POS, 3, GL_HALF_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(vao2, ATTR_POS, vertexBuffer2, offsetof(Vertex, pos), sizeof(Vertex));
	glVertexArrayAttribBinding(vao2, ATTR_POS, ATTR_POS);

	//constexpr GLuint ATTR_COLOR = 4u;
	glEnableVertexArrayAttrib(vao2, ATTR_COLOR);
	glVertexArrayAttribFormat(vao2, ATTR_COLOR, 3, GL_HALF_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(vao2, ATTR_COLOR, vertexBuffer2, offsetof(Vertex, color), sizeof(Vertex));
	glVertexArrayAttribBinding(vao2, ATTR_COLOR, ATTR_COLOR);

	glVertexArrayElementBuffer(vao2, indexBuffer2);


	glBindProgramPipeline(pipeline);
	*/

	const TimePoint tpStart = Clock::now();

	while (!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, 1);
		}
		glClearColor(0.2f, 0.3f, 0.7f, 1.f);


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		const float time = std::chrono::duration_cast<Duration>(Clock::now() - tpStart).count();

		glm::mat4 M = glm::translate(glm::mat4(1.f), glm::vec3(-1.f, -1.f, 0.f));

		M = glm::rotate(M, 100.f * glm::radians(time), glm::vec3(0.f, 0.f, 1.f));

		M = glm::translate(M, glm::vec3(1.f, 1.f, 0.f));

		glm::mat4 V = glm::lookAt(glm::vec3(-1.f, 2.f, 6.f), glm::vec3(-1.f, -1.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

		glm::mat4 P = glm::perspective(glm::radians(45.f), static_cast<float>(szer) / wys, 1.f, 50.f);
		if (box->getModelComponent() != nullptr) {
			box->getModelComponent()->Draw();
		}
		/*
		glProgramUniformMatrix4fv(vs, 0, 1, GL_FALSE, glm::value_ptr(P * V * M));

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, nullptr);
		*/
		glm::mat4 M2 = glm::rotate(glm::mat4(1.f), 100.f * glm::radians(time), glm::vec3(0.f, 0.f, 1.f));
		M2 = glm::translate(M2, glm::vec3(0.5f, -1.f, 2.f));

		/* Ustawienie macierzy transformacji dla drugiego obiektu
		glProgramUniformMatrix4fv(vs, 0, 1, GL_FALSE, glm::value_ptr(P * V * M2));

		glBindVertexArray(vao2);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
		*/
		if (plane->getModelComponent() != nullptr) {
			plane->getModelComponent()->Draw();
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//glDeleteBuffers(1, &vertexBuffer);
	//glDeleteVertexArrays(1, &vao);
	glDeleteProgramPipelines(1, &pipeline);
	glDeleteProgram(vs);
	glDeleteProgram(fs);

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
