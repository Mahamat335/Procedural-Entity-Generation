#ifndef SPHERE_COLLIDER_H
#define SPHERE_COLLIDER_H

#include <Collider.h>

class SphereCollider : public Collider {

public:
  float Radius;
  SphereCollider(IEntity *entity, float radius);
  ~SphereCollider();
  bool CheckForCollision(SphereCollider *collision);
  void Update();
  Node *ColliderNode;
  glm::ivec2 registeredChunk = glm::ivec2(-999, -999);
};

#endif
