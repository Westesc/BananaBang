#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

struct HitParticle {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float life;

    HitParticle()
        : position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) { }
};

class ParticleSystem {
public:
    std::vector<HitParticle> particles;
    unsigned int maxParticles;
    unsigned int lastUsedParticle;
    unsigned int VAO, VBO;
    Shader* particleShader;
    unsigned int textureID;

    ParticleSystem(unsigned int maxParticles) : maxParticles(maxParticles), lastUsedParticle(0) {
        particles.resize(maxParticles);
        particleShader = new Shader("res/shaders/particle.vert", "res/shaders/particle.frag");
        particleShader->use();
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(std::string("res/hitparticle.png").c_str(), &width, &height, &nrComponents, 0);
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
            std::cout << "Particle texture not loaded" << std::endl;
            stbi_image_free(data);
        }
        this->textureID = textureID;
        initRenderData();
    }

    void update(float deltaTime) {
        for (HitParticle& p : particles) {
            if (p.life > 0.0f) {
                p.life -= deltaTime;
                if (p.life > 0.0f) {
                    p.position += p.velocity * deltaTime;
                    p.color.a -= deltaTime * 2.5f;
                }
            }
        }
    }

    void render(glm::mat4 view, glm::mat4 projection) {
        particleShader->use();
        particleShader->setMat4("view", view);
        particleShader->setMat4("projection", projection);
        particleShader->setInt("particleTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glBindVertexArray(VAO);

        for (HitParticle& p : particles) {
            if (p.life > 0.0f) {
                particleShader->setVec4("particleColor", p.color);

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, p.position);
                model = glm::scale(model, glm::vec3(20.0f));
                particleShader->setMat4("model", model);

                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            }
        }

        glBindVertexArray(0);
    }

    void spawnParticle(glm::vec3 position, glm::vec3 velocity, glm::vec4 color) {
        int index = firstUnusedParticle();
        particles[index].position = position;
        particles[index].velocity = velocity;
        particles[index].color = color;
        particles[index].life = 1.0f;
    }

    int firstUnusedParticle() {
        for (int i = lastUsedParticle; i < maxParticles; ++i) {
            if (particles[i].life <= 0.0f) {
                lastUsedParticle = i;
                return i;
            }
        }
        for (int i = 0; i < lastUsedParticle; ++i) {
            if (particles[i].life <= 0.0f) {
                lastUsedParticle = i;
                return i;
            }
        }
        lastUsedParticle = 0;
        return 0;
    }

    void initRenderData() {
        float particleQuad[] = {
            // positions      // texture Coords
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
             0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 0.0f
        };
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(particleQuad), particleQuad, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};
#endif