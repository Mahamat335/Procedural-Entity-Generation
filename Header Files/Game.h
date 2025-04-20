#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stack>
#include <Node.h>
#include <Transform.h>
#include <glm/glm.hpp>
#include <Collider.h>
#include <SpiderGenerationData.h>

class Game
{
    struct GameData
    {
        glm::vec3 playerVel;
        float playerAngularSpeed = 0.0f;
        float playerSpeed = 5.0f;
        bool polygonMode = false;
        bool isCursorEnabled = false;
        unsigned int modelLoc;
        float moveSpeed = 0.5f;
        SpiderGenerationData spiderGenerationData{
            16,
            2,
            10,
            0.2f,
            0.8f,
            0.0f,
            3.0f,
            0.0f,
            3.0f,
            0.0f,
            3.0f,
            0.0f,
            3.0f,
        };
    };

public:
    static GameData data;
    Game();
    bool Start();
    bool Update(float deltaTime);
    void End();
    void RenderEntities(unsigned int modelLoc, Shader shaderProgram);
    void InitializeSpiders();
};

#endif // GAME_H
