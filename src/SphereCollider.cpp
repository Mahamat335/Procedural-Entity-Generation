#include <SphereCollider.h>
#include <CollisionController.h>
#include <RenderingLayer.h>
#include <IEntity.h>

SphereCollider::SphereCollider(IEntity *entity, float radius) : Collider(entity), Radius(radius)
{
    ColliderNode = new Node(Transform(glm::vec3(), glm::vec3(), glm::vec3(radius)), SPHERE, Emerald, Polygon);
    Entity->GetNode()->AddChild(ColliderNode);
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
    Center = Entity->GetNode()->transform.pos;
}