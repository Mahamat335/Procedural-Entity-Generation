#ifndef COLLISION_CONTROLLER_H
#define COLLISION_CONTROLLER_H

#include <SphereCollider.h>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>
#include <vector>

class CollisionController : public Singleton<CollisionController> {
  friend Singleton<CollisionController>;

private:
  std::unordered_map<glm::ivec2, std::vector<SphereCollider *>> __hashGrid;

public:
  const float ChunkSize = 2.0f;
  glm::ivec2 GetChunkIndex(const glm::vec3 &position) const {
    return glm::ivec2(static_cast<int>(position.x / ChunkSize),
                      static_cast<int>(position.z / ChunkSize));
  }

  void AddCollider(SphereCollider *collider) {
    glm::ivec2 chunk = GetChunkIndex(collider->Center);
    __hashGrid[chunk].push_back(collider);
    collider->registeredChunk = chunk;
  }

  void RemoveCollider(SphereCollider *collider) {
    glm::ivec2 chunk = collider->registeredChunk;
    auto it = __hashGrid.find(chunk);
    auto &colliders = __hashGrid[chunk];
    if (it != __hashGrid.end()) {
      auto &colliders = it->second;
      colliders.erase(std::remove(colliders.begin(), colliders.end(), collider),
                      colliders.end());
    }
  }

  void UpdateCollider(SphereCollider *collider) {
    glm::ivec2 currentChunk = GetChunkIndex(collider->Center);
    if (currentChunk != collider->registeredChunk) {
      RemoveCollider(collider);
      AddCollider(collider);
    }
  }

  void CheckForCollisions(SphereCollider *collider) {
    glm::ivec2 chunk = GetChunkIndex(collider->Center);

    for (int dx = -1; dx <= 1; dx++) {
      for (int dy = -1; dy <= 1; dy++) {
        glm::ivec2 neighborChunk = chunk + glm::ivec2(dx, dy);
        auto it = __hashGrid.find(neighborChunk);
        if (it != __hashGrid.end()) {
          for (SphereCollider *other : it->second) {
            if (other == collider) {
              continue; // Skip self-collision
            }

            if (collider->CheckForCollision(other)) {
              if (collider->Collisions.find(other) !=
                  collider->Collisions.end()) {
                collider->OnCollisionStay(other);
              } else {
                collider->OnCollisionEnter(other);
              }
            } else {
              if (collider->Collisions.find(other) !=
                  collider->Collisions.end()) {
                collider->OnCollisionExit(other);
              }
            }
          }
        }
      }
    }
  }
};
#endif