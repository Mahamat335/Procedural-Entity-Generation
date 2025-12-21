#include "BarRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

BarRenderer::BarRenderer() : _vao(nullptr), _vbo(nullptr), _ebo(nullptr) {}

BarRenderer::~BarRenderer() {
  if (_vao) {
    _vao->Delete();
    delete _vao;
  }
  if (_vbo) {
    _vbo->Delete();
    delete _vbo;
  }
  if (_ebo) {
    _ebo->Delete();
    delete _ebo;
  }
}

void BarRenderer::Init() { SetupMesh(); }

void BarRenderer::SetupMesh() {
  // Bar Mesh'i: Sol alt köşe (0, -0.5) olacak şekilde ayarlandı.
  // Böylece X ekseninde scale edince sağa doğru uzayacak.
  float vertices[] = {
      // Pos (x, y, z)
      0.0f, -0.5f, 0.0f, // 0. Sol Alt
      1.0f, -0.5f, 0.0f, // 1. Sag Alt
      1.0f, 0.5f,  0.0f, // 2. Sag Ust
      0.0f, 0.5f,  0.0f  // 3. Sol Ust
  };

  unsigned int indices[] = {0, 1, 2, 2, 3, 0};

  _vao = new VAO();
  _vao->Bind();

  _vbo = new VBO(vertices, sizeof(vertices));
  _ebo = new EBO(indices, sizeof(indices));

  // Attribute 0: Position (3 floats)
  // Senin CubeData yapindaki gibi LinkAttrib kullaniyoruz
  _vao->LinkAttrib(*_vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void *)0);

  _vao->Unbind();
  _vbo->Unbind();
  _ebo->Unbind();
}

void BarRenderer::DrawBar(glm::vec3 position, float currentValue,
                          float maxValue, glm::vec2 size, glm::vec3 color,
                          const glm::mat4 &view, const glm::mat4 &projection) {

  // ShaderManager'dan shader'i alip aktif ediyoruz
  Shader *shader = ShaderManager::Instance().barShaderProgram;
  shader->use();

  // --- RENDER STATE AYARLARI (Görünürlük Sorunu İçin Kritik) ---
  glDisable(GL_CULL_FACE); // Arkası dönük olsa bile çizilsin
  glDisable(GL_DEPTH_TEST); // UI olduğu için her şeyin üstüne çizilsin (Duvar
                            // arkasında olsa bile)

  // Global Uniforms
  glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1,
                     GL_FALSE, glm::value_ptr(projection));
  glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE,
                     glm::value_ptr(view));

  _vao->Bind();

  float percentage = glm::clamp(currentValue / maxValue, 0.0f, 1.0f);

  // --- 1. ARKA PLAN (Siyah Bar) ---
  {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    // Billboarding (Kameraya döndürme)
    // View matrisinin ters rotasyonunu aliyoruz
    model[0][0] = view[0][0];
    model[0][1] = view[1][0];
    model[0][2] = view[2][0];
    model[1][0] = view[0][1];
    model[1][1] = view[1][1];
    model[1][2] = view[2][1];
    model[2][0] = view[0][2];
    model[2][1] = view[1][2];
    model[2][2] = view[2][2];

    // Hizalama: Mesh'i ortalamak icin genisligin yarisi kadar sola kaydir
    model = glm::translate(model, glm::vec3(-size.x / 2.0f, 0.0f, 0.0f));

    // Boyutlandirma (Full genislik)
    model = glm::scale(model, glm::vec3(size.x, size.y, 1.0f));

    glUniform3f(glGetUniformLocation(shader->ID, "barColor"), 0.1f, 0.1f,
                0.1f); // Koyu Gri Arkaplan
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE,
                       glm::value_ptr(model));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }

  // --- 2. ÖN PLAN (Renkli Bar) ---
  {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    // Billboarding
    model[0][0] = view[0][0];
    model[0][1] = view[1][0];
    model[0][2] = view[2][0];
    model[1][0] = view[0][1];
    model[1][1] = view[1][1];
    model[1][2] = view[2][1];
    model[2][0] = view[0][2];
    model[2][1] = view[1][2];
    model[2][2] = view[2][2];

    // Hizalama (Arkaplan ile aynı yere, hafifçe öne alabiliriz ama DepthTest
    // kapalı olduğu için gerek yok)
    model = glm::translate(model, glm::vec3(-size.x / 2.0f, 0.0f, 0.0f));

    // Boyutlandirma (Yüzdeye göre X ekseninde scale)
    // DİKKAT: Yüzde 0 ise scale 0 olur, görünmez.
    if (percentage > 0.01f) {
      model = glm::scale(model, glm::vec3(size.x * percentage, size.y, 1.0f));

      glUniform3fv(glGetUniformLocation(shader->ID, "barColor"), 1,
                   glm::value_ptr(color));
      glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE,
                         glm::value_ptr(model));

      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
  }

  _vao->Unbind();

  // --- AYARLARI GERİ AL ---
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}