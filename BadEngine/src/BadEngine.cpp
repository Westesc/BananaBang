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

#include <random>

bool test = false;
bool frustumTest = false;
int losujLiczbe(int a, int b);
int losujLiczbe2();
bool checkLocations(float x1, float y1,float x2,float y2,float distance);

void setupImGui(GLFWwindow* window);
void renderImGui();
void cleanupImGui();

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
int sectorsPom = 1;
int a = 0;
int b = 0;
bool buttonPressed;
unsigned int maxEnemies = 5;
unsigned int spawnedEnemies = 0;
bool loaded = false;
bool playerAtention = false;


float Lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			 // bottom face
			 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 // top face
			 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			  1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

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

	//glEnable(GL_CULL_FACE);

	//glFrontFace(GL_CCW);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_BLEND);

	//glEnable(GL_STENCIL_TEST);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

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

	glm::vec4 rowX = viewProjectionMatrix[0];
	glm::vec4 rowY = viewProjectionMatrix[1];
	glm::vec4 rowZ = viewProjectionMatrix[2];
	glm::vec4 rowW = viewProjectionMatrix[3];

	// Left plane
	planes[0] = rowW + rowX;
	// Right plane
	planes[1] = rowW - rowX;
	// Top plane
	planes[2] = rowW - rowY;
	// Bottom plane
	planes[3] = rowW + rowY;
	// Near plane
	planes[4] = rowW + rowZ;
	// Far plane
	planes[5] = rowW - rowZ;
	for (auto& plane : planes) {
		float length = glm::length(glm::vec3(plane));
		plane /= length;
	}

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
		bool allOutside = true;
		for (const auto& vertex : vertices) {
			if (glm::dot(glm::vec3(plane), vertex) + plane.w > 0) {
				allOutside = false;
				break;
			}
		}
		if (allOutside) {
			return false;
		}
	}
	return true;
}

bool isCapsuleInFrustum(const std::array<glm::vec4, 6>& frustumPlanes, CapsuleCollider& capsule, glm::mat4 transform) {
	for (const auto& plane : frustumPlanes) {
		glm::vec4 normalizedPlane = plane / glm::length(glm::vec3(plane));
		float distance = glm::dot(glm::vec3(normalizedPlane), glm::vec3(transform * glm::vec4(capsule.center, 1.0f))) + normalizedPlane.w;
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
		performFrustumCulling(frustumPlanes, object->children);
	}
}


