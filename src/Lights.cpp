#include <Lights.h>
#include <ShaderManager.h>

Light::Light(glm::vec3 color, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) : ambient(ambient), diffuse(diffuse), specular(specular)
{
    this->color = color;
}

DirectionalLight::DirectionalLight(glm::vec3 color, glm::vec3 direction)
{
    this->color = color;
    this->direction = direction;
    this->SendLightData();
}

DirectionalLight::DirectionalLight(glm::vec3 color, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) : ambient(ambient), diffuse(diffuse), specular(specular)
{
    this->color = color;
    this->direction = direction;
    this->SendLightData();
}

void DirectionalLight::SendLightData()
{
    Shader shaderProgram = *(ShaderManager::Instance().defaultShaderProgram);
    shaderProgram.use();
    // Directional Light
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "directionalLight.direction"), direction.x, direction.y, direction.z);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "directionalLight.ambient"), ambient.x, ambient.y, ambient.z);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "directionalLight.diffuse"), diffuse.x, diffuse.y, diffuse.z);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "directionalLight.specular"), specular.x, specular.y, specular.z);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "directionalLight.color"), color.x, color.y, color.z);
}

LocatedLight::LocatedLight(glm::vec3 pos, glm::vec3 color, int lightId)
{
    this->position = pos;
    this->color = color;
    this->lightId = lightId;
}

LocatedLight::LocatedLight(glm::vec3 pos, glm::vec3 color, int lightId, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) : Light(color, ambient, diffuse, specular)
{
    this->position = pos;
    this->lightId = lightId;
}

PointLight::PointLight(int lightId, glm::vec3 pos, glm::vec3 color) : LocatedLight(pos, color, lightId)
{
    this->SendLightData();
}

PointLight::PointLight(int lightId, glm::vec3 pos, glm::vec3 color, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) : LocatedLight(pos, color, lightId, ambient, diffuse, specular)
{
    this->SendLightData();
}

void PointLight::SendLightData()
{
    Shader shaderProgram = *(ShaderManager::Instance().defaultShaderProgram);
    shaderProgram.use();
    // Point Light
    glUniform3f(glGetUniformLocation(shaderProgram.ID, ("pointLights[" + std::to_string(lightId) + "].position").c_str()), position.x, position.y, position.z);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, ("pointLights[" + std::to_string(lightId) + "].ambient").c_str()), ambient.x, ambient.y, ambient.z);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, ("pointLights[" + std::to_string(lightId) + "].diffuse").c_str()), diffuse.x, diffuse.y, diffuse.z);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, ("pointLights[" + std::to_string(lightId) + "].specular").c_str()), specular.x, specular.y, specular.z);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, ("pointLights[" + std::to_string(lightId) + "].color").c_str()), color.x, color.y, color.z);
    glUniform1f(glGetUniformLocation(shaderProgram.ID, ("pointLights[" + std::to_string(lightId) + "].constant").c_str()), constant);
    glUniform1f(glGetUniformLocation(shaderProgram.ID, ("pointLights[" + std::to_string(lightId) + "].linear").c_str()), linear);
    glUniform1f(glGetUniformLocation(shaderProgram.ID, ("pointLights[" + std::to_string(lightId) + "].quadratic").c_str()), quadratic);
}

SpotLight::SpotLight(int lightId, glm::vec3 pos, glm::vec3 color, glm::vec3 direction, float cutOff, float outerCutOff) : LocatedLight(pos, color, lightId)
{
    this->direction = direction;
    this->cutOff = cutOff;
    this->outerCutOff = outerCutOff;
    this->SendLightData();
}

SpotLight::SpotLight(int lightId, glm::vec3 pos, glm::vec3 color, glm::vec3 direction, float cutOff, float outerCutOff, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) : LocatedLight(pos, color, lightId, ambient, diffuse, specular)
{
    this->direction = direction;
    this->cutOff = cutOff;
    this->outerCutOff = outerCutOff;
    this->SendLightData();
}

void SpotLight::SendLightData()
{
    Shader shaderProgram = *(ShaderManager::Instance().defaultShaderProgram);
    shaderProgram.use();
    // Spot Light
    glUniform3f(glGetUniformLocation(shaderProgram.ID, ("spotLights[" + std::to_string(lightId) + "].position").c_str()), position.x, position.y, position.z);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, ("spotLights[" + std::to_string(lightId) + "].direction").c_str()), direction.x, direction.y, direction.z);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, ("spotLights[" + std::to_string(lightId) + "].ambient").c_str()), ambient.x, ambient.y, ambient.z);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, ("spotLights[" + std::to_string(lightId) + "].diffuse").c_str()), diffuse.x, diffuse.y, diffuse.z);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, ("spotLights[" + std::to_string(lightId) + "].specular").c_str()), specular.x, specular.y, specular.z);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, ("spotLights[" + std::to_string(lightId) + "].color").c_str()), color.x, color.y, color.z);
    glUniform1f(glGetUniformLocation(shaderProgram.ID, ("spotLights[" + std::to_string(lightId) + "].constant").c_str()), constant);
    glUniform1f(glGetUniformLocation(shaderProgram.ID, ("spotLights[" + std::to_string(lightId) + "].linear").c_str()), linear);
    glUniform1f(glGetUniformLocation(shaderProgram.ID, ("spotLights[" + std::to_string(lightId) + "].quadratic").c_str()), quadratic);
    glUniform1f(glGetUniformLocation(shaderProgram.ID, ("spotLights[" + std::to_string(lightId) + "].cutOff").c_str()), cutOff);
    glUniform1f(glGetUniformLocation(shaderProgram.ID, ("spotLights[" + std::to_string(lightId) + "].outerCutOff").c_str()), outerCutOff);
}