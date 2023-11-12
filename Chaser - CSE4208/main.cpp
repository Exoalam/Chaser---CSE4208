//
//  main.cpp
//  3D Object Drawing
//
//  Created by Nazirul Hasan on 4/9/23.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "basic_camera.h"
#include "pointLight.h"
#include "SpotLight.h"
#include "sphere.h"
#include <iostream>
#include "cube.h"
#include "stb_image.h"
#include "CurvedRoad.h"
#include "Tunnel.h"
#include "Pyramid.h"
#include "Skybox.h"
#include "Cylinder.h"
#include "CubeFollower.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void drawCube(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model, float r, float g, float b);
void drawCube(unsigned int& p1VAO, Shader& lightingShader, glm::mat4 model, float r, float g, float b);
void scene(Cube cube, Shader& lightingShaderWithTexture, glm::mat4 alTogether);


const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// modelling transform
float rotateAngle_X = 0.0;
float rotateAngle_Y = 0.0;
float rotateAngle_Z = 0.0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 1.0;
float translate_X = 0.0;
float translate_Y = 0.0;
float translate_Z = 0.0;
float scale_X = 1.0;
float scale_Y = 1.0;
float scale_Z = 1.0;
float s_ambient = 1;
float s_diffuse = 1;
float s_specular = 1;
float p_ambient = 1;
float p_diffuse = 1;
float p_specular = 1;
float d_ambient = 1;
float d_diffuse = 1;
float d_specular = 1;
float movefr = 4.5;
float movelr = 0;
float jump = .01;
float jump_velocity = 0;
float rtime = 0;
bool jumpup = true;
// camera
Camera camera(glm::vec3(0.0f, 1.1f, 5.2f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float eyeX = 0.0, eyeY = 1.0, eyeZ = 3.0;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);
unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax);



// positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(0,  1.50f,  0.0f),
    //glm::vec3(1.5f,  -1.5f,  0.0f),
    //glm::vec3(-1.5f,  1.5f,  0.0f),
    //glm::vec3(-1.5f,  -1.5f,  0.0f)
};
PointLight pointlight1(

    pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,  // position
    1.0f, 1.0f, 1.0f,     // ambient
    1.0f, 1.0f, 1.0f,      // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    1       // light number
);

SpotLight spotlight1(
    pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,  // position
    1.0f, 1.0f, 1.0f,     // ambient
    1.0f, 1.0f, 1.0f,      // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    1,       // light number
    glm::cos(glm::radians(12.5f)),
    glm::cos(glm::radians(15.0f)),
    0,-1,0
);



// light settings
bool onOffToggle = true;
bool ambientToggle = true;
bool diffuseToggle = true;
bool specularToggle = true;

bool pointlightToggle = true;
bool directionallightToggle = true;
bool spotlightToggle = true;

bool nightmode = true;

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

glm::mat4 transforamtion(float tx, float ty, float tz,float sx, float sy, float sz) {
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    translateMatrix = glm::translate(identityMatrix, glm::vec3(tx, ty, tz));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(sx, sy, sz));
    model = translateMatrix * scaleMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix;
    return model;
}

