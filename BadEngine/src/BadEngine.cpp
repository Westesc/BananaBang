#include "imgui.h"
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
unsigned int VAO2, VBO2;
void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);
void RenderPlane(Shader& shader, float x, float y, float width, float height, float transparent, glm::vec3 color);
float pasx = 10;
float pasy = 10;
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

	//glEnable(GL_DEPTH_TEST);
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
	Shader* shadersS = new Shader("../../../../src/vsS.vert", "../../../../src/fsS.frag");
	GameObject* box = new GameObject("box");
	GameObject* box2 = new GameObject("box2");
	GameObject* plane = new GameObject("plane");
	GameObject* plane2 = new GameObject("plane");
	GameObject* sphere = new GameObject("sphere");
	GameObject* sphere2 = new GameObject("sphere2");
	Model* boxmodel = new Model(const_cast<char*>("../../../../res/box.obj"));
	Model* boxmodel2 = new Model(const_cast<char*>("../../../../res/box.obj"));
	Model* planemodel = new Model(const_cast<char*>("../../../../res/plane.obj"));
	Model* planemodel2= new Model(const_cast<char*>("../../../../res/plane.obj"));
	Model* spheremodel = new Model(const_cast<char*>("../../../../res/Sphere1.obj"));
	Mesh* meshSphere = new Mesh();
	
	meshSphere->createSphere(100, 100);

	boxmodel->SetShader(shaders);
	boxmodel2->SetShader(shaders);
	planemodel->SetShader(shaders);
	planemodel2->SetShader(shaders);
	spheremodel->SetShader(shaders);

	sphere->addModelComponent(spheremodel);
	box->addModelComponent(boxmodel);
	box2->addModelComponent(boxmodel2);
	plane->addModelComponent(planemodel);
	plane2->addModelComponent(planemodel2);
	sphere->addModelComponent(spheremodel);
	int key, action;
	camera->transform->localPosition = glm::vec3(-1.0f, 2.0f, 20.0f);
	
Shader shader("../../../../res/Shaders/font.vert", "../../../../res/Shaders/font.frag");
Shader hudShader("../../../../res/Shaders/hud.vert", "../../../../res/Shaders/hud.frag");
glm::mat4 projection1 = glm::ortho(0.0f, static_cast<float>(szer), 0.0f, static_cast<float>(wys), -1.0f, 1.0f);
glm::mat4 projection2 = glm::ortho(0.0f, static_cast<float>(szer), 0.0f, static_cast<float>(wys), -1.0f, 1.0f);
shader.use();
glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection1));
hudShader.use();
glUniformMatrix4fv(glGetUniformLocation(hudShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection2));
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

