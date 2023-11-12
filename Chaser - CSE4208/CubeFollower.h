#ifndef CUBE_FOLLOWER_H
#define CUBE_FOLLOWER_H

#include <glm/glm.hpp>
#include <vector>

class CubeFollower {
public:
    CubeFollower(const std::vector<glm::vec3>& controlPoints);
    glm::mat4 update(float deltaTime);

private:
    std::vector<glm::vec3> controlPoints;
    float t;  // Parameter along the Bézier curve

    glm::vec3 calculateBezierPoint(float t);
};

#endif // CUBE_FOLLOWER_H
