#include <CubeCollider.h>

CubeCollider::CubeCollider(IEntity *entity, glm::vec3 size) : Collider(entity), size(size)
{
    halfSize = size * 0.5f;
}

bool CubeCollider::CheckForCollision(CubeCollider *collision)
{
    bool collisionX = Center.x + halfSize.x >= collision->Center.x - collision->halfSize.x && collision->Center.x + collision->halfSize.x >= Center.x - halfSize.x;
    bool collisionY = Center.y + halfSize.y >= collision->Center.y - collision->halfSize.y && collision->Center.y + collision->halfSize.y >= Center.y - halfSize.y;
    bool collisionZ = Center.z + halfSize.z >= collision->Center.z - collision->halfSize.z && collision->Center.z + collision->halfSize.z >= Center.z - halfSize.z;
    // std::cout << center.x << " " << halfSize.x << " " << collision->center.x << " " << collision->halfSize.x << std::endl;
    return collisionX && collisionY && collisionZ;
}
bool CubeCollider::CheckForCollision(Collider *collision)
{
    return false;
}
/* bool CubeCollider::CheckForCollision(SphereCollider *collision)
{
    return glm::distance(glm::clamp(collision->center - center, -halfSize, halfSize) + center, collision->center) < collision->radius;
} */