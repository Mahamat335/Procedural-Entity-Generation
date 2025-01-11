#include <Game.h>
#include <CubeCollider.h>
#include <Material.h>
#include <Lights.h>

Entity *floor1[10][10];
Entity root, object, child, grandChild, o2, o3, floor2, wall;

Game::GameData Game::data = {};

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