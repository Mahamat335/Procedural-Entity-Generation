#include "ParticleSystem.h"
#include <algorithm> // std::min icin
#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stb/stb_image.h>

ParticleSystem::ParticleSystem()
    : _vao(nullptr), _vbo(nullptr), _ebo(nullptr), _smokeTextureID(0) {}

ParticleSystem::~ParticleSystem() {
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
  // Delete the texture from GPU memory
  glDeleteTextures(1, &_smokeTextureID);
}

void ParticleSystem::Init() {
  SetupMesh();
  LoadTexture();
}

void ParticleSystem::LoadTexture() {
  std::string path = "resources/textures/smoke.png";

  glGenTextures(1, &_smokeTextureID);
  glBindTexture(GL_TEXTURE_2D, _smokeTextureID);

  int width, height, nrChannels;
  // stbi_set_flip_vertically_on_load(true); // Uncomment if texture is upside
  // down
  unsigned char *data =
      stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

  if (data) {
    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Texture wrapping/filtering settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cerr << "ParticleSystem Error: Failed to load texture at " << path
              << std::endl;
    stbi_image_free(data);
  }
}

void ParticleSystem::SetupMesh() {
  // Corrected vertices array (Pos: x,y,z | UV: u,v)
  float vertices[] = {// Pos               // UV
                      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.5f, -0.5f,
                      0.0f,  1.0f,  0.0f, 0.5f, 0.5f, 0.0f, 1.0f,
                      1.0f,  -0.5f, 0.5f, 0.0f, 0.0f, 1.0f};

  unsigned int indices[] = {0, 1, 2, 2, 3, 0};

  _vao = new VAO();
  _vao->Bind();

  _vbo = new VBO(vertices, sizeof(vertices));
  _ebo = new EBO(indices, sizeof(indices));

  // Attribute 0: Position
  _vao->LinkAttrib(*_vbo, 0, 3, GL_FLOAT, 5 * sizeof(float), (void *)0);
  // Attribute 1: TexCoords
  _vao->LinkAttrib(*_vbo, 1, 2, GL_FLOAT, 5 * sizeof(float),
                   (void *)(3 * sizeof(float)));

  _vao->Unbind();
  _vbo->Unbind();
  _ebo->Unbind();
}

void ParticleSystem::EmitPoof(glm::vec3 position) {
  int particleCount = 20; // Increased count for better visuals

  for (int i = 0; i < particleCount; i++) {
    Particle p;
    p.Position = position;

    // Better Random Velocity Spread
    float rX = ((rand() % 100) / 50.0f) - 1.0f; // -1.0 to 1.0
    float rY = ((rand() % 100) / 50.0f) - 1.0f;
    float rZ = ((rand() % 100) / 50.0f) - 1.0f;

    p.Velocity = glm::vec3(rX, rY, rZ) * 3.0f; // Speed

    // Random Life between 1.0s and 1.5s
    p.StartLife = 1.0f + (static_cast<float>(rand() % 100) / 200.0f);
    p.Life = p.StartLife;
    p.Size = 0.5f; // Start small, grow later

    _particles.push_back(p);
  }
}

void ParticleSystem::Update(float deltaTime) {
  // Safe Delta: Prevents animation jumping if game lags (max 20fps step)
  float safeDelta = std::min(deltaTime, 0.05f);

  for (size_t i = 0; i < _particles.size(); i++) {
    Particle &p = _particles[i];
    p.Life -= safeDelta;

    if (p.Life > 0.0f) {
      p.Position += p.Velocity * safeDelta;
      p.Velocity.y += 0.8f * safeDelta; // Upward drift (negative gravity)
      p.Size += safeDelta * 3.0f;       // Grow over time
    } else {
      // Efficient erase
      _particles.erase(_particles.begin() + i);
      i--;
    }
  }
}

void ParticleSystem::Draw(const glm::mat4 &view, const glm::mat4 &projection) {
  if (_particles.empty())
    return;

  Shader *shader = ShaderManager::Instance().particleShaderProgram;
  shader->use();

  // --- GLOBAL STATE CHANGES (Before Loop) ---
  // Disable Depth Test to prevent z-fighting between particles (optional but
  // recommended for smoke)
  glDisable(GL_DEPTH_TEST);

  // Enable Blending for transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Disable Depth Write (Transparent objects shouldn't hide objects behind
  // them)
  glDepthMask(GL_FALSE);

  // Disable Culling (Particles should be visible from both sides)
  glDisable(GL_CULL_FACE);

  // Set Global Uniforms
  glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1,
                     GL_FALSE, glm::value_ptr(projection));
  glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE,
                     glm::value_ptr(view));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _smokeTextureID);
  glUniform1i(glGetUniformLocation(shader->ID, "sprite"), 0);

  _vao->Bind();

  // --- DRAW LOOP ---
  for (const auto &p : _particles) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, p.Position);

    // Billboarding: Set rotation to inverse of view rotation
    model[0][0] = view[0][0];
    model[0][1] = view[1][0];
    model[0][2] = view[2][0];
    model[1][0] = view[0][1];
    model[1][1] = view[1][1];
    model[1][2] = view[2][1];
    model[2][0] = view[0][2];
    model[2][1] = view[1][2];
    model[2][2] = view[2][2];

    model = glm::scale(model, glm::vec3(p.Size));

    float alpha = p.Life / p.StartLife;
    glUniform1f(glGetUniformLocation(shader->ID, "alpha"), alpha);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE,
                       glm::value_ptr(model));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }

  _vao->Unbind();

  // --- RESTORE STATE (After Loop) ---
  glEnable(GL_CULL_FACE);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
}