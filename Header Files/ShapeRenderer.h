#ifndef SHAPE_RENDERER_H
#define SHAPE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <VAO.h>
#include <VBO.h>
#include <EBO.h>
#include <ShapeType.h>
#include <Singleton.h>
#include <Shader.h>
#include <Material.h>
#include <ShaderManager.h>

class ShapeRenderer : public Singleton<ShapeRenderer>
{
    friend Singleton<ShapeRenderer>;

public:
    void Setup();
    void Clear();
    void Draw(ShapeType shapeType, unsigned int modelLoc, const GLfloat *value, Material material, Shader shaderProgram);
};

class CubeData : public Singleton<CubeData>
{
    friend Singleton<ShapeRenderer>;

public:
    void SetupData();
    void ClearData();
    void DrawShape(unsigned int modelLoc, const GLfloat *value);

private:
    VAO *CurrentVAO;
    VBO *CurrentVBO;
    EBO *CurrentEBO;
    int vertexCount = 144;
    int indexCount = 36;
    float *GetVertices();
    unsigned int *GetIndices();
};

class SphereData : public Singleton<SphereData>
{
    friend Singleton<ShapeRenderer>;

public:
    void SetupData();
    void ClearData();
    void DrawShape(unsigned int modelLoc, const GLfloat *value);

private:
    VAO *CurrentVAO;
    VBO *CurrentVBO;
    EBO *CurrentEBO;
    int detailLevel = 20;
    int vertexCount;
    int indexCount;
    float *GetVertices();
    unsigned int *GetIndices();
};
#endif