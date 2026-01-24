#ifndef GAME_H
#define GAME_H

#include "Camera.h"
#include "EntityData.h"
#include <Collider.h>
#include <GLFW/glfw3.h>
#include <GenerationData.h>
#include <Node.h>
#include <Transform.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <stack>

class Spider;
class Caterpillar;
class Producer;

class Game {
public:
  struct GameData {
    glm::mat4 view;
    glm::mat4 projection;
    bool polygonMode = false;
    bool showColliders = false;
    bool isCursorEnabled = false;
    unsigned int modelLoc;
    bool areSpidersMoving = true;
    SpiderGenerationData spiderGenerationData{
        16, 1, 5, 0.2f, 0.8f, 0.1f, 0.25f, 0.25f, 0.25f, 3.0f, 3.0f, 3.0f,
    };
    CaterpillarGenerationData caterpillarGenerationData{
        8, 3, 8, 0.4f, 1.2f, 0.1f,
    };
    ProducerGenerationData producerGenerationData{
        8, 3, 8, 0.4f, 1.2f, 0.1f,
    };
  };

public:
  static GameData data;
  static std::vector<Spider *> spiders;
  static std::vector<Caterpillar *> caterpillars;
  static std::vector<Producer *> producers;
  static std::unordered_set<IEntity *> destroyedEntities;
  Game();
  bool Start();
  bool Update(float deltaTime);
  void End();
  void RenderEntities(unsigned int modelLoc, Shader shaderProgram,
                      bool isShadowPass = false);
  void InitializeSpiders();
  void InitializeCaterpillars();
  void InitializeProducers();
  static void SpawnNewSpider(SpiderEntityData spiderData);
  static void SpawnNewCaterpillar(CaterpillarEntityData caterpillarData);

private:
  void DrawHungerBars(const glm::mat4 &view, const glm::mat4 &projection);
};

#endif // GAME_H