glGenVertexArrays(1, &VAO2);
glGenBuffers(1, &VBO2);
glBindVertexArray(VAO2);
glBindBuffer(GL_ARRAY_BUFFER, VBO2);
glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 2, NULL, GL_DYNAMIC_DRAW);
glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
glBindBuffer(GL_ARRAY_BUFFER, 0);
glBindVertexArray(0);

	const TimePoint tpStart = Clock::now();
	TimePoint last = Clock::now();
	static bool sequenceStarted = false;
	static bool firstKeyPressed = false;
	static bool secondKeyPressed = false;

	//glEnable(GL_CULL_FACE_MODE);
	//glFrontFace(GL_CW);
	glm::vec2 posM = glm::vec2(0.f, 0.f);
	camera->updateCamera(posM);

	bool isFromFile = false; 
	bool rotating = true; 
	bool isBlue = false;

	while (!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, 1);
		}
		glClearColor(0.f, .3f, .5f, 1.f);
		


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		const float time = std::chrono::duration_cast<Duration>(Clock::now() - tpStart).count();
		const float delta = std::chrono::duration_cast<Duration>(Clock::now() - last).count();
		last = Clock::now();
		glm::mat4 V = camera->getViewMatrix();

		glm::mat4 P = glm::perspective(glm::radians(45.f), static_cast<float>(szer) / wys, 1.f, 5000.f);

		glm::mat4 Mm = glm::translate(glm::mat4(1.f), glm::vec3(20.f, 0.f, 18.f) + camera->transform->getLocalPosition());
		Mm = glm::scale(Mm, glm::vec3(50.f, 50.0f, 50.0f));
		Mm = glm::rotate(Mm, 50.f * glm::radians(time), glm::vec3(0.f, 1.f, 0.f));
		shadersS->use();
		shadersS->setMat4("M", Mm);
		shadersS->setMat4("view", V);
		shadersS->setMat4("projection", P);
		meshSphere->Draw(shadersS, &Mm, isFromFile, rotating, isBlue);

		glDisable(GL_BLEND);

		if (box->getModelComponent() != nullptr) {
			glm::mat4 M = glm::translate(glm::mat4(1.f), glm::vec3(15.f, 4.f, 5.f) + camera->transform->getLocalPosition());
			//glm::mat4 M = glm::mat4(1.f);
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
		if (box2->getModelComponent() != nullptr) {
			glm::mat4 M = glm::translate(glm::mat4(1.f), glm::vec3(20.f, 0.f, 18.f));
			//glm::mat4 M = glm::mat4(1.f);
			M = glm::rotate(M, 100.f * glm::radians(time), glm::vec3(0.f, 0.f, 1.f));
			M = glm::rotate(M, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			//M = glm::translate(M, glm::vec3(1.f, 1.f, 0.f));
			M = glm::scale(M, glm::vec3(0.1f, 0.1f, 0.1f));
			box2->getModelComponent()->setTransform(&M);
			shaders->use();
			shaders->setMat4("M", M);
			shaders->setMat4("view", V);
			shaders->setMat4("projection", P);
			box2->getModelComponent()->Draw();
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
		glEnable(GL_BLEND);
		if (plane->getModelComponent() != nullptr) {
			glm::mat4 M2 = glm::translate(glm::mat4(1.f), glm::vec3(10.f, 0.f, 2.f) + camera->transform->getLocalPosition());
			glm::mat4 M = glm::mat4(1.f);
			M2 = glm::rotate(M2, glm::radians(45.0f), glm::vec3(1.0f, 1.0f, 0.0f));
			M2 = glm::rotate(M2, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			M2 = glm::rotate(M2, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			M2 = glm::rotate(M2, 100.f, glm::vec3(0.f, 0.f, 1.f));
			//M = glm::translate(M, glm::vec3(1.f, 1.f, 0.f));
			M2 = glm::scale(M2, glm::vec3(0.1f, 0.1f, 0.1f));
			plane->getModelComponent()->setTransform(&M);
			shaders->use();
			shaders->setMat4("M", M);
			glm::mat4 V2 = glm::mat4(1);
			shaders->setMat4("view", V2);
			shaders->setMat4("projection", projection1);
			plane->getModelComponent()->Draw();
		}
		/*if (sphere->getModelComponent() != nullptr) {

			glm::mat4 M3 = glm::translate(glm::mat4(1.f), glm::vec3(70.f, 0.f, 15.f) + camera->transform->getLocalPosition());
			M3 = glm::rotate(M3, 20.f * glm::radians(time), glm::vec3(0.f, 1.f, 0.f));
			M3 = glm::scale(M3, glm::vec3(2.f, 2.0f, 2.0f));
			sphere->getModelComponent()->setTransform(&M3);

			// Użyj shadera i ustaw macierze widoku i projekcji
			shaders->use();
			shaders->setMat4("M", M3);
			shaders->setMat4("view", V);
			shaders->setMat4("projection", P);

			// Narysuj komponent modelu sfery
			sphere->getModelComponent()->Draw();
		}*/
		///glDisable(GL_DEPTH_TEST);
		RenderText(shader, std::to_string(time), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.5f, 0.5f));
		
		RenderText(shader, "Hello World!", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

		RenderPlane(hudShader, 540.0f, 620.0f, 200.f, 200.f, 0.5f, glm::vec3(0.f, 1.f, 0.f));

		RenderPlane(hudShader, 25.0f, 620.0f, pasx, pasy, 1.f, glm::vec3(pasx/100, 1.f, 0.f));
		pasx += 0.1f  ;
		if (pasx > 200)
			pasx = 10;
		//glEnable(GL_DEPTH_TEST);
		/* Ustawienie macierzy transformacji dla drugiego obiektu
		glProgramUniformMatrix4fv(vs, 0, 1, GL_FALSE, glm::value_ptr(P * V * M2));

		glBindVertexArray(vao2);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
		*/
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}
void RenderPlane(Shader& shader, float x, float y, float width,float height, float transparent, glm::vec3 color)
{
	// activate corresponding render state	
	shader.use();
	glUniform3f(glGetUniformLocation(shader.ID, "col"), color.x, color.y, color.z);
	glUniform1f(glGetUniformLocation(shader.ID, "transparent"), transparent);
	glBindVertexArray(VAO2);

		// update VBO for each character
		float vertices[6][2] = {
			{ x, y+height },
			{ x,y,},
			{ x + width, y},
			{ x,y + height},
			{ x + width, y },
			{ x + width, y + height }
		};
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
}
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