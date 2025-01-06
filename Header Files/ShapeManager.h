#ifndef SHAPE_MANAGER_H
#define SHAPE_MANAGER_H
#include <vector>
#include <Entity.h>
#include <iterator>
#include <ShapeType.h>

class ShapeManager
{
public:
    static unsigned int ModelLoc;
    static const unsigned int CubeVerticesSize = 264 * sizeof(float);
    static const unsigned int CubeIndicesSize = 36 * sizeof(unsigned int);
    static const unsigned int SphereVerticesSize = 108 * sizeof(float);
    static const unsigned int SphereIndicesSize = 60 * sizeof(unsigned int);

    static ShapeManager *Instance()
    {
        if (instance == nullptr)
        {
            instance = new ShapeManager();
        }
        return instance;
    }

    void print()
    {
        std::cout << "Hello" << std::endl;
    }

    const float *GetVertices(ShapeType shape)
    {
        switch (shape)
        {
        case CUBE:
            return _cubeVertices;
        case SPHERE:
            return _sphereVertices;
        }
        return nullptr;
    }

    const unsigned int *GetIndices(ShapeType shape)
    {
        switch (shape)
        {
        case CUBE:
            return _cubeIndices;
        case SPHERE:
            return _sphereIndices;
        }
        return nullptr;
    }

    static void Draw(ShapeType shape)
    {
        for (auto &entity : Entity::CubeEntities)
        {
            entity->DrawMesh(ModelLoc);
        }
        for (auto &entity : Entity::SphereEntities)
        {
            entity->DrawMesh(ModelLoc);
        }
    }

private:
    static ShapeManager *instance;
    const float _cubeVertices[264] = {
        // Back face
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,

        // Front face
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        // Left face
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,

        // Right face
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        // Bottom face
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,

        // Top face
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};

    const unsigned int _cubeIndices[36] = {
        // Back face
        0, 1, 2,
        2, 3, 0,

        // Front face
        4, 5, 6,
        6, 7, 4,

        // Left face
        8, 9, 10,
        10, 11, 8,

        // Right face
        12, 13, 14,
        14, 15, 12,

        // Bottom face
        16, 17, 18,
        18, 19, 16,

        // Top face
        20, 21, 22,
        22, 23, 20};

    const float _sphereVertices[108] = {
    // Pozisyon            // Renk          // Normaller
     0.000f,  1.000f,  0.000f,  1.0f, 1.0f, 1.0f,  0.000f,  1.000f,  0.000f,
     0.894f,  0.447f,  0.000f,  1.0f, 0.0f, 0.0f,  0.894f,  0.447f,  0.000f,
     0.276f,  0.447f,  0.851f,  0.0f, 1.0f, 0.0f,  0.276f,  0.447f,  0.851f,
    -0.724f,  0.447f,  0.526f,  0.0f, 0.0f, 1.0f, -0.724f,  0.447f,  0.526f,
    -0.724f,  0.447f, -0.526f,  1.0f, 1.0f, 0.0f, -0.724f,  0.447f, -0.526f,
     0.276f,  0.447f, -0.851f,  1.0f, 0.0f, 1.0f,  0.276f,  0.447f, -0.851f,
     0.724f, -0.447f,  0.526f,  0.0f, 1.0f, 1.0f,  0.724f, -0.447f,  0.526f,
    -0.276f, -0.447f,  0.851f,  1.0f, 0.5f, 0.5f, -0.276f, -0.447f,  0.851f,
    -0.894f, -0.447f,  0.000f,  0.5f, 1.0f, 0.5f, -0.894f, -0.447f,  0.000f,
    -0.276f, -0.447f, -0.851f,  0.5f, 0.5f, 1.0f, -0.276f, -0.447f, -0.851f,
     0.724f, -0.447f, -0.526f,  1.0f, 1.0f, 1.0f,  0.724f, -0.447f, -0.526f,
     0.000f, -1.000f,  0.000f,  0.0f, 0.0f, 0.0f,  0.000f, -1.000f,  0.000f,
};

   const unsigned int _sphereIndices[60] = {
     0,  1,  2,  0,  2,  3,  0,  3,  4,  0,  4,  5, 
     0,  5,  1,  1,  6,  2,  2,  7,  3,  3,  8,  4, 
     4,  9,  5,  5, 10,  1,  6,  7,  2,  7,  8,  3, 
     8,  9,  4,  9, 10,  5, 10,  6,  1,  6, 11,  7, 
     7, 11,  8,  8, 11,  9,  9, 11, 10, 10, 11,  6,
};
};


ShapeManager *ShapeManager::instance = nullptr;
#endif