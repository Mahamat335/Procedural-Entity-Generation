#ifndef COLLIDER_H
#define COLLIDER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stack>
#include <vector>
#include <Node.h>
#include <Transform.h>
#include <glm/glm.hpp>

class Collider
{

public:
    glm::vec3 Center;
    std::vector<Collider *> Collisions;
    Node *Parent;
    Collider(Node *parent);
    // virtual bool CheckForCollision(Collider *collision) = 0;
    //  virtual bool CheckForCollision(SphereCollider *collision) = 0;
    void OnCollisionEnter(Collider *collision);
    void OnCollisionExit(Collider *collision);
};

#endif
