#include <Caterpillar.h>
#include <CollisionController.h>
#include <CubeCollider.h>
#include <EntityData.h>
#include <Game.h>
#include <Lights.h>
#include <Material.h>
#include <Producer.h>
#include <Spider.h>
#include <ctime>
#include <glm/gtc/random.hpp>

Node root, area, *spidersParent, *caterpillarsParent, *producersParent;
const glm::vec3 AreaSize(70.0f, 0.1f, 70.0f);
Game::GameData Game::data = {};
const int LegCount = 6;

std::vector<Spider *> Game::spiders;
std::vector<Caterpillar *> Game::caterpillars;
std::vector<Producer *> Game::producers;
std::unordered_set<IEntity *> Game::destroyedEntities;

Game::Game() {}

bool Game::Start() {
  data = {};

  area = Node(Transform(glm::vec3(0.0f, -5.0f, 0.0f),
                        glm::vec3(0.0f, 0.0f, 0.0f), AreaSize),
              CUBE, Bronze);
  root.AddChild(&area);

  spidersParent = new Node();
  root.AddChild(spidersParent);

  caterpillarsParent = new Node();
  root.AddChild(caterpillarsParent);

  producersParent = new Node();
  root.AddChild(producersParent);

  // Light Calculations
  DirectionalLight directionalLight(glm::vec3(1.0f, 1.0f, 1.0f),
                                    glm::vec3(-0.5f, -0.5f, -0.5f));
  PointLight pointLight(0, glm::vec3(2.0f, 2.0f, 0.0f),
                        glm::vec3(1.0f, 1.0f, 0.0f));

  return true;
}

bool Game::Update(float deltaTime) {
  // spider movement
  for (Spider *spider : spiders) {
    if (data.areSpidersMoving) {
      spider->Move(deltaTime);
    }
  }

  for (Caterpillar *caterpillar : caterpillars) {
    if (data.areSpidersMoving) {
      caterpillar->Move(deltaTime);
    }
  }

  for (Spider *spider : spiders) {
    CollisionController::Instance().CheckForCollisions(spider->GetCollider());
  }

  for (Caterpillar *caterpillar : caterpillars) {
    CollisionController::Instance().CheckForCollisions(
        caterpillar->GetCollider());
  }

  for (Spider *spider : spiders) {
    if (spider->UpdateHunger(deltaTime)) {
      spider->Die();
    }
  }

  for (Caterpillar *caterpillar : caterpillars) {
    if (caterpillar->UpdateHunger(deltaTime)) {
      caterpillar->Die();
    }
  }

  for (IEntity *entity : destroyedEntities) {
    // (Bu kısım C++20 ile daha kolay, ama şimdilik böyle yapabiliriz)
    spiders.erase(std::remove(spiders.begin(), spiders.end(), entity),
                  spiders.end());
    caterpillars.erase(
        std::remove(caterpillars.begin(), caterpillars.end(), entity),
        caterpillars.end());
    producers.erase(std::remove(producers.begin(), producers.end(), entity),
                    producers.end());

    delete entity;
  }
  destroyedEntities.clear();

  RenderEntities(data.modelLoc,
                 *(ShaderManager::Instance().defaultShaderProgram));
  return true;
}

void Game::End() {
  delete spidersParent;
  delete caterpillarsParent;
  delete producersParent;
}

void Game::RenderEntities(unsigned int modelLoc, Shader shaderProgram) {
  std::stack<Node *> stack;
  for (auto &child : root.children) {
    if (child->isEnable)
      stack.push(child);
  }

  while (!stack.empty()) {
    Node *current = stack.top();
    stack.pop();
    current->DrawMesh(modelLoc, shaderProgram);

    for (auto &child : current->children) {
      if (child->isEnable)
        stack.push(child);
    }
  }
}

