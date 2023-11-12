#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>
#include <string>
#include "shader.h"

class Skybox {
public:
    Skybox(const std::vector<std::string>& dawnFaces, const std::vector<std::string>& duskFaces);
    ~Skybox();

    void Draw(Shader& shader, float timeOfDay);

private:
    GLuint dawnTextureID;
    GLuint duskTextureID;
    GLuint VAO, VBO;

    void loadCubemapTextures(const std::vector<std::string>& faces, GLuint& textureID);
    void setupSkybox();
};

#endif // SKYBOX_H
