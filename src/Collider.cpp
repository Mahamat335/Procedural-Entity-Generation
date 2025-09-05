#include <Collider.h>
#include <CollisionController.h>
#include <IEntity.h>

Collider::Collider(IEntity *entity) : Entity(entity), Center(entity->GetNode()->transform.pos)
{
    ID = ColliderCount++;
}

Collider::~Collider()
{
    // CollisionController::Instance().RemoveCollider(this);
}

void Collider::OnCollisionEnter(Collider *other)
{
    Collisions.insert(other);
    Entity->OnCollisionEnter(other->Entity);
    // std::cout << "Collision Enter " << ID << "-" << other->ID << std::endl;
}

void Collider::OnCollisionStay(Collider *other)
{
    Entity->OnCollisionStay(other->Entity);
    // std::cout << "Collision Stay " << ID << "-" << other->ID << std::endl;
}

void Collider::OnCollisionExit(Collider *other)
{
    Collisions.erase(other);
    Entity->OnCollisionExit(other->Entity);
    // td::cout << "Collision Exit " << ID << "-" << other->ID << std::endl;
}