#include <Collider.h>
#include <CollisionController.h>

Collider::Collider(Node *parent) : Parent(parent), Center(parent->transform.pos)
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
    // std::cout << "Collision Enter " << ID << "-" << other->ID << std::endl;
}

void Collider::OnCollisionStay(Collider *other)
{
}

void Collider::OnCollisionExit(Collider *other)
{
    Collisions.erase(other);
    // td::cout << "Collision Exit " << ID << "-" << other->ID << std::endl;
}