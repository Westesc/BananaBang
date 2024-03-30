#include "../lib/Input.h"
#include <GLFW/glfw3.h>
#include <iostream>


struct InputMessage {
	int key;
	int action;
};

struct InputMessageMouse {
	glm::vec2 dpos = glm::vec2(0.f, 0.f);
};

std::queue<InputMessage> inputQueue;

std::queue<InputMessageMouse> mouseQueue;


Input::Input(GLFWwindow* _window)
{
	glfwSetKeyCallback(_window, key_callback);
	glfwSetMouseButtonCallback(_window, mouse_button_callback);
	glfwSetCursorPosCallback(_window, mouse_callback);
}

Input::~Input()
{
}

Axis Input::getAxis(std::string name)
{
	return Axis(name);
}

bool isRepeat = false;
bool isRepeat2 = false;
bool canRepeat = false;
int lastKey;

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


void Input::GetMessage(int& key, int& action)
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
	InputMessage message;
	message.key = key;
	canRepeat = true;
	message.action = action;
	inputQueue.push(message);
}

void Input::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	InputMessage message;
	message.key = button;
	message.action = action;
	inputQueue.push(message);
}
