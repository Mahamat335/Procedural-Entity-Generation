#ifndef ENTITY_H
#define ENTITY_H

#include <glm/glm.hpp>
#include <Node.h>
#include <EntityData.h>

class SphereCollider;

class IEntity
{
public:
    virtual ~IEntity() {}
    virtual Node *GetNode() = 0;
    virtual void Move(float deltaTime) = 0;
    virtual void SetCollider(SphereCollider *sphereCollider) = 0;
    virtual SphereCollider *GetCollider() = 0;
    virtual void OnCollisionEnter(IEntity *other) = 0;
    virtual void OnCollisionStay(IEntity *other) = 0;
    virtual void OnCollisionExit(IEntity *other) = 0;
    virtual void Die() = 0;
    virtual void Destroy() = 0;
};
#endif