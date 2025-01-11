#ifndef LIGHTS_H
#define LIGHTS_H

#include <glm/glm.hpp>

class Light
{
public:
    glm::vec3 color;
    glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    glm::vec3 diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
    Light() {};
    Light(glm::vec3 color, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
    virtual void SendLightData() = 0;
};

class DirectionalLight : public Light
{
public:
    glm::vec3 direction;
    glm::vec3 ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 specular = glm::vec3(0.5f, 0.5f, 0.5f);
    DirectionalLight(glm::vec3 color, glm::vec3 direction);
    DirectionalLight(glm::vec3 color, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
    void SendLightData() override;
};

class LocatedLight : public Light
{
public:
    glm::vec3 position;
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    int lightId;

    LocatedLight(glm::vec3 pos, glm::vec3 color, int lightId);
    LocatedLight(glm::vec3 pos, glm::vec3 color, int lightId, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
};

class PointLight : public LocatedLight
{
public:
    PointLight(int lightId, glm::vec3 pos, glm::vec3 color);
    PointLight(int lightId, glm::vec3 pos, glm::vec3 color, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
    void SendLightData() override;
};

class SpotLight : public LocatedLight
{
public:
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
    SpotLight(int lightId, glm::vec3 pos, glm::vec3 color, glm::vec3 direction, float cutOff, float outerCutOff);
    SpotLight(int lightId, glm::vec3 pos, glm::vec3 color, glm::vec3 direction, float cutOff, float outerCutOff, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
    void SendLightData() override;
};

#endif