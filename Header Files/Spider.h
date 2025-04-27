#ifndef SPIDER_H
#define SPIDER_H

#include <glm/glm.hpp>
#include <Node.h>
#include <SpiderEntityData.h>
#include <SphereCollider.h>

class Spider
{
public:
    Spider(SpiderEntityData Data);
    Node *GetNode();
    void Move(float deltaTime);
    void SetCollider(SphereCollider *sphereCollider);
    void SetMoveSpeed(float movespeed);
    SphereCollider *GetCollider();
    void PickNewTarget();

private:
    Transform _spiderTransform;
    int _legCount;
    float _hipLocationAsDegree, _moveSpeed, _patrolDirection;
    glm::vec3 _bodySize, _upperLegSize, _middleLegSize, _lowerLegSize, _upperLegRotationAngle, _middleLegRotationAngle, _lowerLegRotationAngle;

    Node *_spiderNode, *_sBody;
    std::vector<Node *> _sUpperLegs, _sLowerLegs, _sMiddleLegs, _sMiddleLegsPivot, _sUpperLegsPivot, _sLowerLegsPivot;
    std::vector<glm::vec2> _rotationDirections;
    SphereCollider *_spiderCollider;
    glm::vec2 _patrolAreaMin;
    glm::vec2 _patrolAreaMax;
    glm::vec3 _targetPosition;
};
#endif