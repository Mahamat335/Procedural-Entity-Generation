#ifndef SPHERE_COLLIDER_H
#define SPHERE_COLLIDER_H

#include <Collider.h>

class SphereCollider : public Collider
{

public:
    float Radius;
    SphereCollider(Node *parent, float radius);
    bool CheckForCollision(SphereCollider *collision);
    void Update();
};

#endif
