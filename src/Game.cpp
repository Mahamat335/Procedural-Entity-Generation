#include <Game.h>
#include <CubeCollider.h>
#include <Material.h>
#include <Lights.h>
#include <ctime>
#include <Spider.h>

Entity root, area, spider, sBody, *sUpperLegs[6], *sLowerLegs[6], *sUpperLegsPivot[6], *sLowerLegsPivot[6];
const glm::vec3 AreaSize(70.0f, 0.1f, 70.0f), BodySize(0.8f, 0.4f, 1.0f), UpperLegSize(0.05f, 1.25f, 0.05f), MiddleLegSize(0.05f, 1.0f, 0.05f), LowerLegSize(0.05f, 0.5f, 0.05f), UpperLegRotationAngle(0.0f, -5.0f, 60.0f), MiddleLegRotationAngle(0.0f, 0.0f, 60.0f), LowerLegRotationAngle(0.0f, 0.0f, 60.0f);
glm::vec3 SpiderLocation(-35.0f, -5.0f, -35.0f);
glm::vec3 SpiderSize(1.0f, 1.0f, 1.0f);
float MoveSpeed = 0.5f;
Game::GameData Game::data = {};
const int LegCount = 6;
const float HipLocationAsDegree = 30.0f;
std::vector<Spider *> spiders;
float patrolTimer = 0.0f;
float patrolTime = 4.0f;

Game::Game()
{
}

bool Game::Start()
{
    data = {};
    area = Entity(Transform(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), AreaSize), CUBE, Silver);
    root.AddChild(&area);
    for (int i = 2; i < 10; i += 2)
    {
        for (int j = 2; j < 10; j += 2)
        {
            float deg2rad = glm::pi<float>() / 180.0f;
            float a = UpperLegRotationAngle.z, b = MiddleLegRotationAngle.z, c = LowerLegRotationAngle.z;
            float bodyHeight = cos(a * deg2rad) * UpperLegSize.y + cos((a + b) * deg2rad) * MiddleLegSize.y + cos((a + b + c) * deg2rad) * LowerLegSize.y;
            float randomRotation = (rand() % 360) - 180;
            bodyHeight -= BodySize.y * sin(HipLocationAsDegree * deg2rad) * 0.5f;
            Spider *spider = new Spider(Transform(SpiderLocation + glm::vec3(j * 6.0f, -bodyHeight, i * 6.0f), glm::vec3(0.0f, randomRotation, 0.0f), SpiderSize), i, HipLocationAsDegree, BodySize, UpperLegSize * (j / 4.0f), MiddleLegSize * (j / 4.0f), LowerLegSize * (j / 4.0f), UpperLegRotationAngle, MiddleLegRotationAngle, LowerLegRotationAngle, MoveSpeed);
            root.AddChild(spider->GetEntity());
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

    RenderEntities(root, data.modelLoc);
}

void Game::End()
{
    // Code to execute once at the end
}

void Game::RenderEntities(Entity root, unsigned int modelLoc)
{
    std::stack<Entity *> stack;
    for (auto &child : root.children)
    {
        if (child->isEnable)
            stack.push(child);
    }

    while (!stack.empty())
    {
        Entity *current = stack.top();
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