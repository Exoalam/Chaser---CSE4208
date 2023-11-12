#include "CubeFollower.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

CubeFollower::CubeFollower(const std::vector<glm::vec3>& controlPoints)
    : controlPoints(controlPoints), t(0.0f) {}

glm::mat4 CubeFollower::update(float deltaTime) {
    // Increment t based on deltaTime
    t += deltaTime;  // You will want to modulate this to keep t between 0 and 1

    // Calculate the new position
    glm::vec3 position = calculateBezierPoint(t);
    std::cout << position[0] << position[1] << position[2] << std::endl;
    // Translate the cube to the new position
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);

    return modelMatrix;
}

glm::vec3 CubeFollower::calculateBezierPoint(float t) {
    // Assuming a cubic Bézier curve
    float u = 1 - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    glm::vec3 point = uuu * controlPoints[0]; // First term
    point += 3 * uu * t * controlPoints[1]; // Second term
    point += 3 * u * tt * controlPoints[2]; // Third term
    point += ttt * controlPoints[3]; // Fourth term

    return point;
}
