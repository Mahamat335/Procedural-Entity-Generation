#ifndef ENTITY_DATA_H
#define ENTITY_DATA_H

#include <Transform.h>
#include <glm/glm.hpp>

struct SpiderEntityData
{
public:
    Transform EntityTransform;
    int LegCount;
    float HipLocationAsDegree, MoveSpeed;
    glm::vec3 BodySize, UpperLegSize, MiddleLegSize, LowerLegSize, UpperLegRotationAngle, MiddleLegRotationAngle, LowerLegRotationAngle;
};

#endif