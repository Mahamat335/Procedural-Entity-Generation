#include <Game.h>
#include <CubeCollider.h>
#include <Material.h>
#include <Lights.h>
#include <ctime>
#include <Spider.h>
#include <SpiderEntityData.h>
#include <CollisionController.h>
#include <glm/gtc/random.hpp>

Node root, area, *spidersParent;
const glm::vec3 AreaSize(70.0f, 0.1f, 70.0f);
Game::GameData Game::data = {};
const int LegCount = 6;
std::vector<Spider *> spiders;
float patrolTimer = 0.0f;
float patrolTime = 4.0f;

Game::Game()
{
}

bool Game::Start()
{
    data = {};

    area = Node(Transform(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), AreaSize), CUBE, Silver);
    root.AddChild(&area);

    spidersParent = new Node();
    root.AddChild(spidersParent);

    // Light Calculations
    DirectionalLight directionalLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.5f, -0.2f, -0.5f));
    // PointLight pointLight(0, glm::vec3(2.0f, 2.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f));

    return true;
}

bool Game::Update(float deltaTime)
{
    if (patrolTimer > patrolTime)
    {
        for (Spider *spider : spiders)
        {
            spider->Patrol();
        }
        patrolTimer = 0.0f;
    }
    patrolTimer += deltaTime;
    // spider movement
    for (Spider *spider : spiders)
    {
        spider->SetMoveSpeed(data.moveSpeed);
        spider->Move(deltaTime);
    }

    for (Spider *spider : spiders)
    {
        if (CollisionController::Instance().CheckForCollisions(spider->GetCollider()))
        {
            std::cout << "Collision detected after movement! \n";
        }
    }

    RenderEntities(data.modelLoc, *(ShaderManager::Instance().defaultShaderProgram));
    return true;
}

void Game::End()
{
    // Code to execute once at the end
}

void Game::RenderEntities(unsigned int modelLoc, Shader shaderProgram)
{
    std::stack<Node *> stack;
    for (auto &child : root.children)
    {
        if (child->isEnable)
            stack.push(child);
    }

    while (!stack.empty())
    {
        Node *current = stack.top();
        stack.pop();
        if (current->Shape == CUBE)
        {
            current->DrawMesh(modelLoc, shaderProgram);
        }
        else
        {
            current->DrawMesh(modelLoc, shaderProgram);
        }
        for (auto &child : current->children)
        {
            if (child->isEnable)
                stack.push(child);
        }
    }
}

void Game::InitializeSpiders()
{
    // Clear Spiders

    spiders.clear();
    spidersParent->Destroy();
    spidersParent = new Node();
    root.AddChild(spidersParent);

    // Generate Spiders

    for (int i = 0; i < data.spiderGenerationData.SpiderCount; i++)
    {
        float deg2rad = glm::pi<float>() / 180.0f;
        glm::vec3 zRotationAngles(60.0f, 60.0f, 60.0f),
            yLegSizes(glm::vec3(1.25f, 1.0f, 0.5f) * glm::linearRand(data.spiderGenerationData.LegScaleMin, data.spiderGenerationData.LegScaleMax));
        float a = zRotationAngles.r, b = zRotationAngles.g, c = zRotationAngles.b;

        float bodyHeight = cos(a * deg2rad) * yLegSizes.r + cos((a + b) * deg2rad) * yLegSizes.g + cos((a + b + c) * deg2rad) * yLegSizes.b;
        float randomRotation = (rand() % 360) - 180;
        float hipLocationAsDegree = 30.0f;
        glm::vec3 bodySize = glm::vec3(0.8f, 0.4f, 1.0f);
        bodyHeight -= bodySize.y * sin(hipLocationAsDegree * deg2rad) * 0.5f;

        SpiderEntityData spiderData{
            Transform(glm::vec3(-AreaSize.x / 2.0f, -5.0f, -AreaSize.z / 2.0f) + glm::vec3(glm::linearRand(0.0f, AreaSize.x), -bodyHeight, glm::linearRand(0.0f, AreaSize.z)), glm::vec3(0.0f, randomRotation, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)), // EntityTransform
            glm::linearRand(data.spiderGenerationData.LegCountMin, data.spiderGenerationData.LegCountMax),                                                                                                                                          // LegCount
            hipLocationAsDegree,                                                                                                                                                                                                                    // HipLocationAsDegree
            glm::linearRand(data.spiderGenerationData.MoveSpeedMin, data.spiderGenerationData.MoveSpeedMax),                                                                                                                                        // MoveSpeed
            bodySize,                                                                                                                                                                                                                               // BodySize
            glm::vec3(0.05f, 1.25f, 0.05f) * glm::linearRand(data.spiderGenerationData.UpperLegSizeScaleMin, data.spiderGenerationData.UpperLegSizeScaleMax),                                                                                       // UpperLegSize
            glm::vec3(0.05f, 1.0f, 0.05f) * glm::linearRand(data.spiderGenerationData.MiddleLegSizeScaleMin, data.spiderGenerationData.MiddleLegSizeScaleMax),                                                                                      // MiddleLegSize
            glm::vec3(0.05f, 0.5f, 0.05f) * glm::linearRand(data.spiderGenerationData.LowerLegSizeScaleMin, data.spiderGenerationData.LowerLegSizeScaleMax),                                                                                        // LowerLegSize
            glm::vec3(0.0f, -5.0f, zRotationAngles.r),                                                                                                                                                                                              // UpperLegRotationAngle
            glm::vec3(0.0f, 0.0f, zRotationAngles.g),                                                                                                                                                                                               // MiddleLegRotationAngle
            glm::vec3(0.0f, 0.0f, zRotationAngles.b),                                                                                                                                                                                               // LowerLegRotationAngle
        };

        Spider *spider = new Spider(spiderData);
        spidersParent->AddChild(spider->GetNode());
        spiders.emplace_back(spider);
    }
}