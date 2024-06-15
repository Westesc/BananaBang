#ifndef UI_H
#define UI_H

#include <string>
#include "Component.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <ft2build.h>
#include <functional>
#include FT_FREETYPE_H
#include <glad/glad.h>
#include <iostream>
#include "Globals.h"
#include "Shader.h"
#include "Transform.h"
#include "Input.h"
#include <stb_image.h>

enum typeUI {
	writing,
	plane,
	button,

};

enum location{
	center,
	rightDown,
	leftDown,
	rightUp,
	leftUp

};
struct Character {
	unsigned int TextureID; // ID handle of the glyph texture
	glm::ivec2   Size;      // Size of glyph
	glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph
};

class UI : Component {
private:
	std::string text;
	glm::vec3 color;
	glm::vec2 size;
	unsigned int planeTexture;
	typeUI type;
	std::map<GLchar, Character> Characters;
	location locate = center;
	glm::vec2 padding = glm::vec2(3.f);
	bool loadFreetype();
	void loadPlane();
	Shader* shader;
	unsigned int VAO, VBO;
public:
	Input* input = nullptr;
	std::function<void()> onClick;
	UI(typeUI type,glm::vec3 color = glm::vec3(1.f));
	
	~UI();
	void setText(std::string text);
	void Draw(Transform* transform);
	void setSize(glm::vec2 size);
	void setTexture(std::string path);
	void update(Transform* transform);
	void addShader(Shader* shader);
	std::string getText();
};

#endif
