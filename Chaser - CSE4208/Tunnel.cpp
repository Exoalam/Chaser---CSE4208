#include "Tunnel.h"
#include "stb_image.h"
#include <iostream>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif



Tunnel::Tunnel(const std::string& texturePath,
    const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3,
    int numSegments, float tunnelRadius, int circlePoints)
    :texturePath(texturePath),
    p0(p0), p1(p1), p2(p2), p3(p3),
    numSegments(numSegments), tunnelRadius(tunnelRadius), circlePoints(circlePoints) {
    init();
}

Tunnel::~Tunnel() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // Handle texture deletion if necessary
}

void Tunnel::init() {
    // Load texture
    texture = loadTexture(texturePath);

    // Create road mesh
    createMesh();

    // Generate and bind VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Position attribute
// Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);


    glBindVertexArray(0);
}

void Tunnel::draw(Shader& shader, glm::mat4& model) {
    shader.use();
    
    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    shader.setMat4("model", model);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glBindVertexArray(0);
}

GLuint Tunnel::loadTexture(const std::string& filename) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Texture wrapping and filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture: " << filename << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}

void Tunnel::createMesh() {
    auto curvePoints = tessellateBezier();
    float deltaAngle = 2 * M_PI / circlePoints;

    for (size_t i = 0; i < curvePoints.size() - 1; ++i) {
        glm::vec3 forward = glm::normalize(curvePoints[i + 1] - curvePoints[i]);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); // Assuming up direction
        glm::vec3 right = glm::normalize(glm::cross(forward, up));
        up = glm::normalize(glm::cross(right, forward)); // Ensure orthogonality

        for (int j = 0; j < circlePoints; ++j) {
            float angle = j * deltaAngle;
            glm::vec3 localPoint(cos(angle) * tunnelRadius, sin(angle) * tunnelRadius, 0.0f);
            glm::vec3 worldPoint = curvePoints[i] + localPoint.x * right + localPoint.y * up;

            glm::vec3 nextForward = glm::normalize(curvePoints[i + 1] - curvePoints[i]);
            glm::vec3 nextRight = glm::normalize(glm::cross(nextForward, up));
            glm::vec3 nextWorldPoint = curvePoints[i + 1] + localPoint.x * nextRight + localPoint.y * up;

            glm::vec3 normal = glm::normalize(glm::cross(forward, localPoint));

            float texCoordY = i / static_cast<float>(curvePoints.size() - 1) *10;
            float texCoordX = j / static_cast<float>(circlePoints) *10;

            // First triangle of the quad
            vertices.push_back({ worldPoint, normal, {texCoordX, texCoordY} });
            vertices.push_back({ nextWorldPoint, -normal, {texCoordX, texCoordY + 1.0f / (curvePoints.size() - 1)} });

            float nextAngle = (j + 1) % circlePoints * deltaAngle;
            glm::vec3 nextLocalPoint(cos(nextAngle) * tunnelRadius, sin(nextAngle) * tunnelRadius, 0.0f);
            glm::vec3 nextWorldPoint2 = curvePoints[i] + nextLocalPoint.x * right + nextLocalPoint.y * up;
            glm::vec3 nextNormal = glm::normalize(glm::cross(forward, nextLocalPoint));
            vertices.push_back({ nextWorldPoint2, nextNormal, {texCoordX + 1.0f / circlePoints, texCoordY} });

            // Second triangle of the quad
            vertices.push_back({ nextWorldPoint, normal, {texCoordX, texCoordY + 1.0f / (curvePoints.size() - 1)} });
            glm::vec3 nextWorldPoint3 = curvePoints[i + 1] + nextLocalPoint.x * nextRight + nextLocalPoint.y * up;
            vertices.push_back({ nextWorldPoint3, nextNormal, {texCoordX + 1.0f / circlePoints, texCoordY + 1.0f / (curvePoints.size() - 1)} });
            vertices.push_back({ nextWorldPoint2, -nextNormal, {texCoordX + 1.0f / circlePoints, texCoordY} });
        }
    }
}



std::vector<glm::vec3> Tunnel::tessellateBezier() {
    std::vector<glm::vec3> curvePoints;
    for (int i = 0; i <= numSegments; ++i) {
        float t = i / static_cast<float>(numSegments);
        curvePoints.push_back(bezierPoint(p0, p1, p2, p3, t));
    }
    return curvePoints;
}

glm::vec3 Tunnel::bezierPoint(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, float t) {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    glm::vec3 point = uuu * p0;
    point += 3 * uu * t * p1;
    point += 3 * u * tt * p2;
    point += ttt * p3;

    return point;
}
