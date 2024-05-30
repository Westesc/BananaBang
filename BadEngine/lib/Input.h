#ifndef Input_H
#define Input_H

#include "Component.h"
#include <string>
#include "Axis.h"
#include <imgui_impl/imgui_impl_glfw.h>
#include <glm/ext/vector_float2.hpp>
#include <queue>


class Input : Component {
public:
	Input(GLFWwindow* _window);
	~Input();
	Axis getAxis(std::string name);
	void OnInput(GLFWwindow* window);
	bool IsKeobarodAction(GLFWwindow* _window);
	bool IsMove();
	glm::vec2 getPosMouse();
	glm::vec2 getScroll();
	void getMessage(int& key, int& action);
	int getPressKey();
	void executeFunctionPeriodically(int key);
	void putAway(int key, int action);
	bool checkSequence(int key1, int key2);
	bool checkKey(int key);
	bool checkAnyKey();
	float GetZoom();
private:
	static void mouse_callback(GLFWwindow* _window, double xpos, double ypos);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};

#endif
