#ifndef PRODUCER_H
#define PRODUCER_H

#include <EntityData.h>
#include <IEater.h>
#include <IEntity.h>
#include <Node.h>
#include <SphereCollider.h>
#include <glm/glm.hpp>

class Producer : public IEntity {
public:
  Producer(ProducerEntityData Data);
  ~Producer();
  Node *GetNode();
  void Move(float deltaTime);
  void SetCollider(SphereCollider *sphereCollider);
  SphereCollider *GetCollider();
  void OnCollisionEnter(IEntity *other);
  void OnCollisionStay(IEntity *other);
  void OnCollisionExit(IEntity *other);
  void Die();

private:
  Transform _producerTransform;
  glm::vec3 _bodySize;

  Node *_producerNode, *_pBody;
  SphereCollider *_producerCollider;
};
#endif