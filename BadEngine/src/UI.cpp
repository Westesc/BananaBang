#include "../lib/UI.h"

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

UI::UI(typeUI type, glm::vec3 color)
{
	this->type = type;
	this->color = color;
	size = glm::vec2(100.f);
	if (type == writing || type == button) {
		loadFreetype();
	}
	else if (type == plane) {
		loadPlane();
	}

}

UI::~UI()
{
}

std::string UI::getText()
{
	return text;
}

void UI::setText(std::string text) {
	this->text = text;
}

bool UI::loadFreetype() {
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return false;
	}

	// find path to font
	std::string font_name = "../../../../res/fonts/arial.ttf";
	if (font_name.empty())
	{
		std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
		return false;
	}

	// load font as face
	FT_Face face;
	if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return false;
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
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return true;
}

void UI::loadPlane()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void UI::addShader(Shader* shader) {
	this->shader = shader;
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Window::windowWidth), 0.0f, static_cast<float>(Window::windowHeight), -1.0f, 1.0f);
	shader->use();
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}
void UI::Draw(Transform* transform) {
	glDisable(GL_DEPTH_TEST);
	if (type == plane || type == button) {
		shader->use();
		shader->setBool("isText", false);
		glUniform3f(glGetUniformLocation(shader->ID, "color"), color.x, color.y, color.z);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);
		float vertices[6][4] = {
				{ transform->localPosition.x,								 transform->localPosition.y + size.y * transform->localScale.y,    0.0f, 0.0f },
				{ transform->localPosition.x,								 transform->localPosition.y,									0.0f, 1.0f },
				{ transform->localPosition.x + size.x* transform->localScale.x, transform->localPosition.y,									1.0f, 1.0f },

				{ transform->localPosition.x,								  transform->localPosition.y + size.y * transform->localScale.y,   0.0f, 0.0f },
				{ transform->localPosition.x + size.x * transform->localScale.x, transform->localPosition.y,									1.0f, 1.0f },
				{ transform->localPosition.x + size.x * transform->localScale.x, transform->localPosition.y + size.y * transform->localScale.y,   1.0f, 0.0f }
		};
		glBindTexture(GL_TEXTURE_2D, planeTexture);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	if (type == writing || type == button) {
		shader->use();
		shader->setBool("isText", true);
		glUniform3f(glGetUniformLocation(shader->ID, "color"), color.x, color.y, color.z);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);
		float x = transform->localPosition.x;
		// iterate through all characters
		std::string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = Characters[*c];

			float xpos = x + ch.Bearing.x * transform->localScale.x;
			float ypos = transform->localPosition.y - (ch.Size.y - ch.Bearing.y) * transform->localScale.y;

			float w = ch.Size.x * transform->localScale.x;
			float h = ch.Size.y * transform->localScale.y;
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
			x += (ch.Advance >> 6) * transform->localScale.x; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glEnable(GL_DEPTH_TEST);
}

void UI::setTexture(std::string path)
{

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	planeTexture = textureID;
}
void UI::update(Transform* transform) {
	if (input != nullptr) {
		glm::vec2 mousePosition = input->getMousePositionOnScreen();
		mousePosition.y = Window::windowHeight - mousePosition.y;
		//printf("x: %.f	y: %.f", mousePosition.x, mousePosition.y);
		if(input->getPressKey()!=-1)
		if(mousePosition.x>transform->getLocalPosition().x && mousePosition.x < transform->getLocalPosition().x + size.x * transform->getLocalScale().x)
			if (mousePosition.y > transform->getLocalPosition().y && mousePosition.y < transform->getLocalPosition().y + size.y * transform->getLocalScale().y) {
				printf("przycisk dzia³a\n");
				//onClick();
			}
	}
}