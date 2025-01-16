#include <Game.h>
#include <CubeCollider.h>
#include <Material.h>
#include <Lights.h>
#include <ctime>

Entity *floor1[10][10];
Entity root, object, child, grandChild, o2, o3, floor2, wall, spider, sBody, *sUpperLegs[6], *sLowerLegs[6], *sUpperLegsPivot[6], *sLowerLegsPivot[6];
const glm::vec3 BodySize(1.0f, 1.0f, 1.0f), UpperLegSize(0.1f, 1.0f, 0.1f), LowerLegSize(0.1f, 1.5f, 0.1f), LegRotationAngle(30.0f, 0.0f, 0.0f);
glm::vec3 SpiderLocation(8.0f, 3.0f, 7.0f);
Game::GameData Game::data = {};
const float RotationAngle = 60.0f;

Game::Game()
{
}

bool Game::Start()
{
    data = {};
    root;
    object = Entity(Transform(glm::vec3(0.0f, 0.0f, 0.0f)), CUBE, Emerald);
    child = Entity(Transform(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f)), SPHERE, RedPlastic);
    grandChild = Entity(Transform(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f)), SPHERE, CyanPlastic);
    o2 = Entity(Transform(glm::vec3(10.0f, 0.0f, 0.0f)), SPHERE, Turquoise);
    o3 = Entity(Transform(glm::vec3(12.0f, 0.0f, 0.0f)), CUBE, YellowRubber);
    floor2 = Entity(Transform(glm::vec3(0.0f, -20.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(50.0f, 1.0f, 50.0f)), SPHERE, Chrome);
    wall = Entity(Transform(glm::vec3(25.0f, -20.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 30.0f, 30.0f)), CUBE, Gold);
    root.AddChild(&o2);
    root.AddChild(&o3);

    root.AddChild(&object);
    object.AddChild(&child);
    child.AddChild(&grandChild);

    root.AddChild(&floor2);
    root.AddChild(&wall);

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            floor1[i][j] = new Entity(Transform(glm::vec3((float)i, -10.0f, (float)j)));
            root.AddChild(floor1[i][j]);
            if (i % 5 && j % 5)
                floor1[i][j]->isEnable = false;
            if ((i % 5 == 2 || i % 5 == 3) && (j % 5 == 2 || j % 5 == 3))
                floor1[i][j]->isEnable = true;
        }
    }

    spider = Entity(Transform(SpiderLocation, glm::vec3(), BodySize), SPHERE, Obsidian);
    root.AddChild(&spider);

    float r = BodySize.y * 0.5f;                        // Kürenin yarıçapı
    float verticalAngle = -glm::pi<float>() / 6.0f;     // Dikey açı (kürenin alt tarafına yakın)
    float horizontalStep = glm::two_pi<float>() / 6.0f; // 6 bacak için eşit yatay açı

    for (int i = 0; i < 6; i++)
    {
        float theta = i * horizontalStep; // Her bacak için yatay açı
        float phi = verticalAngle;        // Sabit dikey açı

        // Kürenin yüzeyindeki noktaları hesapla
        float x = r * cos(phi) * sin(theta);
        float y = r * sin(phi);
        float z = r * cos(phi) * cos(theta);

        // Pivot noktasını tanımla
        sUpperLegsPivot[i] = new Entity(Transform(glm::vec3(x, y, z), LegRotationAngle * (z > 0 ? 1.0f : -1.0f), glm::vec3(0.01f, 0.01f, 0.01f)), CUBE);
        sUpperLegs[i] = new Entity(Transform(UpperLegSize * 0.5f * glm::vec3(0.0f, 100.0f, 0.0f), glm::vec3(), UpperLegSize * glm::vec3(100.0f, 100.0f, 100.0f)), SPHERE, Ruby);
        sLowerLegsPivot[i] = new Entity(Transform(UpperLegSize * glm::vec3(0.0f, 100.0f, 0.0f), LegRotationAngle * (z > 0 ? 4.0f : -4.0f)), CUBE);
        sLowerLegs[i] = new Entity(Transform(LowerLegSize * 0.5f * glm::vec3(0.0f, 100.0f, 0.0f), glm::vec3(), LowerLegSize * glm::vec3(100.0f, 100.0f, 100.0f)), SPHERE, Ruby);
        spider.AddChild(sUpperLegsPivot[i]);
        sUpperLegsPivot[i]->AddChild(sUpperLegs[i]);
        sUpperLegsPivot[i]->AddChild(sLowerLegsPivot[i]);
        sLowerLegsPivot[i]->AddChild(sLowerLegs[i]);
    }

    // Light Calculations
    DirectionalLight directionalLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.5f, -1.0f, -0.5f));
    PointLight pointLight(0, glm::vec3(2.0f, 2.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f));

    return true;
}

bool Game::Update(float deltaTime)
{

    /* CubeCollider *c1 = new CubeCollider(&object, object.transform.pos, glm::vec3(1.0f, 1.0f, 1.0f));

    CubeCollider *c2 = new CubeCollider(&o2, o2.transform.pos, glm::vec3(1.0f, 1.0f, 1.0f));
    if (c1->CheckForCollision(c2))
    {
        // std::cout << 1;
    } */
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            floor1[i][j]->Rotate(floor1[i][j]->transform.eulerRot + glm::vec3(20 * deltaTime, 15 * deltaTime, (200 - i - j) * deltaTime));
    object.Move(data.playerVel + object.transform.pos);
    object.Rotate(glm::vec3(0.0f, (float)deltaTime * 50, 0.0f) + object.transform.eulerRot);
    object.Rotate(glm::vec3(0.0f, 0.0f, (float)deltaTime * 90 * data.playerAngularSpeed) + object.transform.eulerRot);
    child.Rotate(glm::vec3((float)glfwGetTime() * 50, (float)glfwGetTime() * 30, (float)glfwGetTime() * 10) + object.transform.eulerRot);
    grandChild.Rotate(glm::vec3((float)glfwGetTime() * -50, (float)glfwGetTime() * -30, (float)glfwGetTime() * -10) + object.transform.eulerRot);

    // spider movement
    spider.Move(data.playerVel + spider.transform.pos);
    /*  for (int i = 0; i < 6; i++)
     {
         sLowerLegsPivot[i]->Rotate(sLowerLegsPivot[i]->transform.eulerRot + glm::vec3(0.0f, 0.0f, cos(std::time(NULL) * RotationAngle)));
     }
  */
    PointLight pointLight(1, object.transform.pos, glm::vec3(1.0f, 0.2f, 1.0f));

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