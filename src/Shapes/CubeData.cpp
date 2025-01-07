#include <ShapeRenderer.h>

void CubeData::SetupData()
{
    CurrentVAO = new VAO();
    CurrentVAO->Bind();
    CurrentVBO = new VBO(GetVertices(), vertexCount * sizeof(float));
    CurrentEBO = new EBO(GetIndices(), indexCount * sizeof(unsigned int));
    CurrentVAO->LinkAttrib(*CurrentVBO, 0, 3, GL_FLOAT, 6 * sizeof(float), (void *)0);
    CurrentVAO->LinkAttrib(*CurrentVBO, 1, 3, GL_FLOAT, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    CurrentVAO->Unbind();
    CurrentVBO->Unbind();
    CurrentEBO->Unbind();
}

void CubeData::ClearData()
{
    CurrentVAO->Delete();
    CurrentVBO->Delete();
    CurrentEBO->Delete();
}

void CubeData::DrawShape(unsigned int modelLoc, const GLfloat *value)
{
    CurrentVAO->Bind();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    CurrentVAO->Unbind();
}

float *CubeData::GetVertices()
{
    return new float[vertexCount]{
        // Back face
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

        // Front face
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        // Left face
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

        // Right face
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

        // Bottom face
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,

        // Top face
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f};
}

unsigned int *CubeData::GetIndices()
{
    return new unsigned int[indexCount]{
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
}
