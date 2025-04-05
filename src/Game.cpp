#include <Game.h>
#include <CubeCollider.h>
#include <Material.h>
#include <Lights.h>
#include <ctime>
#include <Spider.h>
#include <SpiderEntityData.h>
#include <CollisionController.h>

Node root, area, spider, sBody, *sUpperLegs[6], *sLowerLegs[6], *sUpperLegsPivot[6], *sLowerLegsPivot[6];
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
    
    for (int i = 2; i < 10; i += 2)
    {
        for (int j = 2; j < 10; j += 2)
        {
            float deg2rad = glm::pi<float>() / 180.0f;
            glm::vec3 zRotationAngles(60.0f, 60.0f, 60.0f), yLegSizes(glm::vec3(1.25f, 1.0f, 0.5f) * (j / 4.0f));
            float a = zRotationAngles.r, b = zRotationAngles.g, c = zRotationAngles.b;

            float bodyHeight = cos(a * deg2rad) * yLegSizes.r + cos((a + b) * deg2rad) * yLegSizes.g + cos((a + b + c) * deg2rad) * yLegSizes.b;
            float randomRotation = (rand() % 360) - 180;
            float hipLocationAsDegree = 30.0f;
            glm::vec3 bodySize = glm::vec3(0.8f, 0.4f, 1.0f);
            bodyHeight -= bodySize.y * sin(hipLocationAsDegree * deg2rad) * 0.5f;

            SpiderEntityData spiderData{
                Transform(glm::vec3(-35.0f, -5.0f, -35.0f) + glm::vec3(j * 6.0f, -bodyHeight, i * 6.0f), glm::vec3(0.0f, randomRotation, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)), // EntityTransform
                i,                                                                                                                                                            // LegCount
                hipLocationAsDegree,                                                                                                                                          // HipLocationAsDegree
                0.5f,                                                                                                                                                         // MoveSpeed
                bodySize,                                                                                                                                                     // BodySize
                glm::vec3(0.05f, 1.25f, 0.05f) * (j / 4.0f),                                                                                                                  // UpperLegSize
                glm::vec3(0.05f, 1.0f, 0.05f) * (j / 4.0f),                                                                                                                   // MiddleLegSize
                glm::vec3(0.05f, 0.5f, 0.05f) * (j / 4.0f),                                                                                                                   // LowerLegSize
                glm::vec3(0.0f, -5.0f, zRotationAngles.r),                                                                                                                    // UpperLegRotationAngle
                glm::vec3(0.0f, 0.0f, zRotationAngles.g),                                                                                                                     // MiddleLegRotationAngle
                glm::vec3(0.0f, 0.0f, zRotationAngles.b),                                                                                                                     // LowerLegRotationAngle
            };

            Spider *spider = new Spider(spiderData);
            root.AddChild(spider->GetNode());
            spiders.emplace_back(spider);
        }
    }

    // Light Calculations
    DirectionalLight directionalLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.5f, -0.2f, -0.5f));
    PointLight pointLight(0, glm::vec3(2.0f, 2.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f));

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
        spider->Move(deltaTime);
    }

   // for (Spider *spider : spiders)
   // {
      //  if (CollisionController::Instance().CheckForCollisions(spider->GetCollider()))
     //   {
       //     std::cout << "Collision detected after movement! \n";
     //   }
 //   }

    RenderEntities(root, data.modelLoc);
    return true;
}

void Game::End()
{
    // Code to execute once at the end
}

void Game::RenderEntities(Node root, unsigned int modelLoc)
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
            current->DrawMesh(modelLoc);
        }
        else
        {
            current->DrawMesh(modelLoc);
        }
        for (auto &child : current->children)
        {
            if (child->isEnable)
                stack.push(child);
        }
    }
}