#include <SphereCollider.h>
#include <CollisionController.h>

SphereCollider::SphereCollider(Node *parent, float radius) : Collider(parent), Radius(radius)
{
    CollisionController::Instance().AddCollider(this);
}

bool SphereCollider::CheckForCollision(SphereCollider *collision)
{
    return glm::distance(Center, collision->Center) < Radius + collision->Radius;
}