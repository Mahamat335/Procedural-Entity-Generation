#include <SphereCollider.h>
#include <CollisionController.h>
#include <RenderingLayer.h>

SphereCollider::SphereCollider(Node *parent, float radius) : Collider(parent), Radius(radius)
{
    ColliderNode = new Node(Transform(glm::vec3(), glm::vec3(), glm::vec3(radius)), SPHERE, Emerald, Polygon);
    parent->AddChild(ColliderNode);
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