void tree(Pyramid &pyramid, Shader shader, glm::mat4 model1) {
    glm::mat4 model, identityMatrix, rotateYMatrix, goaround;
    for (int i = 0; i < 360; i += 30) {

        identityMatrix = glm::mat4(1.0f);
        goaround = glm::rotate(identityMatrix, glm::radians(float(i)), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = model1 * transforamtion(0, 1, 0, 1, 1, 1);
        pyramid.setTransform(model * goaround);
        pyramid.draw(shader);
        pyramid.setTransform(model * rotateYMatrix * goaround);
        pyramid.draw(shader);
        model = model1 * transforamtion(0, 1.5, 0, .7, .7, .7);
        pyramid.setTransform(model * goaround);
        pyramid.draw(shader);
        pyramid.setTransform(model * rotateYMatrix * goaround);
        pyramid.draw(shader);
        model = model1 * transforamtion(0, 2, 0, .4, .4, .4);
        pyramid.setTransform(model * goaround);
        pyramid.draw(shader);
        pyramid.setTransform(model * rotateYMatrix * goaround);
        pyramid.draw(shader);
        model = model1 * transforamtion(0, 2.3, 0, .2, .2, .2);
        pyramid.setTransform(model * goaround);
        pyramid.draw(shader);
        pyramid.setTransform(model * rotateYMatrix * goaround);
        pyramid.draw(shader);
        
    }


}


void scene_manager(Cube cube[], glm::mat4 alTogether, Shader lightingShaderWithTexture) {
    float baseHeight = 0.01;
    float width = 2;
    float length = 20;
    glm::mat4 model = glm::mat4(1.0f);
    //Ground
    model = transforamtion(-10, -0.01, -5, width * 10, baseHeight, length);
    model = alTogether * model;
    cube[0].drawCubeWithTexture(lightingShaderWithTexture, model);

    //road
    model = transforamtion(-1, 0, -5, width, baseHeight, length);
    model = alTogether * model;
    cube[1].drawCubeWithTexture(lightingShaderWithTexture, model);

    model = transforamtion(-1.4, 0, -5, width * .2, baseHeight * 10, length);
    model = alTogether * model;
    cube[3].drawCubeWithTexture(lightingShaderWithTexture, model);
    model = transforamtion(1, 0, -5, width * .2, baseHeight * 10, length);
    model = alTogether * model;
    cube[3].drawCubeWithTexture(lightingShaderWithTexture, model);
    model = transforamtion(-1.38, .1, -5, width * .18, baseHeight * .1, length);
    model = alTogether * model;
    cube[2].drawCubeWithTexture(lightingShaderWithTexture, model);
    model = transforamtion(1.02, .1, -5, width * .18, baseHeight * .1, length);
    model = alTogether * model;
    cube[2].drawCubeWithTexture(lightingShaderWithTexture, model);


    


    //building
    for (int i = 0;i < 3;i++) {
        model = transforamtion(-4.4, 0, -5 + i * 3.5, width, baseHeight * 500, 10 * .25);
        model = alTogether * model;
        cube[5].drawCubeWithTexture(lightingShaderWithTexture, model);
        model = transforamtion(2.6, 0, -5 + i * 3.5, width, baseHeight * 500, 10 * .25);
        model = alTogether * model;
        cube[5].drawCubeWithTexture(lightingShaderWithTexture, model);
    }

    //pool
    for (int i = 0; i < 4; i++) {
        model = transforamtion(1.5, 0, 4.5 - i * 3, .05, 2, .05);
        model = alTogether * model;
        cube[6].drawCubeWithTexture(lightingShaderWithTexture, model);
    }
    for (int i = 0; i < 4; i++) {
        model = transforamtion(-1.5, 0, 4.5 - i * 3, .05, 2, .05);
        model = alTogether * model;
        cube[6].drawCubeWithTexture(lightingShaderWithTexture, model);
    }
    for (int i = 0; i < 4; i++) {
        model = transforamtion(1.5, 1.95, 4.5 - i * 3, -.95, .05, .05);
        model = alTogether * model;
        cube[6].drawCubeWithTexture(lightingShaderWithTexture, model);
    }
    for (int i = 0; i < 4; i++) {
        model = transforamtion(-1.5, 1.95, 4.5 - i * 3, 1, .05, .05);
        model = alTogether * model;
        cube[6].drawCubeWithTexture(lightingShaderWithTexture, model);
    }
    for (int i = 0;i < 3;i++) {
        for (int j = 0; j < 3; j++) {
            model = transforamtion(-2.4, .5 + j * 1.5, -3.95 + i * 3.5, .01, .8, .5);
            model = alTogether * model;
            cube[7].drawCubeWithTexture(lightingShaderWithTexture, model);
            //ourShader.setMat4("model", model);
            //if (pointlightToggle) {
            //    ourShader.setVec3("color", glm::vec3(0.722, 0.71, 0.161));
            //}
            //else {
            //    ourShader.setVec3("color", glm::vec3(0.122, 0.118, 0.035));
            //}
            //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

    }
    for (int i = 0;i < 3;i++) {
        for (int j = 0; j < 3; j++) {
            model = transforamtion(2.59, .5 + j * 1.5, -3.95 + i * 3.5, .01, .8, .5);
            model = alTogether * model;
            cube[7].drawCubeWithTexture(lightingShaderWithTexture, model);
            //ourShader.setMat4("model", model);
            //if (pointlightToggle) {
            //    ourShader.setVec3("color", glm::vec3(0.722, 0.71, 0.161));
            //}
            //else {
            //    ourShader.setVec3("color", glm::vec3(0.122, 0.118, 0.035));
            //}

            //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
    }

}


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CSE 4208: Computer Graphics Laboratory", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("vertexShaderForPhongShading.vs", "fragmentShaderForPhongShading.fs");
    Shader lightingShaderWithTexture("vertexShaderForPhongShadingWithTexture.vs", "fragmentShaderForPhongShadingWithTexture.fs");                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
    //Shader lightingShader("vertexShaderForGouraudShading.vs", "fragmentShaderForGouraudShading.fs");
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");
    Shader skyboxShader("skyboxvshader.vs", "skyboxshader.fs");

    // ... setup the rest of your application ...

    // When drawing the skybox



    string droadpath = "Textures/road.png";
    string sroadpath = "Textures/road.png";
    unsigned int droad = loadTexture(droadpath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int sroad = loadTexture(sroadpath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube = Cube(droad, sroad, 32.0f, 0.0f, 0.0f, 1.0f, 4.0f);
    Cube road = Cube(droad, sroad, 32.0f, 0.0f, 0.0f, 1.0f, 4.0f);

    string dgrasspath = "Textures/grass.png";
    string sgrasspath = "Textures/grass.png";
    unsigned int dgrass = loadTexture(dgrasspath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int sgrass = loadTexture(sgrasspath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube grass = Cube(dgrass, sgrass, 32.0f, 0.0f, 0.0f, 8.0f, 8.0f);

    string db1path = "Textures/dbuilding1.jpg";
    string sb1path = "Textures/dbuilding1.jpg";
    unsigned int db1 = loadTexture(db1path.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int sb1 = loadTexture(sb1path.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube b1 = Cube(db1, sb1, 32.0f, 0.0f, 0.0f, 4.0f, 4.0f);


    string dppath = "Textures/dpool.jpg";
    string sppath = "Textures/spool.jpg";
    unsigned int dp = loadTexture(dppath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int sp = loadTexture(sppath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube pool = Cube(dp, sp, 32.0f, 0.0f, 0.0f, 0.5f, 10.0f);

    string dfpath = "Textures/footpath.jpg";
    string sfpath = "Textures/footpath.jpg";
    unsigned int df = loadTexture(dfpath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int sf = loadTexture(sfpath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube footpath = Cube(df, sf, 32.0f, 0.0f, 0.0f, 0.5f, 10.0f);

    string df2path = "Textures/footpath2.jpg";
    string sf2path = "Textures/footpath2.jpg";
    unsigned int df2 = loadTexture(df2path.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int sf2 = loadTexture(sf2path.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube footpath2 = Cube(df2, sf2, 32.0f, 0.0f, 0.0f, 20.0f, 1.0f);

    string dwpath = "Textures/window.png";
    string swpath = "Textures/window.png";
    unsigned int dw = loadTexture(dwpath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int sw = loadTexture(swpath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube win = Cube(dw, sw, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string dplayerpath = "Textures/dplayer.jpg";
    string splayerpath = "Textures/splayer.jpg";
    unsigned int dplayer = loadTexture(dplayerpath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int splayer = loadTexture(splayerpath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube player = Cube(dplayer, splayer, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string forestgpath = "Textures/forestground.jpg";
    unsigned int fgp = loadTexture(forestgpath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube forestg = Cube(fgp, fgp, 32.0f, 0.0f, 0.0f, 10.0f, 10.0f);

    string treepath = "Textures/tree.jpg";

    Cube cube_array[] = {grass, road, footpath, footpath2, player, b1, pool, win};
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    float cube_vertices[] = {
        // positions      // normals
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f
    };

    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };

    unsigned int cubeVAO, cubeVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

   
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



    //ourShader.use();
    //lightingShader.use();

    // render loop
    // -----------
    glm::mat4 rotateXMatrix, rotateYMatrix, rotateZMatrix;
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
    SpotLight spotlight[8];
    PointLight pointlight[9];
    PointLight pointlight2[9];
    Sphere sphere = Sphere();
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 model = transforamtion(0, 0, -10, 1, 1, 1) * rotateXMatrix;
    glm::vec3 p0(0, 5, 0);
    glm::vec3 p1(0, 2, 0);
    glm::vec3 p2(7, 0, 0);
    glm::vec3 p3(7, -10, 0);
    int numSegments = 1000;
    float roadWidth = 1.0f;
    CurvedRoad cr(model, sroadpath, p0, p1, p2, p3, numSegments, roadWidth);

    model = transforamtion(0, 0, -10, 1, 1, 1);
    Tunnel tunnel(model * rotateXMatrix, "Textures/tunnel.jpg", p0, p1, p2, p3,
        1000, 1.2f, 16);

    p0 = glm::vec3(0, 5, 0);
    p1 = glm::vec3(0, 2, 0);
    p2 = glm::vec3(-7, 0, 0);
    p3 = glm::vec3(-7, -10, 0);
    model = transforamtion(7, 0, -25, 1, 1, 1) * rotateXMatrix;
    CurvedRoad cr2(model, sroadpath, p0, p1, p2, p3, numSegments, roadWidth);
    model = transforamtion(7, 0, -25, 1, 1, 1);
    Tunnel tunnel2(model* rotateXMatrix, "Textures/tunnel.jpg", p0, p1, p2, p3,
        1000, 1.2f, 16);

    // In your render loop
    
    glm::mat4 pyramidTransform = transforamtion(0, 5, 0, 5, 5, 5);
    std::string texturePath = treepath;

    Pyramid pyramid(pyramidTransform, texturePath);
    Pyramid pyramid2(pyramidTransform, "Textures/hill.jpg");

    std::vector<std::string> dawnFaces = { "Textures/grass.png","Textures/grass.png","Textures/grass.png","Textures/grass.png","Textures/grass.png","Textures/grass.png" };
    std::vector<std::string> duskFaces = { "Textures/grass.png","Textures/grass.png","Textures/grass.png","Textures/grass.png","Textures/grass.png","Textures/grass.png" };

    Skybox skybox(dawnFaces, duskFaces);
  
    Cylinder cylinder(.15, .1, 1, 16, 20, "Textures/treebase.png");

    while (!glfwWindowShouldClose(window))
    {
        
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        // Colors for dawn and dusk
        glm::vec4 dawnColor(0.3f, 0.3f, 0.5f, 1.0f); // Soft blue
        glm::vec4 duskColor(0.1f, 0.1f, 0.2f, 1.0f); // Darker blue

        // Calculate the interpolation factor based on time
        // Assuming 'time' is a float that goes from 0.0 at dawn to 1.0 at dusk
        float time = (sin(glfwGetTime()) + 1.0f) / 2.0f; // Example time value that oscillates between 0 and 1

        // Interpolate between dawn and dusk colors
        glm::vec4 currentColor = dawnColor * (1.0f - time) + duskColor * time;

        // Set the clear color using the current interpolated color
        glClearColor(currentColor.r, currentColor.g, currentColor.b, currentColor.a);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        //glm::mat4 view = basic_camera.createViewMatrix();
        //glDepthFunc(GL_LEQUAL);
        //skyboxShader.setVec3("viewPos", camera.Position);
        //skyboxShader.setMat4("view", view);
        //skyboxShader.setMat4("projection", projection);
        //float timeOfDay = .5f;
        //skybox.Draw(skyboxShader, timeOfDay);
        //glDepthFunc(GL_LESS);


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShaderWithTexture.use();
        lightingShaderWithTexture.setVec3("viewPos", camera.Position);

        // point light 1
        //pointlight1.setUpPointLight(lightingShader);
        for (int i = 0; i < 4; i++) {
            spotlight[i].position = glm::vec3(.5, 1.95, 4.5 - i * 3);
            spotlight[i].Number = i;
            spotlight[i].s_ambient = s_ambient;
            spotlight[i].s_diffuse = s_diffuse;
            spotlight[i].s_specular = s_specular;
            spotlight[i].setUpspotLight(lightingShaderWithTexture);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            spotlight[i+4].position = glm::vec3(-.5, 1.95, 4.5 - i * 3);
            spotlight[i+4].Number = i+4;
            spotlight[i+4].s_ambient = s_ambient;
            spotlight[i+4].s_diffuse = s_diffuse;
            spotlight[i+4].s_specular = s_specular;
            spotlight[i+4].setUpspotLight(lightingShaderWithTexture);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
        for (int i = 0;i < 9;i+=3) {
            for (int j = 0; j < 3; j++) {
                pointlight[j+i].position = glm::vec3(-2.4, 1 + j * 1.5, -3.7 + i * 3.5);
                pointlight[j+i].Number = j+i;
                pointlight[j + i].p_ambient = p_ambient;
                pointlight[j + i].p_diffuse = p_diffuse;
                pointlight[j + i].p_specular = p_specular;
                pointlight[j+i].setUpPointLight(lightingShaderWithTexture);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
        }
        for (int i = 0;i < 9;i += 3) {
            for (int j = 0; j < 3; j++) {
                pointlight2[j + i].position = glm::vec3(2.59, 1 + j * 1.5, -3.7 + i * 3.5);
                pointlight2[j + i].Number = j + i;
                pointlight2[j + i].p_ambient = p_ambient;
                pointlight2[j + i].p_diffuse = p_diffuse;
                pointlight2[j + i].p_specular = p_specular;
                pointlight2[j + i].setUpPointLight(lightingShaderWithTexture);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }

        }

        // point light 2
        //pointlight2.setUpPointLight(lightingShader);
        //// point light 3
        //pointlight3.setUpPointLight(lightingShader);
        //// point light 4
        //pointlight4.setUpPointLight(lightingShader);

        // activate shader

        lightingShaderWithTexture.use();
        
        // pass projection matrix to shader (note that in this case it could change every frame)
        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        lightingShaderWithTexture.setMat4("projection", projection);

        // camera/view transformation
        //glm::mat4 view = camera.GetViewMatrix();
        //glm::mat4 view = basic_camera.createViewMatrix();
        lightingShaderWithTexture.setMat4("view", view);

        glm::vec3 d_a = glm::vec3(0.2f, 0.2f, 0.2f) * d_ambient;
        glm::vec3 d_d = glm::vec3(0.7f, 0.7f, 0.7f) * d_diffuse;
        glm::vec3 d_s = glm::vec3(1.0f, 1.0f, 1.0f) * d_specular;
        lightingShaderWithTexture.setVec3("direcLight.direction", 0.5f, -3.0f, -3.0f);
        lightingShaderWithTexture.setVec3("direcLight.ambient", d_a);
        lightingShaderWithTexture.setVec3("direcLight.diffuse", d_d);
        lightingShaderWithTexture.setVec3("direcLight.specular", d_s);
        lightingShaderWithTexture.setBool("dlighton", directionallightToggle);


        glm::vec3 e_a = glm::vec3(0.2f, 0.2f, 0.2f) * d_ambient;
        glm::vec3 e_d = glm::vec3(0.7f, 0.7f, 0.7f) * d_diffuse;
        glm::vec3 e_s = glm::vec3(1.0f, 1.0f, 1.0f) * d_specular;
        lightingShaderWithTexture.setVec3("emissionlight.position", -5, 30, -30);
        lightingShaderWithTexture.setVec3("emissionlight.ambient", e_a);
        lightingShaderWithTexture.setVec3("emissionlight.diffuse", e_d);
        lightingShaderWithTexture.setVec3("emissionlight.specular", e_s);
        lightingShaderWithTexture.setFloat("emissionlightk_c", 1);
        lightingShaderWithTexture.setFloat("emissionlight.k_l", .09);
        lightingShaderWithTexture.setFloat("emissionlight.k_q", .032);
        lightingShaderWithTexture.setVec3("emissionlight.emission", 0.2f, 0.2f, 0.2f);
        lightingShaderWithTexture.setBool("elighton", false);
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 alTogether = transforamtion(0, 0, 0, 1, 1, 1);
        if (jump >= .02) {
            rtime += deltaTime;
            if (jump <= 1 && jumpup) {
                jump = jump_velocity + 2.5 * rtime;
            }
            else if (jump > 1 && jumpup) {
                jumpup = false;
                rtime = 0;
                jump_velocity = 1;
            }
            else if (!jumpup) {
                jump = jump_velocity - 2.5 * rtime;
            }

        }
        else {
            jump = .01;
            rtime = 0;
            jumpup = true;
        }
        //model = transforamtion(movelr, jump, movefr, .5, .5, .5);
        float playerx = camera.Position.x;
        if (playerx < -1)
            playerx = -1;
        if (playerx > .5f)
            playerx = .5;
        model = transforamtion(playerx, jump, camera.Position.z-3, .5, .5, .5);
        //model = transforamtion(0, jump, 10, .5, .5, .5);
        //model *=  cubefollower.update(deltaTime/20);
        cube_array[4].drawCubeWithTexture(lightingShaderWithTexture, model);
        scene_manager(cube_array, alTogether, lightingShaderWithTexture);
        //alTogether = transforamtion(0, 0, -11, 1, 1, 1);
        //scene_manager(cube_array, alTogether, lightingShaderWithTexture);
        //
        //model = transforamtion(0, 0, 0, 1, 1, 1);

        model = transforamtion(-10, -0.02, -35, 20, .01, 30);
        forestg.drawCubeWithTexture(lightingShaderWithTexture, model);
        
        cr.draw(lightingShaderWithTexture);
        cr2.draw(lightingShaderWithTexture);
        tunnel.draw(lightingShaderWithTexture);
        tunnel2.draw(lightingShaderWithTexture);
        //for (int i = 0; i < 25; i+=4) {
        //    model = transforamtion(-8, 0, -30 + i, 1, 1, 1);
        //    tree(pyramid, lightingShaderWithTexture, model);
        //    model = transforamtion(-8, .5, -30 + i, 1, 1, 1);
        //    cylinder.Draw(lightingShaderWithTexture, model);
        //}
        for (int i = 0; i < 25; i += 5) {
            model = transforamtion(-8, 0, -30 + i, 1, 1, 1);
            tree(pyramid, lightingShaderWithTexture, model);
            model = transforamtion(-8, .5, -30 + i, 1, 1, 1);
            cylinder.Draw(lightingShaderWithTexture, model);
        }
        for (int i = 0; i < 25; i += 5) {
            model = transforamtion(-8 + 17, 0, -30 + i, 1, 1, 1);
            tree(pyramid, lightingShaderWithTexture, model);
            model = transforamtion(-8 + 17, .5, -30 + i, 1, 1, 1);
            cylinder.Draw(lightingShaderWithTexture, model);
        }
        glm::mat4 model1 = glm::mat4(1.0f);
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = model1 * transforamtion(-2, 0, -15, 8, 8, 8);
        pyramid2.setTransform(model);
        pyramid2.draw(lightingShaderWithTexture);
        pyramid2.setTransform(model * rotateYMatrix);
        pyramid2.draw(lightingShaderWithTexture);
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = model1 * transforamtion(-3, 0, -11, 5, 5, 5);
        pyramid2.setTransform(model);
        pyramid2.draw(lightingShaderWithTexture);
        pyramid2.setTransform(model * rotateYMatrix);
        pyramid2.draw(lightingShaderWithTexture);
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = model1 * transforamtion(-4, 0, -18, 6, 6, 6);
        pyramid2.setTransform(model);
        pyramid2.draw(lightingShaderWithTexture);
        pyramid2.setTransform(model * rotateYMatrix);
        pyramid2.draw(lightingShaderWithTexture);
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = model1 * transforamtion(-4, 0, -22, 4, 4, 4);
        pyramid2.setTransform(model);
        pyramid2.draw(lightingShaderWithTexture);
        pyramid2.setTransform(model * rotateYMatrix);
        pyramid2.draw(lightingShaderWithTexture);
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = model1 * transforamtion(-1, 0, -20, 3, 3, 3);
        pyramid2.setTransform(model);
        pyramid2.draw(lightingShaderWithTexture);
        pyramid2.setTransform(model * rotateYMatrix);
        pyramid2.draw(lightingShaderWithTexture);

        model = transforamtion(-2, 1, -9, .5, .5, .5);
        tree(pyramid, lightingShaderWithTexture, model);
        model = transforamtion(-2, 1.3, -9, .5, .5, .5);
        cylinder.Draw(lightingShaderWithTexture, model);
        model = transforamtion(-2, 2.5, -12, .5, .5, .5);
        tree(pyramid, lightingShaderWithTexture, model);
        model = transforamtion(-2, 2.8, -12, .5, .5, .5);
        cylinder.Draw(lightingShaderWithTexture, model);
        model = transforamtion(-.1, 4, -15, .5, .5, .5);
        tree(pyramid, lightingShaderWithTexture, model);
        model = transforamtion(-.1, 4.5, -15, .5, .5, .5);
        cylinder.Draw(lightingShaderWithTexture, model);
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        //// we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = transforamtion(.5, 1.95, 4.5 - i * 3, .05, .05, .05);
            ourShader.setMat4("model", model);
            if (spotlightToggle){
                ourShader.setVec3("color", glm::vec3(0.8f, 0.8f, 0.8f));
            }
            else {
                ourShader.setVec3("color", glm::vec3(0.1f, 0.1f, 0.1f));
            }
            
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            model = transforamtion(-.5, 1.95, 4.5 - i * 3, .05, .05, .05);
            ourShader.setMat4("model", model);
            if (spotlightToggle) {
                ourShader.setVec3("color", glm::vec3(0.8f, 0.8f, 0.8f));
            }
            else {
                ourShader.setVec3("color", glm::vec3(0.1f, 0.1f, 0.1f));
            }
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            //glDrawArrays(GL_TRIANGLES, 0, 36);
        }

       
        if(nightmode)
            ourShader.setVec3("color", glm::vec3(0.8f, 0.8f, 0.8f));
        else
            ourShader.setVec3("color", glm::vec3(1, 0.984, 0));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setVec3("direcLight.direction", 0.5f, -3.0f, -3.0f);
        lightingShader.setVec3("direcLight.ambient", d_a);
        lightingShader.setVec3("direcLight.diffuse", d_d);
        lightingShader.setVec3("direcLight.specular", d_s);
        lightingShader.setBool("dlighton", directionallightToggle);
        model = transforamtion(-5, 30, -30, 5, 5, 5);

        sphere.drawSphere(lightingShader, model);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void drawCube(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f), float r = 1.0f, float g = 1.0f, float b = 1.0f)
{
    lightingShader.use();

    lightingShader.setVec3("material.ambient", glm::vec3(r, g, b));
    lightingShader.setVec3("material.diffuse", glm::vec3(r, g, b));
    lightingShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    lightingShader.setFloat("material.shininess", 32.0f);

    lightingShader.setMat4("model", model);

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void drawPool(unsigned int& p1VAO, Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f), float r = 1.0f, float g = 1.0f, float b = 1.0f) {
    lightingShader.use();

    lightingShader.setVec3("material.ambient", glm::vec3(r, g, b));
    lightingShader.setVec3("material.diffuse", glm::vec3(r, g, b));
    lightingShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    lightingShader.setFloat("material.shininess", 32.0f);

    lightingShader.setMat4("model", model);

    glBindVertexArray(p1VAO);
    glDrawElements(GL_TRIANGLES, 27, GL_UNSIGNED_INT, 0);
}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        camera.ProcessKeyboard(P_UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        camera.ProcessKeyboard(P_DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        camera.ProcessKeyboard(Y_LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        camera.ProcessKeyboard(Y_RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        camera.ProcessKeyboard(R_LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.ProcessKeyboard(R_RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        movefr -= .05;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        movefr += .05;
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        movelr -= .05;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        movelr += .05;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        jump = .03;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if (directionallightToggle)
            directionallightToggle = false;
        else
            directionallightToggle = true;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (pointlightToggle) {
            pointlightToggle = false;
            p_ambient = 0;
            p_diffuse = 0;
            p_specular = 0;
        }

        else {
            pointlightToggle = true;
            p_ambient = 1;
            p_diffuse = 1;
            p_specular = 1;
        }

    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        if (spotlightToggle) {
            spotlightToggle = false;
            s_ambient = 0;
            s_diffuse = 0;
            s_specular = 0;
        }

        else {
            spotlightToggle = true;
            s_ambient = 1;
            s_diffuse = 1;
            s_specular = 1;
        }

    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        if (ambientToggle) {
            ambientToggle = false;
            if (directionallightToggle) {
                d_ambient = 0;
            }
            if (pointlightToggle) {
                p_ambient = 0;
            }

            if (spotlightToggle) {
                s_ambient = 0;
            }
 
        }

        else {
            ambientToggle = true;
            if (directionallightToggle) {
                d_ambient = 1;
            }
            if (pointlightToggle) {
                p_ambient = 1;
            }

            if (spotlightToggle) {
                s_ambient = 1;
            }



        }

    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
        if (diffuseToggle) {
            diffuseToggle = false;
            if (directionallightToggle) {
                d_diffuse = 0;
            }
            if (pointlightToggle) {
                p_diffuse = 0;
            }

            if (spotlightToggle) {
                s_diffuse = 0;
            }



        }

        else {
            diffuseToggle = true;
            if (directionallightToggle) {
                d_diffuse = 1;
            }
            if (pointlightToggle) {
                p_diffuse = 1;
            }

            if (spotlightToggle) {
                s_diffuse = 1;;
            }



        }

    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
        if (specularToggle) {
            specularToggle = false;
            if (directionallightToggle) {
                d_specular = 0;
            }
            if (pointlightToggle) {
                p_specular = 0;
            }

            if (spotlightToggle) {
                s_specular = 0;
            }



        }

        else {
            specularToggle = true;
            if (directionallightToggle) {
                d_specular = 1;
            }
            if (pointlightToggle) {
                p_specular = 1;
            }

            if (spotlightToggle) {
                s_specular = 1;
            }



        }


    }
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
        if (nightmode) {
            nightmode = false;
        }

        else {
            nightmode = true;

        }

    }


}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrappingModeS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrappingModeT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilteringModeMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilteringModeMax);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}