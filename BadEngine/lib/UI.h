#ifndef UI_H
#define UI_H

#include <string>
#include "Component.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.h>
#include <iostream>
#include "Globals.h"
#include "Shader.h"

enum typeUI {
	writing,
	plane,
	button,

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
	glm::vec2 size;
	glm::vec3 color;
	typeUI type;
	std::map<GLchar, Character> Characters;
	bool loadFreetype();
	Shader* shader;
	unsigned int VAO, VBO;
public:
	UI(typeUI type,glm::vec3 size,glm::vec3 color = glm::vec3(1.f));
	
	~UI();
	void setText(std::string text);
	void Draw();
	void addShader(Shader* shader);
	std::string getText();
};

#endif
