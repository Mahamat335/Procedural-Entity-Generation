#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <Shader.h>
#include <Singleton.h>

class ShaderManager : public Singleton<ShaderManager> {
  friend Singleton<ShaderManager>;

public:
  Shader *defaultShaderProgram;
  Shader *shadowMapShaderProgram;
  Shader *particleShaderProgram;
  Shader *barShaderProgram;
  ShaderManager() {
    defaultShaderProgram = new Shader("resources/shaders/default.vert",
                                      "resources/shaders/default.frag");
    shadowMapShaderProgram = new Shader("resources/shaders/shadowMap.vert",
                                        "resources/shaders/shadowMap.frag");
    particleShaderProgram = new Shader("resources/shaders/particle.vert",
                                       "resources/shaders/particle.frag");
    barShaderProgram =
        new Shader("resources/shaders/bar.vert", "resources/shaders/bar.frag");
  };
};

#endif