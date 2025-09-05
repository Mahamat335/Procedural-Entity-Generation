#ifndef COLLIDER_H
#define COLLIDER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stack>
#include <unordered_set>
#include <Node.h>
#include <Transform.h>
#include <glm/glm.hpp>
class IEntity;

static int ColliderCount = 0;

class Collider
{
public:
    glm::vec3 Center;
    std::unordered_set<Collider *> Collisions;
    IEntity *Entity;
    int ID;
    Collider(IEntity *entity);
    ~Collider();
    // virtual bool CheckForCollision(Collider *collision) = 0;
    //  virtual bool CheckForCollision(SphereCollider *collision) = 0;
    void OnCollisionEnter(Collider *other);
    void OnCollisionStay(Collider *other);
    void OnCollisionExit(Collider *other);
};

#endif