int main() {
	Start();
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	//sound test
	//std::cout << "starting...\n";

	//ALCdevice* p_ALCDevice = alcOpenDevice(nullptr); // nullptr = get default device
	//if (!p_ALCDevice)
	//	throw("failed to get sound device");

	//ALCcontext* p_ALCContext = alcCreateContext(p_ALCDevice, nullptr);  // create context
	//if (!p_ALCContext)
	//	throw("Failed to set sound context");

	//if (!alcMakeContextCurrent(p_ALCContext))   // make context current
	//	throw("failed to make context current");

	//const ALCchar* name = nullptr;
	//if (alcIsExtensionPresent(p_ALCDevice, "ALC_ENUMERATE_ALL_EXT"))
	//	name = alcGetString(p_ALCDevice, ALC_ALL_DEVICES_SPECIFIER);
	//if (!name || alcGetError(p_ALCDevice) != AL_NO_ERROR)
	//	name = alcGetString(p_ALCDevice, ALC_DEVICE_SPECIFIER);
	//printf("Opened \"%s\"\n", name);

	//uint32_t /*ALuint*/ sound1 = SoundBuffer::get()->addSoundEffect("../../../../res/media/spell.ogg");
	//uint32_t /*ALuint*/ sound2 = SoundBuffer::get()->addSoundEffect("../../../../res/media/magicfail.ogg");

	//SoundSource mySpeaker;

	//mySpeaker.Play(sound1);
	//mySpeaker.Play(sound2);

	auto animodel = std::make_shared<Model>(const_cast<char*>("../../../../res/animations/Walking.dae"), true); 

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
	Shader* enemyShader = new Shader("../../../../src/shaders/enemy.vert", "../../../../src/shaders/enemy.frag");
	auto enemyModel = std::make_shared<Model>(const_cast<char*>("../../../../res/capsule.obj"), false);
	enemyModel->SetShader(enemyShader);

	Shader shaderGeometryPass("../../../../src/shaders/ssao_geometry.vs", "../../../../src/shaders/ssao_geometry.fs");
	Shader shaderLightingPass("../../../../src/shaders/ssao.vs", "../../../../src/shaders/ssao_lighting.fs");
	Shader shaderSSAO("../../../../src/shaders/ssao.vs", "../../../../src/shaders/ssao.fs");
	Shader shaderSSAOBlur("../../../../src/shaders/ssao.vs", "../../../../src/shaders/ssao_blur.fs");
	Model backpackModel(const_cast<char*>("../../../../res/objects/backpack/backpack.obj"), false);

	unsigned int gBuffer;
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	unsigned int gPosition, gNormal, gAlbedo;
	// position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::windowWidth, Window::windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	// normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::windowWidth, Window::windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	// color + specular color buffer
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Window::windowWidth, Window::windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
	// create and attach depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Window::windowWidth, Window::windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	unsigned int ssaoFBO, ssaoBlurFBO;
	glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
	// SSAO color buffer
	glGenTextures(1, &ssaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Window::windowWidth, Window::windowHeight, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Framebuffer not complete!" << std::endl;
	// and blur stage
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glGenTextures(1, &ssaoColorBufferBlur);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Window::windowWidth, Window::windowHeight, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
	std::default_random_engine generator;
	std::vector<glm::vec3> ssaoKernel;
	for (unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0f;

		scale = Lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	std::vector<glm::vec3> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f);
		ssaoNoise.push_back(noise);
	}
	unsigned int noiseTexture; glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// lighting info
	// -------------
	glm::vec3 lightPosSSAO = glm::vec3(2.0, 4.0, -2.0);
	glm::vec3 lightColorSSAO = glm::vec3(0.2, 0.2, 0.7);

	// shader configuration
	// --------------------
	shaderLightingPass.use();
	shaderLightingPass.setInt("gPosition", 0);
	shaderLightingPass.setInt("gNormal", 1);
	shaderLightingPass.setInt("gAlbedo", 2);
	shaderLightingPass.setInt("ssao", 3);
	shaderSSAO.use();
	shaderSSAO.setInt("gPosition", 0);
	shaderSSAO.setInt("gNormal", 1);
	shaderSSAO.setInt("texNoise", 2);
	shaderSSAOBlur.use();
	shaderSSAOBlur.setInt("ssaoInput", 0);

	GameObject* outlineObj = new GameObject("outline");
	auto outlinemodel = std::make_shared<Model>(const_cast<char*>("../../../../res/Lumberjack.obj"), false);

	outlinemodel->SetShader(shaderTree);
	outlinemodel->SetOutlineShader(outlineShader);
	outlinemodel->SetFillingShader(fillingShader);

	outlineObj->addModelComponent(outlinemodel);
	sm->getActiveScene()->addObject(outlineObj);

	sm->getActiveScene()->findByName("outline")->getTransform()->localPosition = glm::vec3(0.f, 0.f, 0.f);
	sm->getActiveScene()->findByName("outline")->getTransform()->localScale = glm::vec3(1.f, 1.f, 1.f);
	sm->getActiveScene()->findByName("outline")->getTransform()->localRotation = glm::vec3(0.f, 0.f, 0.f);

	//depth shader
	Shader* depthShader = new Shader("../../../../src/shaders/depthShader.vert", "../../../../src/shaders/depthShader.frag");
	Shader* depthAnimationShader = new Shader("../../../../src/shaders/depthAnimationShader.vert", "../../../../src/shaders/depthAnimationShader.frag");

	GameObject* skydome = new GameObject("skydome");

	auto box2model = std::make_shared<Model>(const_cast<char*>("../../../../res/tree.obj"), false);

	Mesh* meshSphere = new Mesh();
	meshSphere->createDome(20, 20, 50);
	auto skydomeModel = std::make_shared<Model>(meshSphere);

	//Fruit
	Mesh* meshFruit = new Mesh();
	meshFruit->createSphere(20, 20, 50);
	auto FruitModel = std::make_shared<Model>(meshFruit);

	//drzewa
	auto treelog = std::make_shared<Model>(const_cast<char*>("../../../../res/objects/trees/tree_log.obj"), false);
    treelog->AddTexture("../../../../res/textures/bark.jpg", "diffuseMap");
	auto treetrunk = std::make_shared<Model>(const_cast<char*>("../../../../res/objects/trees/tree_trunk.obj"), false);
    treetrunk->AddTexture("../../../../res/textures/bark.jpg", "diffuseMap");
	Shader* phongShader = new Shader("../../../../src/shaders/phong.vert", "../../../../src/shaders/phong.frag");
	Shader* phongInstancedShader = new Shader("../../../../src/shaders/phonginstanced.vert", "../../../../src/shaders/phong.frag");
	auto treebranch1= std::make_shared<Model>(const_cast<char*>("../../../../res/objects/trees/tree_branch_1.obj"), false);
    treebranch1->AddTexture("../../../../res/textures/bark.jpg", "diffuseMap");
	treebranch1->SetShader(phongInstancedShader);
	auto planeSectormodel = std::make_shared<Model>(const_cast<char*>("../../../../res/plane.obj"), false);
	planeSectormodel->AddTexture("../../../../res/drewno.png", "diffuseMap");
	treetrunk->SetShader(phongInstancedShader);
	treelog->SetShader(phongInstancedShader);
	planeSectormodel->SetShader(phongShader);

	box2model->SetShader(shaderTree);
	skydomeModel->SetShader(skydomeShader);

	skydome->addModelComponent(skydomeModel);

	skydome->localTransform->localScale=glm::vec3(100.f);

	sm->getActiveScene()->addObject(skydome);
	int key, action;
	camera->transform->localPosition = glm::vec3(-1.0f, 2.0f, 20.0f);

	const TimePoint tpStart = Clock::now();

	setupImGui(window);

	sm->getActiveScene()->findByName("player")->getModelComponent()->AddTexture("../../../../res/bialy.png", "diffuseMap");
	sm->getActiveScene()->findByName("player")->getTransform()->localPosition = glm::vec3(7.f, 1.f, 1.f);
	sm->getActiveScene()->findByName("player")->getTransform()->localScale = glm::vec3(1.f, 1.f, 1.f);

	sm->getActiveScene()->findByName("skydome")->getModelComponent()->AddTexture("../../../../res/chmury1.png","diffuseMap");

	
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

	sm->getActiveScene()->findByName("skydome")->setRotating(true, 1.f, glm::vec3(0.f, 1.f, 0.f));

	float deltaTime = 0;
	float deltaTime2 = 0;
	float lastTime = 0;
	float spawnerTime = 0;
	float staticUpdateTime = 0;
	float sectorSelectorTime = 0;
	GameMode gameMode;
	bool isFromFile = false;
	bool rotating = true;
	bool isBlue = false;


	CollisionManager cm = CollisionManager(1000, 100);
	cm.pm = pm;

	sm->loadScene("first");
	sm->activeScene = sm->scenes.at(0);
	sm->getActiveScene()->addObject(anim);

	Pathfinder* pathfinder = new Pathfinder();
	PBDManager* pbd = new PBDManager(10);
	EnemyStateManager* enemyManager = new EnemyStateManager(pathfinder, &cm, pm);
	std::vector<Transform*> transformsTree;
	std::vector<Transform*> transformsLog;
	std::vector<Transform*> transformsBranch;
	SectorSelector sectorSelector = SectorSelector(&sectorsPom);
	bool regenInstances = false;
	bool ssaoEnabled = true;
	while (!glfwWindowShouldClose(window)) {
		FrameMark;
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, 1);
		}
		glClearColor(0.2f, 0.3f, 0.7f, 1.f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		const float time = std::chrono::duration_cast<Duration>(Clock::now() - tpStart).count();
		deltaTime = time - lastTime;
		deltaTime2 += time - lastTime;
		lastTime = time;
		spawnerTime += deltaTime;
		staticUpdateTime += deltaTime;
		sectorSelectorTime += deltaTime;
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
		std::array<glm::vec4, 6> frustumPlanes = calculateFrustumPlanes(glm::perspective(glm::radians(120.f), static_cast<float>(Window::windowWidth) / Window::windowHeight, 0.1f, 500.f) * V);

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
			else if (input->checkKey(GLFW_KEY_TAB) && input->checkKey(GLFW_KEY_5))
			{
				gameMode.setMode(GameMode::ssaoTest);
				camera->transform->localPosition = glm::vec3(0.0f);
			}
		}

		if (gameMode.getMode() == GameMode::ssaoTest) {
			
			glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glm::mat4 projection = glm::perspective(glm::radians(input->GetZoom()), (float)Window::windowWidth / (float)Window::windowHeight, 0.1f, 50.0f);
			glm::mat4 view = camera->getViewMatrix();
			glm::mat4 model = glm::mat4(1.0f);
			shaderGeometryPass.use();
			shaderGeometryPass.setMat4("projection", projection);
			shaderGeometryPass.setMat4("view", view);
			// room cube
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0, 7.0f, 0.0f));
			model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f));
			shaderGeometryPass.setMat4("model", model);
			shaderGeometryPass.setInt("invertedNormals", 1); // invert normals as we're inside the cube
			renderCube();
			shaderGeometryPass.setInt("invertedNormals", 0);
			// backpack model on the floor
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
			model = glm::scale(model, glm::vec3(1.0f));
			shaderGeometryPass.setMat4("model", model);
			backpackModel.setTransform(model);
			backpackModel.Draw(&shaderGeometryPass);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			if (ssaoEnabled) {
				glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
				glClear(GL_COLOR_BUFFER_BIT);
				shaderSSAO.use();
				// Send kernel + rotation 
				for (unsigned int i = 0; i < 64; ++i)
					shaderSSAO.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
				shaderSSAO.setMat4("projection", projection);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, gPosition);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, gNormal);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, noiseTexture);
				renderQuad();
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
				glClear(GL_COLOR_BUFFER_BIT);
				shaderSSAOBlur.use();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
				renderQuad();
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shaderLightingPass.use();
			// send light relevant uniforms
			glm::vec3 lightPosView = glm::vec3(camera->getViewMatrix() * glm::vec4(lightPosSSAO, 1.0));
			shaderLightingPass.setVec3("light.Position", lightPosView);
			shaderLightingPass.setVec3("light.Color", lightColorSSAO);
			// Update attenuation parameters
			const float linear = 0.09f;
			const float quadratic = 0.032f;
			shaderLightingPass.setFloat("light.Linear", linear);
			shaderLightingPass.setFloat("light.Quadratic", quadratic);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gPosition);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, gNormal);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, gAlbedo);
			if (ssaoEnabled) {
				glActiveTexture(GL_TEXTURE3); // add extra SSAO texture to lighting pass
				glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
			}
			else {
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			renderQuad();

			while (input->IsMove()) {
				glm::vec2 dpos = input->getPosMouse();
				if (glfwGetInputMode(window, GLFW_CURSOR) != 212993) {
					camera->updateCamera(dpos);
				}
			}

			while (input->IsKeobarodAction(window)) {
				input->getMessage(key, action);
				input->getPressKey();
				if (key == GLFW_KEY_I && action == GLFW_PRESS) {
					ssaoEnabled = !ssaoEnabled;
				}
				if ((key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) ||
					(key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) ||
					(key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) ||
					(key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))) {
					switch (key) {
					case GLFW_KEY_W:
						camera->ProcessKeyboard(FORWARD, time);
						break;
					case GLFW_KEY_S:
						camera->ProcessKeyboard(BACKWARD, time);
						break;
					case GLFW_KEY_D:
						camera->ProcessKeyboard(RIGHT, time);
						break;
					case GLFW_KEY_A:
						camera->ProcessKeyboard(LEFT, time);
						break;
					}
					// Debug output for camera position
					std::cout << "Camera Position: " << glm::to_string(camera->transform->localPosition) << std::endl;
				}
				else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
				//jednorazowe
				else if (key == GLFW_MOUSE_BUTTON_RIGHT) {
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					std::cout << "LEFT MOUSE " << key << std::endl;
				}
			}
		}
		else {
			if (gameMode.getMode() == GameMode::Game) {
				pm->ManagePlayer(deltaTime2, deltaTime);
			}

			if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("outline")->getModelComponent()->SetOutlineShader(nullptr);
			}
			if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
				sm->getActiveScene()->findByName("outline")->getModelComponent()->SetOutlineShader(outlineShader);
			}

			if (sm->getActiveScene()->findByName("player")) {
				if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
					sm->getActiveScene()->findByName("player")->velocity += glm::vec3(0.0f, 0.0f, boxSpeed);
				}
				if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
					sm->getActiveScene()->findByName("player")->velocity += glm::vec3(0.0f, 0.0f, -boxSpeed);
				}
				if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
					sm->getActiveScene()->findByName("player")->velocity += glm::vec3(-boxSpeed, 0.0f, 0.0f);
				}
				if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
					sm->getActiveScene()->findByName("player")->velocity += glm::vec3(boxSpeed, 0.0f, 0.0f);
				}
				if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
					if (sm->getActiveScene()->findByName("player")->getTransform()->localPosition.y < 0.1f) {
						sm->getActiveScene()->findByName("player")->velocity += glm::vec3(0.0f, boxSpeed, 0.0f);
					}
				}
				if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
					sm->getActiveScene()->findByName("player")->velocity += glm::vec3(0.0f, -boxSpeed, 0.0f);
				}

				if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
					staticUpdateTime = 0;
					playerAtention = true;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
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
							/*
							transformsBranch.clear();
							transformsLog.clear();
							transformsTree.clear();
							for (auto tree : object->children) {
								if (tree->name.starts_with("tree")) {
									transformsTree.push_back(tree->getTransform());
									transformsLog.push_back(tree->children.at(0)->getTransform());
									for (auto ch : tree->children.at(0)->children)
									{
										transformsBranch.push_back(ch->getTransform());
									}
								}
							}
							if (object->children.size() > 0) {
								for (int i = 0; i < object->children.size(); i++) {
									if (object->children.at(i)->name.starts_with("tree")) {
										object->children.at(i)->getModelComponent().get()->getFirstMesh()->initInstances(transformsTree);
										object->children.at(i)->children.at(0)->getModelComponent().get()->getFirstMesh()->initInstances(transformsLog);
										object->children.at(i)->children.at(0)->children.at(0)->getModelComponent().get()->getFirstMesh()->initInstances(transformsBranch);
										break;
									}
								}
							}
							*/
							regenInstances = true;
							delete treeActual;
							if (object->name.ends_with(std::to_string(sectorSelector.selectedSector)) && object->children.size() == 0 && sectorSelectorTime > 30.0f) {
								sectorSelector.selectSector(1);
							}
							else if (object->name.ends_with(std::to_string(sectorSelector.selectedSector2)) && object->children.size() == 0 && sectorSelectorTime > 30.0f) {
								sectorSelector.selectSector(2);
							}
							sectorSelectorTime = 0;
						}
					}
				}
			}
			if (regenInstances) {
				transformsBranch.clear();
				transformsLog.clear();
				transformsTree.clear();
				for (auto object : sm->getActiveScene()->gameObjects) {
					if (object->name.starts_with("sector")) {
						for (auto tree : object->children) {
							if (tree->name.starts_with("tree")) {
								transformsTree.push_back(tree->getTransform());
								transformsLog.push_back(tree->children.at(0)->getTransform());
								for (auto ch : tree->children.at(0)->children)
								{
									transformsBranch.push_back(ch->getTransform());
								}
							}
						}
					}
				}
				for (int i = 0; i < sectorsPom * sectorsPom; i++) {
					if (sm->getActiveScene()->findByName("sector" + std::to_string(i + 1))->children.size() > 0) {
						GameObject* sector = sm->getActiveScene()->findByName("sector" + std::to_string(i + 1));
						sector->children.at(0)->getModelComponent().get()->getFirstMesh()->initInstances(transformsTree);
						sector->children.at(0)->children.at(0)->getModelComponent().get()->getFirstMesh()->initInstances(transformsLog);
						sector->children.at(0)->children.at(0)->children.at(0)->getModelComponent().get()->getFirstMesh()->initInstances(transformsBranch);
						break;
					}
				}
				regenInstances = false;
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
				if (sm->getActiveScene()->findByName("HPcount")) {
					sm->getActiveScene()->findByName("HPcount")->uiComponent->setText("HP: " + std::to_string(sm->getActiveScene()->findByName("player")->hp));
				}
				cm.addObject(sm->getActiveScene()->findByName("player"));
			}
			for (Enemy* enemy : enemyManager->enemies) {
				if (enemy->hp <= 0) {
					pbd->objects.erase(std::remove(pbd->objects.begin(), pbd->objects.end(), enemy), pbd->objects.end());
					for (auto sect : cm.sections) {
						sect->objects.erase(std::remove(sect->objects.begin(), sect->objects.end(), enemy), sect->objects.end());
					}
					sm->getActiveScene()->gameObjects.erase(std::remove(sm->getActiveScene()->gameObjects.begin(), sm->getActiveScene()->gameObjects.end(), enemy), sm->getActiveScene()->gameObjects.end());
					enemyManager->enemies.erase(std::remove(enemyManager->enemies.begin(), enemyManager->enemies.end(), enemy), enemyManager->enemies.end());
					enemy->chosenTree->getAsActualType<Tree>()->removeChopper(enemy);
					/*if (enemyManager->enemies.size() > 0) {
						std::vector<Transform*> transformsEnemy;
						for (auto enemy : enemyManager->enemies) {
							transformsEnemy.push_back(enemy->getTransform());
						}
						enemyManager->enemies.at(0)->getModelComponent().get()->getFirstMesh()->initInstances(transformsEnemy);
					}*/
					delete enemy;
				}
				else {
					cm.addObject(enemy);
				}
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

			if (sm->getActiveScene()->findByName("skydome")) {
				sm->getActiveScene()->findByName("skydome")->getTransform()->localPosition = camera->transform->localPosition;
			}
			sm->getActiveScene()->Draw(V, P);
			for (auto sector : sm->getActiveScene()->gameObjects) {
				if (sector->name.starts_with("sector")) {
					for (auto tree : sector->children) {
						tree->getModelComponent().get()->GetShader()->use();
						tree->getModelComponent().get()->GetShader()->setVec3("lightPos", lightPos);
						tree->getModelComponent().get()->GetShader()->setVec3("viewPos", camera->transform->getLocalPosition());
						tree->getModelComponent().get()->GetShader()->setVec3("lightColor", *lightColor);
						tree->getModelComponent().get()->GetShader()->setMat4("LSMatrix", lightSpaceMatrix);
						tree->getModelComponent().get()->GetShader()->setMat4("view", V);
						tree->getModelComponent().get()->GetShader()->setMat4("projection", P);
					}
					if (sector->children.size() > 0) {
						sector->children.at(0)->getModelComponent().get()->getFirstMesh()->initInstances(transformsTree);
						sector->children.at(0)->getModelComponent().get()->getFirstMesh()->drawInstances();
						for (auto log : sector->children.at(0)->children) {
							log->getModelComponent().get()->GetShader()->use();
							log->getModelComponent().get()->GetShader()->setVec3("lightPos", lightPos);;
							log->getModelComponent().get()->GetShader()->setVec3("viewPos", camera->transform->getLocalPosition());
							log->getModelComponent().get()->GetShader()->setVec3("lightColor", *lightColor);
							log->getModelComponent().get()->GetShader()->setMat4("LSMatrix", lightSpaceMatrix);
							log->getModelComponent().get()->GetShader()->setMat4("view", V);
							log->getModelComponent().get()->GetShader()->setMat4("projection", P);
							for (auto branch : log->children) {
								branch->getModelComponent().get()->GetShader()->use();
								branch->getModelComponent().get()->GetShader()->setVec3("lightPos", lightPos);;
								branch->getModelComponent().get()->GetShader()->setVec3("viewPos", camera->transform->getLocalPosition());
								branch->getModelComponent().get()->GetShader()->setVec3("lightColor", *lightColor);
								branch->getModelComponent().get()->GetShader()->setMat4("LSMatrix", lightSpaceMatrix);
								branch->getModelComponent().get()->GetShader()->setMat4("view", V);
								branch->getModelComponent().get()->GetShader()->setMat4("projection", P);
							}
						}
						sector->children.at(0)->children.at(0)->getModelComponent().get()->getFirstMesh()->initInstances(transformsLog);
						sector->children.at(0)->children.at(0)->getModelComponent().get()->getFirstMesh()->drawInstances();
						sector->children.at(0)->children.at(0)->children.at(0)->getModelComponent().get()->getFirstMesh()->initInstances(transformsBranch);
						sector->children.at(0)->children.at(0)->children.at(0)->getModelComponent().get()->getFirstMesh()->drawInstances();
					}
				}
			}
			/*if (enemyManager->enemies.size() > 0) {
				for (auto enemy : enemyManager->enemies) {
					enemy->getModelComponent().get()->GetShader()->use();
					enemy->getModelComponent().get()->GetShader()->setMat4("M", *enemy->getModelComponent().get()->getTransform());
					enemy->getModelComponent().get()->GetShader()->setMat4("view", V);
					enemy->getModelComponent().get()->GetShader()->setMat4("projection", P);
				}
				enemyManager->enemies.at(0)->getModelComponent().get()->getFirstMesh()->drawInstances();
			}*/

			shaderTree->use();
			shaderTree->setMat4("view", V);
			shaderTree->setMat4("projection", P);

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
				int sectorcounter = 1;
				for (int i = 0; i < sectorsPom; i++) {
					for (int j = 0; j < sectorsPom; j++) {
						GameObject* planeSector = new GameObject("sector" + std::to_string(sectorcounter));
						planeSector->localTransform->localScale = glm::vec3(2.f, 2.f, 2.f);
						planeSector->addModelComponent(planeSectormodel);
						planeSector->localTransform->localPosition = glm::vec3(i * 20 * planeSector->localTransform->localScale.x, 0.f, j * 20 * planeSector->localTransform->localScale.z);
						int treeCount = losujLiczbe(a, b);
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
							tree->addModelComponent(treetrunk);
							tree->localTransform->localPosition.x = planeSector->localTransform->localPosition.x + treeX;
							tree->localTransform->localPosition.z = planeSector->localTransform->localPosition.z + treeZ;
							tree->addColider(1);
							GameObject* log = new GameObject("log" + std::to_string(k));
							log->addModelComponent(treelog);
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
								branch->addModelComponent(treebranch1);
								branch->localTransform->localPosition.x = planeSector->localTransform->localPosition.x + treeX;
								branch->localTransform->localPosition.y = float(losujLiczbe((m * 13 / branchCount) + 5, ((m + 1) * 13 / branchCount) + 5));
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
								branch->boundingBox = new BoundingBox(glm::vec3(1.1f, 0.0f, 1.1f),
									glm::vec3(3.0f, 0.0f, 3.0f), 0.0f, true);
								//std::cout << branch->localTransform->localRotation.x << std::endl;
								log->addChild(branch);
							}
						}

						sm->activeScene->addObject(planeSector);
						for (auto ch : planeSector->children)
						{
							pathfinder->trees.push_back(std::make_pair(sectorcounter, ch->getAsActualType<Tree>()));
						}
						transformsBranch.clear();
						transformsLog.clear();
						transformsTree.clear();
						for (auto tree : pathfinder->trees) {
							transformsTree.push_back(tree.second->getTransform());
							transformsLog.push_back(tree.second->children.at(0)->getTransform());
							for (auto ch : tree.second->children.at(0)->children)
							{
								transformsBranch.push_back(ch->getTransform());
							}
						}
						if (pathfinder->trees.size() > 0) {
							pathfinder->trees.at(0).second->getModelComponent().get()->getFirstMesh()->initInstances(transformsTree);
							pathfinder->trees.at(0).second->children.at(0)->getModelComponent().get()->getFirstMesh()->initInstances(transformsLog);
							pathfinder->trees.at(0).second->children.at(0)->children.at(0)->getModelComponent().get()->getFirstMesh()->initInstances(transformsBranch);
							pathfinder->sortTrees();
						}
						sectorcounter++;
					}
					loaded = true;
					spawnerTime = 0;
					sectorSelector.selectSector(1);
					sectorSelectorTime = 0;
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

				buttonPressed = false;
				GameObject* HPcount = new GameObject("HPcount");
				UI* ui = new UI(button);
				Shader* UIShader = new Shader("../../../../src/shaders/font.vert", "../../../../src/shaders/font.frag");
				ui->addShader(UIShader);
				ui->setTexture("../../../../res/chmury.png");
				HPcount->localTransform->localPosition = glm::vec3(25.f);
				ui->input = input;
				//ui->onClick = std::bind(&typKlasy::nazwafunkcji,&stworzonaKalsa);
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
				anim->addAnimation(const_cast<char*>("../../../../res/animations/Climbing Up Wall.dae"), "climbing up", 1.3f);
				anim->addAnimation(const_cast<char*>("../../../../res/animations/Climbing Down Wall.dae"), "climbing down", 1.3f);
				anim->addAnimation(const_cast<char*>("../../../../res/animations/Jump Attack.dae"), "tree attack", 0.7f);

				anim->capsuleCollider = new CapsuleCollider(anim->localTransform->localPosition, 0.5f, 2.0f, 1.0f, true);
				pbd->objects.push_back(anim);
				if (sm->getActiveScene()->findByName("player") == nullptr) {
					sm->getActiveScene()->addObject(anim);
					cm.addObject(anim);
				}
				sm->getActiveScene()->addObject(anim);
				sm->getActiveScene()->findByName("player")->Move(glm::vec3(0.f, 2.f, 0.f));
				sm->getActiveScene()->findByName("player")->getTransform()->localScale = glm::vec3(2.f, 2.f, 2.f);
				pm->setGroundPosition(sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition().y);
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
				sm->getActiveScene()->findByName("skydome")->timeSetting(time / 7, glm::vec2(10, 10));

				GameObject* outlineObj = new GameObject("outline");
				auto outlinemodel = std::make_shared<Model>(const_cast<char*>("../../../../res/Lumberjack.obj"), false);

				/*outlinemodel->SetShader(shaderTree);
				outlinemodel->SetOutlineShader(outlineShader);
				outlinemodel->SetFillingShader(fillingShader);

				outlineObj->addModelComponent(outlinemodel);
				sm->getActiveScene()->addObject(outlineObj);

				sm->getActiveScene()->findByName("outline")->getTransform()->localPosition = glm::vec3(0.f, 0.f, 0.f);
				sm->getActiveScene()->findByName("outline")->getTransform()->localScale = glm::vec3(1.f, 1.f, 1.f);
				sm->getActiveScene()->findByName("outline")->getTransform()->localRotation = glm::vec3(0.f, 0.f, 0.f);*/

				sm->getActiveScene()->addObject(HPcount);

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
			/*for (auto object : sm->getActiveScene()->gameObjects) {
				if (object->name.starts_with("sector")) {
					for (auto ch : object->children) {
						if (glm::distance(ch->getTransform()->localPosition, camera->transform->localPosition) > 100.f) {
							if (ch->modelComponent != box2model) {
								ch->modelComponent = box2model;
								ch->children.at(0)->modelComponent = box2model;
								//for (auto branch : ch->children.at(0)->children)
								//{
								//	branch->modelComponent = nullptr;
								//}
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
			}*/
			int pom = 0;
			if (sectorSelectorTime > 30.0f) {
				bool selected = false;
				if (sectorSelector.selectedSector == 0) {
					sectorSelector.selectSector(1);
					selected = true;
				}
				else if (sectorSelector.selectedSector2 == 0) {
					sectorSelector.selectSector(2);
					selected = true;
				}
				if (selected) {
					sectorSelectorTime = 0;
				}
			}
			if (spawnerTime > 8.f && loaded && spawnedEnemies <= maxEnemies) {
				spawnerTime = 0;
				int sector = rand() % 2 + 1;
				if (sectorSelector.selectedSector2 != 0) {
					if (sector == 1) {
						sector = sectorSelector.selectedSector;
					}
					else if (sector == 2) {
						sector = sectorSelector.selectedSector2;
					}
				}
				else {
					sector = sectorSelector.selectedSector;
				}
				Enemy* enemy = new Enemy("enemy" + std::to_string(spawnedEnemies), sm->getActiveScene()->findByName("sector" + std::to_string(sector))->localTransform->localPosition, glm::vec3(0.1f), glm::vec3(0.f), std::make_pair(2.0f, 6.f));
				enemy->Move(glm::vec3(5.0f));
				enemy->sector = sector;
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
				enemy->hp = 30;
				GameObject* enemyWeapon = new GameObject("enemyWeapon" + std::to_string(spawnedEnemies));
				enemyWeapon->addModelComponent(box2model);
				enemyWeapon->getTransform()->localScale = glm::vec3(0.1f);
				enemyWeapon->active = false;
				enemy->addChild(enemyWeapon);
				enemyWeapon->addColider(1);
				enemyWeapon->boundingBox->isTriggerOnly = true;
				enemyWeapon->colliders.push_back(enemyWeapon->boundingBox);
				enemyWeapon = nullptr;
				/*std::vector<Transform*> transformsEnemy;
				for (auto enemy : enemyManager->enemies) {
					transformsEnemy.push_back(enemy->getTransform());
				}
				enemyManager->enemies.at(0)->getModelComponent().get()->getFirstMesh()->initInstances(transformsEnemy);*/
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
