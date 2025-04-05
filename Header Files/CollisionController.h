#ifndef COLLISION_CONTROLLER_H
#define COLLISION_CONTROLLER_H

#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <SphereCollider.h>
#include <glm/gtx/hash.hpp>

class CollisionController : public Singleton<CollisionController>
{
    friend Singleton<CollisionController>;

private:
    std::unordered_map<glm::ivec2, std::vector<SphereCollider *>> __hashGrid;

public:
    const float ChunkSize = 1.0f;
    glm::ivec2 GetChunkIndex(const glm::vec3 &position) const
    {
        return glm::ivec2(
            static_cast<int>(position.x / ChunkSize),
            static_cast<int>(position.z / ChunkSize));
    }

    void AddCollider(SphereCollider *collider)
    {
        glm::ivec2 chunk = GetChunkIndex(collider->Center);
        __hashGrid[chunk].push_back(collider);
    }

    void RemoveCollider(SphereCollider *collider)
    {
        glm::ivec2 chunk = GetChunkIndex(collider->Center);
        auto &colliders = __hashGrid[chunk];
        colliders.erase(std::remove(colliders.begin(), colliders.end(), collider), colliders.end());
    }

    void UpdateCollider(SphereCollider *collider, const glm::vec3 &newPosition)
    {
        glm::ivec2 oldChunk = GetChunkIndex(collider->Center);
        glm::ivec2 newChunk = GetChunkIndex(newPosition);

        if (oldChunk != newChunk)
        {
            RemoveCollider(collider);
            collider->Center = newPosition;
            AddCollider(collider);
        }
        else
        {
            collider->Center = newPosition;
        }
    }

    bool CheckForCollisions(SphereCollider *collider)
    {
        glm::ivec2 chunk = GetChunkIndex(collider->Center);

        // Çevresindeki 9 chunk'ı kontrol et
        for (int dx = -1; dx <= 1; dx++)
        {
            for (int dy = -1; dy <= 1; dy++)
            {
                glm::ivec2 neighborChunk = chunk + glm::ivec2(dx, dy);
                auto it = __hashGrid.find(neighborChunk);
                if (it != __hashGrid.end())
                {
                    for (SphereCollider *other : it->second)
                    {
                        if (other != collider && collider->CheckForCollision(other))
                        {
                            return true; // Çarpışma var
                        }
                    }
                }
            }
        }
        return false; // Çarpışma yok
    }
};
#endif