void Game::InitializeSpiders() {
  for (Spider *spider : spiders) {
    delete spider;
  }

  spiders.clear();
  spidersParent->RemoveFromParent();
  delete spidersParent;

  spidersParent = new Node();
  root.AddChild(spidersParent);

  // Generate Spiders

  for (int i = 0; i < data.spiderGenerationData.EntityCount; i++) {
    float deg2rad = glm::pi<float>() / 180.0f;
    glm::vec3 zRotationAngles(60.0f, 60.0f, 60.0f);
    glm::vec3 bodySize = glm::vec3(0.8f, 0.4f, 1.0f);
    float a = zRotationAngles.r, b = zRotationAngles.g, c = zRotationAngles.b;
    glm::vec3 upperLegSize, middleLegSize, lowerLegSize;
    float hipLocationAsDegree = 30.0f, bodyHeight;

    int tryCount = 0;
    while (tryCount < 20) {
      upperLegSize =
          glm::vec3(0.05f, 1.25f, 0.05f) *
          glm::linearRand(
              data.spiderGenerationData.UpperLegSizeScaleMin,
              data.spiderGenerationData.UpperLegSizeScaleMax); // UpperLegSize
      middleLegSize =
          glm::vec3(0.05f, 1.0f, 0.05f) *
          glm::linearRand(
              data.spiderGenerationData.MiddleLegSizeScaleMin,
              data.spiderGenerationData.MiddleLegSizeScaleMax); // MiddleLegSize
      lowerLegSize =
          glm::vec3(0.05f, 0.5f, 0.05f) *
          glm::linearRand(
              data.spiderGenerationData.LowerLegSizeScaleMin,
              data.spiderGenerationData.LowerLegSizeScaleMax); // LowerLegSize
      bodyHeight = cos(a * deg2rad) * upperLegSize.y +
                   cos((a + b) * deg2rad) * middleLegSize.y +
                   cos((a + b + c) * deg2rad) * lowerLegSize.y;
      if (bodyHeight + bodySize.y * 0.5f < 0.0f) {
        bodyHeight =
            bodySize.y * sin(hipLocationAsDegree * deg2rad) * 0.5f - bodyHeight;
        break;
      }
      tryCount++;
    }

    if (tryCount == 20) {
      std::cout << "Spider generation failed after 20 attempts." << std::endl;
      continue;
    }

    bodyHeight -= bodySize.y * sin(hipLocationAsDegree * deg2rad) * 0.5f;

    float randomRotation = (rand() % 360) - 180;
    SpiderEntityData spiderData{
        Transform(glm::vec3(-AreaSize.x / 2.0f, -5.0f, -AreaSize.z / 2.0f) +
                      glm::vec3(glm::linearRand(0.0f, AreaSize.x), bodyHeight,
                                glm::linearRand(0.0f, AreaSize.z)),
                  glm::vec3(0.0f, randomRotation, 0.0f),
                  glm::vec3(1.0f, 1.0f, 1.0f)), // EntityTransform
        glm::linearRand(data.spiderGenerationData.LegPairCountMin,
                        data.spiderGenerationData.LegPairCountMax) *
            2,               // LegCount
        hipLocationAsDegree, // HipLocationAsDegree
        glm::linearRand(data.spiderGenerationData.MoveSpeedMin,
                        data.spiderGenerationData.MoveSpeedMax), // MoveSpeed
        bodySize,                                                // BodySize
        upperLegSize,                                            // UpperLegSize
        middleLegSize,                             // MiddleLegSize
        lowerLegSize,                              // LowerLegSize
        glm::vec3(0.0f, -5.0f, zRotationAngles.r), // UpperLegRotationAngle
        glm::vec3(0.0f, 0.0f, zRotationAngles.g),  // MiddleLegRotationAngle
        glm::vec3(0.0f, 0.0f, zRotationAngles.b),  // LowerLegRotationAngle
        glm::vec2(-AreaSize.x / 2.0f, -AreaSize.z / 2.0f), // patrolAreaMin
        glm::vec2(AreaSize.x / 2.0f, AreaSize.z / 2.0f)    // patrolAreaMax
    };

    Spider *spider = new Spider(spiderData);
    spidersParent->AddChild(spider->GetNode());
    spiders.emplace_back(spider);
  }
}

void Game::InitializeCaterpillars() {
  for (Caterpillar *caterpillar : caterpillars) {
    delete caterpillar;
  }

  caterpillars.clear();
  caterpillarsParent->RemoveFromParent();
  delete caterpillarsParent;
  caterpillarsParent = new Node();
  root.AddChild(caterpillarsParent);

  // Generate Caterpillars

  for (int i = 0; i < data.caterpillarGenerationData.EntityCount; i++) {
    float deg2rad = glm::pi<float>() / 180.0f;
    glm::vec3 zRotationAngles(60.0f, 60.0f, 60.0f);
    glm::vec3 bodySize = glm::vec3(0.8f, 0.4f, 1.0f);
    float a = zRotationAngles.r, b = zRotationAngles.g, c = zRotationAngles.b;

    float randomRotation = (rand() % 360) - 180;
    CaterpillarEntityData caterpillarData{
        Transform(glm::vec3(-AreaSize.x / 2.0f, -5.0f, -AreaSize.z / 2.0f) +
                      glm::vec3(glm::linearRand(0.0f, AreaSize.x),
                                bodySize.y / 2.0f,
                                glm::linearRand(0.0f, AreaSize.z)),
                  glm::vec3(0.0f, randomRotation, 0.0f),
                  glm::vec3(1.0f, 1.0f, 1.0f)), // EntityTransform
        glm::linearRand(data.caterpillarGenerationData.LegPairCountMin,
                        data.caterpillarGenerationData.LegPairCountMax) *
            2, // segmentCount
        glm::linearRand(
            data.caterpillarGenerationData.MoveSpeedMin,
            data.caterpillarGenerationData.MoveSpeedMax),  // MoveSpeed
        bodySize,                                          // BodySize
        glm::vec2(-AreaSize.x / 2.0f, -AreaSize.z / 2.0f), // patrolAreaMin
        glm::vec2(AreaSize.x / 2.0f, AreaSize.z / 2.0f)    // patrolAreaMax
    };

    Caterpillar *caterpillar = new Caterpillar(caterpillarData);
    caterpillarsParent->AddChild(caterpillar->GetNode());
    caterpillars.emplace_back(caterpillar);
  }
}

void Game::InitializeProducers() {
  for (Producer *producer : producers) {
    delete producer;
  }

  producers.clear();
  producersParent->RemoveFromParent();
  delete producersParent;
  producersParent = new Node();
  root.AddChild(producersParent);

  // Generate Producers

  for (int i = 0; i < data.producerGenerationData.EntityCount; i++) {
    glm::vec3 bodySize = glm::vec3(1.0f, 0.85f, 1.0f);

    float randomRotation = (rand() % 360) - 180;
    ProducerEntityData producerData{
        Transform(glm::vec3(-AreaSize.x / 2.0f, -5.0f, -AreaSize.z / 2.0f) +
                      glm::vec3(glm::linearRand(0.0f, AreaSize.x),
                                bodySize.y / 2.0f,
                                glm::linearRand(0.0f, AreaSize.z)),
                  glm::vec3(0.0f, randomRotation, 0.0f),
                  glm::vec3(1.0f, 1.0f, 1.0f)), // EntityTransform
        bodySize,                               // BodySize
    };

    Producer *producer = new Producer(producerData);
    producersParent->AddChild(producer->GetNode());
    producers.emplace_back(producer);
  }
}