#include <ShapeRenderer.h>
#include <Game.h>

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

void ShapeRenderer::Draw(ShapeType shapeType, RenderingLayer layer, unsigned int modelLoc, const GLfloat *value, Material material, Shader shaderProgram)
{
    shaderProgram.use();
    shaderProgram.setMaterial("material", material);

    if (layer == Polygon)
    {
        if (Game::data.showColliders == false)
        {
            return;
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else if (Game::data.polygonMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

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