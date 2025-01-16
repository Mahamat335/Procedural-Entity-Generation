#include <ShapeRenderer.h>
#define M_PI 3.14159265358979323846

void SphereData::SetupData()
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

void SphereData::ClearData()
{
    CurrentVAO->Delete();
    CurrentVBO->Delete();
    CurrentEBO->Delete();
}

void SphereData::DrawShape(unsigned int modelLoc, const GLfloat *value)
{
    CurrentVAO->Bind();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    CurrentVAO->Unbind();
}

float *SphereData::GetVertices()
{
    float *vertices = nullptr;

    float radius = 0.5f;

    // Calculate the number of vertices
    vertexCount = (detailLevel) * (detailLevel) * 6;

    if (vertices == nullptr)
    {
        vertices = new float[vertexCount * 6]; // Position and Normal for each vertex
    }

    int index = 0;

    // Iterate over latitude and longitude divisions
    for (int i = 0; i < detailLevel; i++)
    {
        float theta = (i * M_PI) / (detailLevel - 1); // Latitude angle
        for (int j = 0; j < detailLevel; j++)
        {
            float phi = (j * 2 * M_PI) / (detailLevel - 1); // Longitude angle

            float x = radius * sin(theta) * cos(phi);
            float y = radius * sin(theta) * sin(phi);
            float z = radius * cos(theta);

            // Normal is just the normalized position (since the sphere is centered at the origin)
            float nx = x / radius;
            float ny = y / radius;
            float nz = z / radius;

            // Store position and normal
            vertices[index++] = x;
            vertices[index++] = y;
            vertices[index++] = z;
            vertices[index++] = nx;
            vertices[index++] = ny;
            vertices[index++] = nz;
        }
    }

    return vertices;
}

unsigned int *SphereData::GetIndices()
{
    static unsigned int *indices = nullptr;

    // Calculate the number of indices for the sphere
    indexCount = (detailLevel - 1) * (detailLevel - 1) * 6; // 2 triangles per grid square

    if (indices == nullptr)
    {
        indices = new unsigned int[indexCount];
    }

    int index = 0;
    for (int i = 0; i < detailLevel - 1; i++)
    {
        for (int j = 0; j < detailLevel - 1; j++)
        {
            int topLeft = i * detailLevel + j;
            int topRight = i * detailLevel + (j + 1);
            int bottomLeft = (i + 1) * detailLevel + j;
            int bottomRight = (i + 1) * detailLevel + (j + 1);

            // First triangle (Clockwise order for correct culling with GL_FRONT)
            indices[index++] = topLeft;
            indices[index++] = topRight;
            indices[index++] = bottomLeft;

            // Second triangle (Clockwise order for correct culling with GL_FRONT)
            indices[index++] = topRight;
            indices[index++] = bottomRight;
            indices[index++] = bottomLeft;
        }
    }

    return indices;
}
