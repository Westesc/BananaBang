#include "../lib/Input.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <unordered_set>


struct InputMessage {
	int key;
	int action;
};

struct InputMessageMouse {
	glm::vec2 dpos = glm::vec2(0.f, 0.f);
};

std::queue<int> pressKey;

std::queue<InputMessage> inputQueue;

std::queue<InputMessageMouse> mouseQueue;

std::unordered_set<int> pressedKeys;
int lastKey;

float Zoom = 50.0f;

Input::Input(GLFWwindow* _window)
{
	glfwSetKeyCallback(_window, key_callback);
	glfwSetMouseButtonCallback(_window, mouse_button_callback);
	glfwSetCursorPosCallback(_window, mouse_callback);
	glfwSetScrollCallback(_window, scroll_callback);
}

Input::~Input()
{
}

Axis Input::getAxis(std::string name)
{
	return Axis(name);
}

bool Input::IsKeobarodAction(GLFWwindow* _window)
{
	if (!inputQueue.empty()) {
		return true;
	}
	return false;
}

bool Input::IsMove()
{
	if (!mouseQueue.empty()) {
		return true;
	}
	return false;
}


void Input::getMessage(int& key, int& action)
{
	if (!inputQueue.empty()) {
		InputMessage message = inputQueue.front();
		inputQueue.pop();
		key = message.key;
		action = message.action;
	}
}

glm::vec2 Input::getPosMouse()
{
	if (!mouseQueue.empty()) {
		InputMessageMouse imm = mouseQueue.front();
		mouseQueue.pop();
		glm::vec2 pos = imm.dpos;
		return pos;
	}
	return glm::vec2(0.f, 0.f);
}

float Input::GetZoom()
{
	return Zoom;
}

void Input::putAway(int key, int action) {
	InputMessage message;
	message.key = key;
	message.action = action;
	inputQueue.push(message);
}

void Input::mouse_callback(GLFWwindow* _window, double xpos, double ypos)
{
	InputMessageMouse imm;
	static glm::vec2 last;
	static bool first = true;
	if (first)
	{
		last = glm::vec2(xpos, ypos);
		first = false;
	}
	glm::vec2 pos = glm::vec2(xpos, ypos);
	imm.dpos = pos - last;
	last = pos;
	mouseQueue.push(imm);
}


void Input::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key != GLFW_KEY_W && key != GLFW_KEY_A && key != GLFW_KEY_S && key != GLFW_KEY_D) {
		pressKey.push(key);
	}
	if (pressKey.size() > 3) {
		pressKey.pop();
	}

	if (action == GLFW_PRESS) {
		pressedKeys.insert(key);
	}
	else if (action == GLFW_RELEASE) {
		pressedKeys.erase(key);
		lastKey = key;
	}
	InputMessage message;
	message.key = key;
	message.action = action;
	inputQueue.push(message);
}
bool Input::checkAnyKey()
{
	if (pressedKeys.size() > 0)
	{
		return true;
	}
	return false;
}

int Input::getPressKey() {
	if (!pressKey.empty()){
		int k = pressKey.front();
		pressKey.pop();
		return k;
	}
	return -1;
}

bool Input::checkKey(int key)
{
	if (pressedKeys.find(key) != pressedKeys.end()) {
		return true;
	}
	else {
		return false;
	}
}

bool Input::checkSequence(int key1, int key2)
{
	if (lastKey == key1 && checkKey(key2))
	{
		lastKey = key2;
		return true;
	}
	return false;
}

void Input::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		pressedKeys.insert(button);
		
		pressKey.push(button);
		if (pressKey.size() > 3) {
			pressKey.pop();
		}
	}
	else if (action == GLFW_RELEASE) {
		pressedKeys.erase(button);
	}
	InputMessage message;
	message.key = button;
	message.action = action;
	inputQueue.push(message);
}

void Input::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Zoom -= (float)yoffset;
	if (Zoom < 30.0f)
		Zoom = 30.0f;
	if (Zoom > 90.0f)
		Zoom = 90.0f;
}