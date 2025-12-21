#ifndef BAR_RENDERER_H
#define BAR_RENDERER_H

#include "Singleton.h"
#include <EBO.h>
#include <ShaderManager.h>
#include <VAO.h>
#include <VBO.h>
#include <glm/glm.hpp>

class BarRenderer : public Singleton<BarRenderer> {
  friend Singleton<BarRenderer>;

public:
  BarRenderer();
  ~BarRenderer();

  void Init();

  void DrawBar(glm::vec3 position, float currentValue, float maxValue,
               glm::vec2 size, glm::vec3 color, const glm::mat4 &view,
               const glm::mat4 &projection);

private:
  VAO *_vao;
  VBO *_vbo;
  EBO *_ebo;

  void SetupMesh();
};

#endif