#ifndef SPIDER_H
#define SPIDER_H

#include <glm/glm.hpp>
#include <Entity.h>

class Spider
{
public:
    Spider(Transform SpiderTransform, int LegCount, float _hipLocationAsDegree, glm::vec3 BodySize, glm::vec3 UpperLegSize, glm::vec3 MiddleLegSize, glm::vec3 LowerLegSize, glm::vec3 UpperLegRotationAngle, glm::vec3 MiddleLegRotationAngle, glm::vec3 LowerLegRotationAngle, float MoveSpeed);
    Entity *GetEntity();
    void Move(float deltaTime);
    void Patrol();

private:
    Transform _spiderTransform;
    glm::vec3 _bodySize, _upperLegSize, _middleLegSize, _lowerLegSize, _upperLegRotationAngle, _middleLegRotationAngle, _lowerLegRotationAngle, _moveSpeed;
    int _legCount;
    float _hipLocationAsDegree;
    Entity *_spiderEntity, *_sBody;
    std::vector<Entity *> _sUpperLegs, _sLowerLegs, _sMiddleLegs, _sMiddleLegsPivot, _sUpperLegsPivot, _sLowerLegsPivot;
    std::vector<glm::vec2> _rotationDirections;
};
#endif