//
//  pointLight.h
//  test
//
//  Created by Nazirul Hasan on 22/9/23.
//

#ifndef pointLight_h
#define pointLight_h

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"
#include <iostream>
#include <string>
using namespace std;

class PointLight {
public:
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float k_c;
    float k_l;
    float k_q;
    int Number;
    float p_ambient;
    float p_diffuse;
    float p_specular;

    PointLight(float posX, float posY, float posZ, float ambR, float ambG, float ambB, float diffR, float diffG, float diffB, float specR, float specG, float specB, float constant, float linear, float quadratic, int num) {

        position = glm::vec3(posX, posY, posZ);
        ambient = glm::vec3(ambR, ambG, ambB);
        diffuse = glm::vec3(diffR, diffG, diffB);
        specular = glm::vec3(specR, specG, specB);
        k_c = constant;
        k_l = linear;
        k_q = quadratic;
        Number = num - 1;
        p_ambient = 1;
        p_diffuse = 1;
        p_specular = 1;
    }
    PointLight() {
        position = glm::vec3(0, 0, 0);
        ambient = glm::vec3(0.1f, 0.1f, 0.1f);
        diffuse = glm::vec3(0.141, 0.137, 0);
        specular = glm::vec3(0.2f, 0.2f, 0.2f);
        k_c = 1.0f;
        k_l = 0.8f;
        k_q = 0.032f;
        Number = 1;
        p_ambient = 1;
        p_diffuse = 1;
        p_specular = 1;
    }
    void setUpPointLight(Shader& lightingShader)
    {
        lightingShader.use();
        string lightNumber = to_string(Number);
        lightingShader.setVec3("pointLights[" + lightNumber + "].position", position);
        lightingShader.setVec3("pointLights[" + lightNumber + "].ambient", ambient * p_ambient);
        lightingShader.setVec3("pointLights[" + lightNumber + "].diffuse", diffuse * p_diffuse);
        lightingShader.setVec3("pointLights[" + lightNumber + "].specular", specular * p_specular);
        lightingShader.setFloat("pointLights[" + lightNumber + "].k_c", k_c);
        lightingShader.setFloat("pointLights[" + lightNumber + "].k_l", k_l);
        lightingShader.setFloat("pointLights[" + lightNumber + "].k_q", k_q);
        lightingShader.setBool("plighton", true);

    }
};
#endif /* pointLight_h */
