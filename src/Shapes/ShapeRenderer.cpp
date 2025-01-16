#include <ShapeRenderer.h>
#include <ShaderManager.h>

void ShapeRenderer::Setup()
{
    CubeData::Instance().SetupData();
    SphereData::Instance().SetupData();
};

void ShapeRenderer::Clear()
{
    CubeData::Instance().ClearData();
    SphereData::Instance().ClearData();
};

void ShapeRenderer::Draw(ShapeType shapeType, unsigned int modelLoc, const GLfloat *value, Material material)
{
    Shader shaderProgram = *(ShaderManager::Instance().defaultShaderProgram);
    shaderProgram.use();
    shaderProgram.setMaterial("material", material);

    switch (shapeType)
    {
    case POINT:
        break;
    case CUBE:
        CubeData::Instance().DrawShape(modelLoc, value);
        break;
    case SPHERE:
        SphereData::Instance().DrawShape(modelLoc, value);
        break;
    }
};