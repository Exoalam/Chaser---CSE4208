#include "Skybox.h"
#include "stb_image.h"

Skybox::Skybox(const std::vector<std::string>& dawnFaces, const std::vector<std::string>& duskFaces) {
    setupSkybox();
    loadCubemapTextures(dawnFaces, dawnTextureID);
    loadCubemapTextures(duskFaces, duskTextureID);
}

Skybox::~Skybox() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &dawnTextureID);
    glDeleteTextures(1, &duskTextureID);
}

void Skybox::Draw(Shader& shader, float timeOfDay) {
    glDepthMask(GL_FALSE);
    //shader.use();

    shader.setFloat("timeOfDay", timeOfDay);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, dawnTextureID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, duskTextureID);
    shader.setInt("dawnSkybox", 0);
    shader.setInt("duskSkybox", 1);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
}

void Skybox::loadCubemapTextures(const std::vector<std::string>& faces, GLuint& textureID) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    if (textureID == 0) {
        std::cerr << "Skybox texture failed to load." << std::endl;
        // Handle error
    }
    int width, height, nrChannels;
    for (GLuint i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else {
            std::cerr << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // Unbind the texture
}

void Skybox::setupSkybox() {
    GLfloat skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f, // Back top left
        -1.0f, -1.0f, -1.0f, // Back bottom left
         1.0f, -1.0f, -1.0f, // Back bottom right
         1.0f, -1.0f, -1.0f, // Back bottom right
         1.0f,  1.0f, -1.0f, // Back top right
        -1.0f,  1.0f, -1.0f, // Back top left

        -1.0f, -1.0f,  1.0f, // Front bottom left
        -1.0f, -1.0f, -1.0f, // Back bottom left
        -1.0f,  1.0f, -1.0f, // Back top left
        -1.0f,  1.0f, -1.0f, // Back top left
        -1.0f,  1.0f,  1.0f, // Front top left
        -1.0f, -1.0f,  1.0f, // Front bottom left

         1.0f, -1.0f, -1.0f, // Back bottom right
         1.0f, -1.0f,  1.0f, // Front bottom right
         1.0f,  1.0f,  1.0f, // Front top right
         1.0f,  1.0f,  1.0f, // Front top right
         1.0f,  1.0f, -1.0f, // Back top right
         1.0f, -1.0f, -1.0f, // Back bottom right

        -1.0f, -1.0f,  1.0f, // Front bottom left
        -1.0f,  1.0f,  1.0f, // Front top left
         1.0f,  1.0f,  1.0f, // Front top right
         1.0f,  1.0f,  1.0f, // Front top right
         1.0f, -1.0f,  1.0f, // Front bottom right
        -1.0f, -1.0f,  1.0f, // Front bottom left

        -1.0f,  1.0f, -1.0f, // Back top left
         1.0f,  1.0f, -1.0f, // Back top right
         1.0f,  1.0f,  1.0f, // Front top right
         1.0f,  1.0f,  1.0f, // Front top right
        -1.0f,  1.0f,  1.0f, // Front top left
        -1.0f,  1.0f, -1.0f, // Back top left

        -1.0f, -1.0f, -1.0f, // Back bottom left
        -1.0f, -1.0f,  1.0f, // Front bottom left
         1.0f, -1.0f, -1.0f, // Back bottom right
        -1.0f, -1.0f,  1.0f, // Front bottom left
         1.0f, -1.0f,  1.0f, // Front bottom right
         1.0f, -1.0f, -1.0f, // Back bottom right
    };


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

// Definitions for loadCubemapTextures and setupSkybox would go here
