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

#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void drawCube(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model, float r, float g, float b);
void drawCube(unsigned int& p1VAO, Shader& lightingShader, glm::mat4 model, float r, float g, float b);
void scene(unsigned int& cubeVAO, unsigned int& p1VAO, Shader& lightingShader, glm::mat4 alTogether, Shader& Shader);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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
// camera
Camera camera(glm::vec3(0.0f, 1.1f, 5.2f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float eyeX = 0.0, eyeY = 1.0, eyeZ = 3.0;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);


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

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

glm::mat4 transforamtion(float tx, float ty, float tz,float sx, float sy, float sz) {
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    translateMatrix = glm::translate(identityMatrix, glm::vec3(tx, ty, tz));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(sx, sy, sz));
    model = translateMatrix * scaleMatrix;
    return model;
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
    //Shader lightingShader("vertexShaderForGouraudShading.vs", "fragmentShaderForGouraudShading.fs");
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");

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

    float pool[] = {
        0,0,0,0.0f, 1.0f, 0.0f,
        1.0,0,0.0,0.0f, 1.0f, 0.0f,
        0.866,0,0.5,0.0f, 1.0f, 0.0f,
        0.5,0,0.866,0.0f, 1.0f, 0.0f,
        0.0,0,1.0,0.0f, 1.0f, 0.0f,
        - 0.5,0,0.866,0.0f, 1.0f, 0.0f,
        - 0.866,0,0.5,0.0f, 1.0f, 0.0f,
        - 1.0,0,0.0,0.0f, 1.0f, 0.0f,
        - 0.866,0,-0.5,0.0f, 1.0f, 0.0f,
        - 0.5,0,0.866,0.0f, 1.0f, 0.0f,
        0.0,0,-1.0,0.0f, 1.0f, 0.0f,
        0.5,0,-0.866,0.0f, 1.0f, 0.0f,
        0.866,0,-0.5,0.0f, 1.0f, 0.0f,
        1.0,0,0.0,0.0f, 1.0f, 0.0f,

        0,1,0, 0.0f, -1.0f, 0.0f,
        1.0,1,0.0, 0.0f, -1.0f, 0.0f,
        0.866,1,0.5, 0.0f, -1.0f, 0.0f,
        0.5,1,0.866, 0.0f, -1.0f, 0.0f,
        0.0,1,1.0, 0.0f, -1.0f, 0.0f,
        - 0.5,1,0.866, 0.0f, -1.0f, 0.0f,
        - 0.866,1,0.5, 0.0f, -1.0f, 0.0f,
        - 1.0,1,0.0, 0.0f, -1.0f, 0.0f,
        - 0.866,1,-0.5, 0.0f, -1.0f, 0.0f,
        - 0.5,1,-0.866, 0.0f, -1.0f, 0.0f,
        - 0.0,1,-1.0, 0.0f, -1.0f, 0.0f,
        0.5,1,-0.866, 0.0f, -1.0f, 0.0f,
        0.866,1,-0.5, 0.0f, -1.0f, 0.0f,
        1.0,1,-0.0, 0.0f, -1.0f, 0.0f,

    };

    unsigned int pool_indx[] = {
        0,1,2,
        0,2,3,
        0,3,4,
        0,4,5,
        0,5,6,
        0,6,7,
        0,7,8,
        0,8,9,
        0,9,10,
        0,10,11,
        0,11,12,
        0,12,13,
        0,13,14,
        //15,16,17,
        //15,17,18,
        //15,18,19,
        //15,19,20,
        //15,20,21,
        //15,21,22,
        //15,22,23,
        //15,23,24,
        //15,24,25,
        //15,25,26,
        //15,26,27,
        //15,27,28
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

    unsigned int p1VAO, p1VBO, p1EBO;
    glGenVertexArrays(1, &p1VAO);
    glGenBuffers(1, &p1VBO);
    glGenBuffers(1, &p1EBO);

    glBindVertexArray(p1VAO);

    glBindBuffer(GL_ARRAY_BUFFER, p1VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pool), pool, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p1EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pool_indx), pool_indx, GL_STATIC_DRAW);


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
    SpotLight spotlight[8];
    PointLight pointlight[9];
    PointLight pointlight2[9];
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
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        // point light 1
        //pointlight1.setUpPointLight(lightingShader);
        for (int i = 0; i < 4; i++) {
            spotlight[i].position = glm::vec3(.5, 1.95, 4.5 - i * 3);
            spotlight[i].Number = i;
            spotlight[i].s_ambient = s_ambient;
            spotlight[i].s_diffuse = s_diffuse;
            spotlight[i].s_specular = s_specular;
            spotlight[i].setUpspotLight(lightingShader);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            spotlight[i+4].position = glm::vec3(-.5, 1.95, 4.5 - i * 3);
            spotlight[i+4].Number = i+4;
            spotlight[i+4].s_ambient = s_ambient;
            spotlight[i+4].s_diffuse = s_diffuse;
            spotlight[i+4].s_specular = s_specular;
            spotlight[i+4].setUpspotLight(lightingShader);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
        for (int i = 0;i < 9;i+=3) {
            for (int j = 0; j < 3; j++) {
                pointlight[j+i].position = glm::vec3(-2.4, 1 + j * 1.5, -3.7 + i * 3.5);
                pointlight[j+i].Number = j+i;
                pointlight[j + i].p_ambient = p_ambient;
                pointlight[j + i].p_diffuse = p_diffuse;
                pointlight[j + i].p_specular = p_specular;
                pointlight[j+i].setUpPointLight(lightingShader);
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
                pointlight2[j + i].setUpPointLight(lightingShader);
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
        lightingShader.use();
        
        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        lightingShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        //glm::mat4 view = basic_camera.createViewMatrix();
        lightingShader.setMat4("view", view);

        lightingShader.setVec3("direcLight.direction", 0.5f, -3.0f, -3.0f);
        lightingShader.setVec3("direcLight.ambient", 0.2f, 0.2f, 0.2f);
        lightingShader.setVec3("direcLight.diffuse", 0.7f, 0.7f, 0.7f);
        lightingShader.setVec3("direcLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setBool("dlighton", directionallightToggle);

        // Modelling Transformation
        glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        //glBindVertexArray(cubeVAO);
        //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_TRIANGLES, 0, 36);

        scene(cubeVAO, p1VAO, lightingShader, model, ourShader);

        // also draw the lamp object(s)
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = transforamtion(.5, 1.95, 4.5 - i * 3, .05, .05, .05);
            ourShader.setMat4("model", model);
            ourShader.setVec3("color", glm::vec3(0.8f, 0.8f, 0.8f));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            model = transforamtion(-.5, 1.95, 4.5 - i * 3, .05, .05, .05);
            ourShader.setMat4("model", model);
            ourShader.setVec3("color", glm::vec3(0.8f, 0.8f, 0.8f));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            //glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        for (int i = 0;i < 3;i++) {
            for (int j = 0; j < 3; j++) {
                model = transforamtion(-2.4, .5 + j * 1.5, -3.95 + i * 3.5, .01, .8, .5);
                ourShader.setMat4("model", model);
                ourShader.setVec3("color", glm::vec3(0.722, 0.71, 0.161));
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
     
        }
        for (int i = 0;i < 3;i++) {
            for (int j = 0; j < 3; j++) {
                model = transforamtion(2.59, .5 + j * 1.5, -3.95 + i * 3.5, .01, .8, .5);
                ourShader.setMat4("model", model);
                ourShader.setVec3("color", glm::vec3(0.722, 0.71, 0.161));
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }

        }

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

void scene(unsigned int& cubeVAO, unsigned int& p1VAO, Shader& lightingShader, glm::mat4 alTogether, Shader& ourShader)
{
    float baseHeight = 0.01;
    float width = 2;
    float length = 10;

    glm::mat4 model = glm::mat4(1.0f);

    //Ground
    model = transforamtion(-5, -0.01, -5, width*5, baseHeight, length);
    model = alTogether * model;
    drawCube(cubeVAO, lightingShader, model, 0, 0.431, 0.008);

    //road
    model = transforamtion(-1, 0, -5, width, baseHeight, length);
    model = alTogether * model;
    drawCube(cubeVAO, lightingShader, model, 0.1, 0.1, 0.1);

    for (int i = -3; i < 3; i++) {
        model = transforamtion(0, .001, .5+i*1.5, .05, .01, .7);
        model = alTogether * model;
        drawCube(cubeVAO, lightingShader, model, .8, .8, .8);
    }
    model = transforamtion(-1.4, 0, -5, width*.2, baseHeight*10, length);
    model = alTogether * model;
    drawCube(cubeVAO, lightingShader, model, 0.71, 0.71, 0.71);
    model = transforamtion(1, 0, -5, width * .2, baseHeight * 10, length);
    model = alTogether * model;
    drawCube(cubeVAO, lightingShader, model, 0.71, 0.71, 0.71);
    model = transforamtion(-1.38, .1, -5, width * .18, baseHeight*.1, length);
    model = alTogether * model;
    drawCube(cubeVAO, lightingShader, model, 0.239, 0.051, 0.09);
    model = transforamtion(1.02, .1, -5, width * .18, baseHeight*.1, length);
    model = alTogether * model;
    drawCube(cubeVAO, lightingShader, model, 0.239, 0.051, 0.09);

    //building
    for (int i = 0;i < 3;i++) {
        model = transforamtion(-4.4, 0, -5+i*3.5, width, baseHeight * 500, length * .25);
        model = alTogether * model;
        drawCube(cubeVAO, lightingShader, model, 0.71, 0.71, 0.71);
        model = transforamtion(2.6, 0, -5+i*3.5, width, baseHeight * 500, length * .25);
        model = alTogether * model;
        drawCube(cubeVAO, lightingShader, model, 0.71, 0.71, 0.71);
    }

    //pool
    for (int i = 0; i < 4; i++) {
        model = transforamtion(1.5, 0, 4.5-i*3, .05, 2, .05);
        model = alTogether * model;
        drawCube(cubeVAO, lightingShader, model, 0.71, 0.71, 0.71);
    }
    for (int i = 0; i < 4; i++) {
        model = transforamtion(-1.5, 0, 4.5 - i * 3, .05, 2, .05);
        model = alTogether * model;
        drawCube(cubeVAO, lightingShader, model, 0.71, 0.71, 0.71);
    }
    for (int i = 0; i < 4; i++) {
        model = transforamtion(1.5, 1.95, 4.5 - i * 3, -.95, .05, .05);
        model = alTogether * model;
        drawCube(cubeVAO, lightingShader, model, 0.71, 0.71, 0.71);
    }
    for (int i = 0; i < 4; i++) {
        model = transforamtion(-1.5, 1.95, 4.5 - i * 3, 1, .05, .05);
        model = alTogether * model;
        drawCube(cubeVAO, lightingShader, model, 0.71, 0.71, 0.71);
    }
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
