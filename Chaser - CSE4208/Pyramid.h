#ifndef PYRAMID_H
#define PYRAMID_H

#include <string>
#include <glm/glm.hpp>
#include "shader.h"

class Pyramid {
public:
    Pyramid(const glm::mat4& transform, const std::string& texturePath);
    ~Pyramid();

    void draw(Shader& shader);
    void setTransform(glm::mat4 model);

private:
    GLuint VAO, VBO, texture;
    glm::mat4 modelMatrix;
    std::string texturePath;

    // Initialize the pyramid (setup VAO, VBO, texture, etc.)
    void init();

    // Load texture
    GLuint loadTexture(const std::string& filename);
    
};

#endif // PYRAMID_H
