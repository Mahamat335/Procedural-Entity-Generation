#include <ShapeRenderer.h>

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

void ShapeRenderer::Draw(ShapeType shapeType, unsigned int modelLoc, const GLfloat *value)
{
    switch (shapeType)
    {
    case CUBE:
        CubeData::Instance().DrawShape(modelLoc, value);
        break;
    case SPHERE:
        SphereData::Instance().DrawShape(modelLoc, value);
        break;
    }
};