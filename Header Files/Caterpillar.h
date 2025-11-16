#ifndef CATERPILLAR_H
#define CATERPILLAR_H

#include <EntityData.h>
#include <IEater.h>
#include <IEntity.h>
#include <Node.h>
#include <SphereCollider.h>
#include <glm/glm.hpp>

class Caterpillar : public IEntity, public IEater {
public:
  Caterpillar(CaterpillarEntityData Data);
  ~Caterpillar();
  Node *GetNode();
  void Move(float deltaTime);
  void SetCollider(SphereCollider *sphereCollider);
  void SetMoveSpeed(float movespeed);
  SphereCollider *GetCollider();
  void PickNewTarget();
  void OnCollisionEnter(IEntity *other);
  void OnCollisionStay(IEntity *other);
  void OnCollisionExit(IEntity *other);
  void Die();
  void SizeUp();

private:
  Transform _caterpillarTransform;
  int _segmentCount;
  float _hipLocationAsDegree, _moveSpeed, _patrolDirection;
  glm::vec3 _bodySize;

  Node *_caterpillarNode, *_cBody;
  SphereCollider *_caterpillarCollider;
  glm::vec2 _patrolAreaMin;
  glm::vec2 _patrolAreaMax;
  glm::vec3 _targetPosition;
  std::vector<glm::vec3> _positionHistory;
};
#endif