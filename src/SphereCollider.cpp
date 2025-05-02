#include <SphereCollider.h>
#include <CollisionController.h>

SphereCollider::SphereCollider(Node *parent, float radius) : Collider(parent), Radius(radius)
{
    CollisionController::Instance().AddCollider(this);
}

SphereCollider::~SphereCollider()
{
    CollisionController::Instance().RemoveCollider(this);
}

bool SphereCollider::CheckForCollision(SphereCollider *collision)
{
    return glm::distance(Center, collision->Center) < Radius + collision->Radius;
}

void SphereCollider::Update()
{
    Center = Parent->transform.pos;
}