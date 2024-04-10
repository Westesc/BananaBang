﻿#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <sstream>
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

struct Character {
	unsigned int TextureID; // ID handle of the glyph texture
	glm::ivec2   Size;      // Size of glyph
	glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph
};
std::map<GLchar, Character> Characters;
unsigned int VAO, VBO;
void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);

//std::string loadShaderSource(const std::string& _filepath);
//GLuint compileShader(const GLchar* _source, GLenum _stage, const std::string& _msg);

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

Camera* camera = new Camera();
using Duration = std::chrono::duration<float, std::ratio<1, 1>>;

constexpr int wys = 800, szer = 1000;
GLFWwindow* window;
SceneManager* sm;
Input* input;
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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	sm = new SceneManager();
	Scene* scene = new Scene("main");
	GameObject* go = new GameObject("test object");
	Transform* trans = new Transform();
	Component* comp = new Component();
	UI* ui = new UI();
	AnimateBody* ab = new AnimateBody();
	RigidBody* rb = new RigidBody();
	Axis* axis = new Axis("axis");
	input = new Input(window);
	go->getTransform();
}

int main() {
	
	Start();
	
	Shader* shaders = new Shader("../../../../src/vs.vert", "../../../../src/fs.frag");
	GameObject* box = new GameObject("box");
	GameObject* plane = new GameObject("plane");
	Model* boxmodel = new Model(const_cast<char*>("../../../../res/box.obj"));
	Model* planemodel = new Model(const_cast<char*>("../../../../res/plane.obj"));
	boxmodel->SetShader(shaders);
	planemodel->SetShader(shaders);
	box->addModelComponent(boxmodel);
	plane->addModelComponent(planemodel);
	int key, action;
	camera->transform->localPosition = glm::vec3(-1.0f, 2.0f, 20.0f);
	//camera->transform->localRotation = glm::vec3(0.0f, 45.0f,0.0f);
	/*
	GLuint vs = compileShader(loadShaderSource("../../../../src/vs.vert").c_str(), GL_VERTEX_SHADER, "vs log");
	GLuint fs = compileShader(loadShaderSource("../../../../src/fs.frag").c_str(), GL_FRAGMENT_SHADER, "fs log");

	GLuint pipeline;
	glCreateProgramPipelines(1, &pipeline);
	glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vs);
	glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fs);
	*/
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
Shader shader("../../../../res/Shaders/font.vert", "../../../../res/Shaders/font.frag");
glm::mat4 projection1 = glm::ortho(0.0f, static_cast<float>(szer), 0.0f, static_cast<float>(wys));
shader.use();
glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection1));

// FreeType
// --------
FT_Library ft;
// All functions return a value different than 0 whenever an error occurred
if (FT_Init_FreeType(&ft))
{
	std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	return -1;
}

// find path to font
std::string font_name = "../../../../res/fonty/arial.ttf";
if (font_name.empty())
{
	std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
	return -1;
}

// load font as face
FT_Face face;
if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
	std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	return -1;
}
else {
	// set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);

	// disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// load first 128 characters of ASCII set
	for (unsigned char c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// generate texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<unsigned int>(face->glyph->advance.x)
		};
		Characters.insert(std::pair<char, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}
// destroy FreeType once we're finished
FT_Done_Face(face);
FT_Done_FreeType(ft);


// configure VAO/VBO for texture quads
// -----------------------------------
glGenVertexArrays(1, &VAO);
glGenBuffers(1, &VBO);
glBindVertexArray(VAO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
glBindBuffer(GL_ARRAY_BUFFER, 0);
glBindVertexArray(0);


	const TimePoint tpStart = Clock::now();
	static bool sequenceStarted = false;
	static bool firstKeyPressed = false;
	static bool secondKeyPressed = false;

	//glEnable(GL_CULL_FACE_MODE);
	//glFrontFace(GL_CW);

	while (!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, 1);
		}
		glClearColor(0.f, .3f, .5f, 1.f);


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		const float time = std::chrono::duration_cast<Duration>(Clock::now() - tpStart).count();

		glm::mat4 V = camera->getViewMatrix();

		glm::mat4 P = glm::perspective(glm::radians(45.f), static_cast<float>(szer) / wys, 1.f, 5000.f);
		if (box->getModelComponent() != nullptr) {
			//glm::mat4 M = glm::translate(glm::mat4(1.f), glm::vec3(-1.f, -1.f, 0.f));
			glm::mat4 M = glm::mat4(1.f);
			M = glm::rotate(M, 100.f * glm::radians(time), glm::vec3(0.f, 0.f, 1.f));
			//M = glm::translate(M, glm::vec3(1.f, 1.f, 0.f));
			M = glm::scale(M, glm::vec3(0.1f, 0.1f, 0.1f));
			box->getModelComponent()->setTransform(&M);
			shaders->use();
			shaders->setMat4("M", M);
			shaders->setMat4("view", V);
			shaders->setMat4("projection", P);
			box->getModelComponent()->Draw();
		}

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
			else if (key == GLFW_KEY_D&& action == GLFW_REPEAT) {
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
				std::cout << "Toto " << key << std::endl;
			}
			else if (action == GLFW_RELEASE) {
				std::cout << "Puszczono klawisz " << key << std::endl;
			}
		}
		/*
		glProgramUniformMatrix4fv(vs, 0, 1, GL_FALSE, glm::value_ptr(P * V * M));

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, nullptr);
		*/
		if (plane->getModelComponent() != nullptr) {
			glm::mat4 M2 = glm::rotate(glm::mat4(1.f), 100.f * glm::radians(time), glm::vec3(0.f, 0.f, 1.f));
			M2 = glm::rotate(M2, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			M2 = glm::translate(M2, glm::vec3(0.5f, -1.f, -3.f));
			M2 = glm::scale(M2, glm::vec3(0.1f, 0.1f, 0.1f));
			plane->getModelComponent()->setTransform(&M2);
			shaders->setMat4("M", M2);
			shaders->setMat4("view", V);
			shaders->setMat4("projection", P);
			plane->getModelComponent()->Draw();
		}
		///glDisable(GL_DEPTH_TEST);
		RenderText(shader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.5f, 0.5f));

		RenderText(shader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
		//glEnable(GL_DEPTH_TEST);
		/* Ustawienie macierzy transformacji dla drugiego obiektu
		glProgramUniformMatrix4fv(vs, 0, 1, GL_FALSE, glm::value_ptr(P * V * M2));

		glBindVertexArray(vao2);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
		*/
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//glDeleteBuffers(1, &vertexBuffer);
	//glDeleteVertexArrays(1, &vao);
	//glDeleteProgramPipelines(1, &pipeline);
	//glDeleteProgram(vs);
	//glDeleteProgram(fs);

	glfwTerminate();
	return 0;
}
/*
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
*/
void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
	// activate corresponding render state	
	shader.use();
	glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}