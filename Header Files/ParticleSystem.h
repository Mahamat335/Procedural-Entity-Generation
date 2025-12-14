#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "Singleton.h"
#include <EBO.h>
#include <ShaderManager.h>
#include <Texture.h> // Texture class'in varsa kullaniriz, yoksa raw GL
#include <VAO.h>
#include <VBO.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>


struct Particle {
  glm::vec3 Position;
  glm::vec3 Velocity;
  float Life;
  float StartLife;
  float Size;
};

class ParticleSystem : public Singleton<ParticleSystem> {

  friend Singleton<ParticleSystem>;

public:
  ParticleSystem();
  ~ParticleSystem();

  // Texture'i ve Mesh'i hazirlar
  void Init();

  // Efekti tetikleyen ana fonksiyon
  void EmitPoof(glm::vec3 position);

  // Fizik ve omur hesaplamalari
  void Update(float deltaTime);

  // Cizim islemi (Artik texture parametresi istemiyor)
  void Draw(const glm::mat4 &view, const glm::mat4 &projection);

private:
  std::vector<Particle> _particles;
  unsigned int _smokeTextureID; // Texture ID internal olarak tutuluyor

  VAO *_vao;
  VBO *_vbo;
  EBO *_ebo;

  void SetupMesh();
  void LoadTexture(); // Texture yukleme fonksiyonu
};

#endif