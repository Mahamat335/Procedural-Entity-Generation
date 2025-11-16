#ifndef PRODUCER_H
#define PRODUCER_H

#include <glm/glm.hpp>
#include <Node.h>
#include <EntityData.h>
#include <SphereCollider.h>
#include <IEntity.h>
#include <IEater.h>

class Producer : public IEntity
{
public:
    Producer(ProducerEntityData Data);
    Node *GetNode();
    void Move(float deltaTime);
    void SetCollider(SphereCollider *sphereCollider);
    SphereCollider *GetCollider();
    void OnCollisionEnter(IEntity *other);
    void OnCollisionStay(IEntity *other);
    void OnCollisionExit(IEntity *other);
    void Die();
    void Destroy();

private:
    Transform _producerTransform;
    glm::vec3 _bodySize;

    Node *_producerNode, *_pBody;
    SphereCollider *_producerCollider;
};
#endif