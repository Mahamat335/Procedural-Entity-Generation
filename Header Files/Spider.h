#ifndef SPIDER_H
#define SPIDER_H

#include <glm/glm.hpp>
#include <Node.h>
#include <SpiderEntityData.h>

class Spider
{
public:
    Spider(SpiderEntityData Data);
    Node *GetNode();
    void Move(float deltaTime);
    void Patrol();

private:
    Transform _spiderTransform;
    int _legCount;
    float _hipLocationAsDegree, _moveSpeed;
    glm::vec3 _bodySize, _upperLegSize, _middleLegSize, _lowerLegSize, _upperLegRotationAngle, _middleLegRotationAngle, _lowerLegRotationAngle;

    Node *_spiderNode, *_sBody;
    std::vector<Node *> _sUpperLegs, _sLowerLegs, _sMiddleLegs, _sMiddleLegsPivot, _sUpperLegsPivot, _sLowerLegsPivot;
    std::vector<glm::vec2> _rotationDirections;
};
#endif