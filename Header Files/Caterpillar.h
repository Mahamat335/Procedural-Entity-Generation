#ifndef CATERPILLAR_H
#define CATERPILLAR_H

#include <glm/glm.hpp>
#include <Node.h>
#include <EntityData.h>
#include <SphereCollider.h>
#include <IEntity.h>

class Caterpillar : public IEntity
{
public:
    Caterpillar(CaterpillarEntityData Data);
    Node *GetNode();
    void Move(float deltaTime);
    void SetCollider(SphereCollider *sphereCollider);
    void SetMoveSpeed(float movespeed);
    SphereCollider *GetCollider();
    void PickNewTarget();

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
};
#endif