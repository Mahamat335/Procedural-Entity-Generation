#ifndef ENTITY_H
#define ENTITY_H

#include <EntityData.h>
#include <INutrient.h>
#include <Node.h>
#include <glm/glm.hpp>

class SphereCollider;

class IEntity : public INutrient {
public:
  virtual ~IEntity() = default;
  virtual Node *GetNode() = 0;
  virtual void Move(float deltaTime) = 0;
  virtual void SetCollider(SphereCollider *sphereCollider) = 0;
  virtual SphereCollider *GetCollider() = 0;
  virtual void OnCollisionEnter(IEntity *other) = 0;
  virtual void OnCollisionStay(IEntity *other) = 0;
  virtual void OnCollisionExit(IEntity *other) = 0;
  virtual void Die() = 0;
};
#endif