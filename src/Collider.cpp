#include <Collider.h>

Collider::Collider(Node *parent) : Parent(parent), Center(parent->transform.pos)
{
}

void Collider::OnCollisionEnter(Collider *collision)
{
    Collisions.emplace_back(collision);
}

void Collider::OnCollisionExit(Collider *collision)
{
    auto it = std::find(Collisions.begin(), Collisions.end(), collision);
    if (it != Collisions.end())
    {
        Collisions.erase(it);
    }
}