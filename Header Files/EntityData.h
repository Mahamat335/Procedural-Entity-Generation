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
    glm::vec2 patrolAreaMin, patrolAreaMax; // Patrol area min and max
};

struct CaterpillarEntityData
{
public:
    Transform EntityTransform;
    int SegmentCount;
    float MoveSpeed;
    glm::vec3 BodySize;
    glm::vec2 patrolAreaMin, patrolAreaMax; // Patrol area min and max
};

struct ProducerEntityData
{
public:
    Transform EntityTransform;
    glm::vec3 BodySize;
};
#endif