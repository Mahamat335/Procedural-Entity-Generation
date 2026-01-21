#ifndef SPIDER_H
#define SPIDER_H

#include <EntityData.h>
#include <IEater.h>
#include <IEntity.h>
#include <Node.h>
#include <SphereCollider.h>
#include <glm/glm.hpp>

class Spider : public IEntity, public IEater {
public:
  Spider(SpiderEntityData Data);
  ~Spider();
  Node *GetNode() override;
  void Move(float deltaTime) override;
  void SetCollider(SphereCollider *sphereCollider) override;
  void SetMoveSpeed(float movespeed);
  SphereCollider *GetCollider() override;
  void PickNewTarget();
  void OnCollisionEnter(IEntity *other) override;
  void OnCollisionStay(IEntity *other) override;
  void OnCollisionExit(IEntity *other) override;
  void Die() override;
  void SizeUp() override;
  virtual glm::vec3 GetBodyPosition() override {
    // SpiderNode'un pozisyonunu dondur
    return GetNode()->transform.pos;
  }

private:
  Transform _spiderTransform;
  int _legCount;
  float _hipLocationAsDegree, _moveSpeed, _patrolDirection;
  glm::vec3 _bodySize, _upperLegSize, _middleLegSize, _lowerLegSize,
      _upperLegRotationAngle, _middleLegRotationAngle, _lowerLegRotationAngle;

  Node *_spiderNode, *_sBody;
  std::vector<Node *> _sUpperLegs, _sLowerLegs, _sMiddleLegs, _sMiddleLegsPivot,
      _sUpperLegsPivot, _sLowerLegsPivot;
  std::vector<glm::vec2> _rotationDirections;
  SphereCollider *_spiderCollider;
  glm::vec2 _patrolAreaMin;
  glm::vec2 _patrolAreaMax;
  glm::vec3 _targetPosition;
};
#endif