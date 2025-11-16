#ifndef NODE_H
#define NODE_H

#include <Material.h>
#include <RenderingLayer.h>
#include <ShapeRenderer.h>
#include <ShapeType.h>
#include <Transform.h>
#include <algorithm>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <vector>

class Node {

public:
  Transform transform;
  // std::vector<std::unique_ptr<Node>> children;
  std::vector<Node *> children;
  Transform *parent = nullptr;
  Node *parentPtr = nullptr;
  bool isEnable = true;
  bool isStatic = false;
  ShapeType Shape;
  static std::vector<Node *> CubeEntities;
  static std::vector<Node *> SphereEntities;
  Material material;
  RenderingLayer Layer;

  Node(Transform t = Transform(), ShapeType shape = POINT,
       Material material = Emerald, RenderingLayer renderingLayer = Default)
      : transform(t), Shape(shape), material(material), Layer(renderingLayer) {}

  ~Node() { RemoveAllChildren(); }
  void RemoveFromParent() {
    if (parentPtr) {
      auto &parentChildren = parentPtr->children;
      auto it = std::find(parentChildren.begin(), parentChildren.end(), this);
      if (it != parentChildren.end()) {
        parentChildren.erase(it);
      }
      parentPtr = nullptr;
    }
  }

  void RemoveAllChildren() {
    while (!children.empty()) {
      delete children.back();
      children.pop_back();
    }
  }

  void AddChild(Node *child) {
    child->RemoveFromParent();

    child->parent = &this->transform;
    child->parentPtr = this;
    child->UpdateTransform();
    children.emplace_back(child);
  }

  void UpdateTransform() {
    transform.modelMatrix = glm::translate(parent->modelMatrix, transform.pos);
    transform.modelMatrix =
        glm::rotate(transform.modelMatrix, glm::radians(transform.eulerRot.x),
                    glm::vec3(1.0f, 0.0f, 0.0f));
    transform.modelMatrix =
        glm::rotate(transform.modelMatrix, glm::radians(transform.eulerRot.y),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    transform.modelMatrix =
        glm::rotate(transform.modelMatrix, glm::radians(transform.eulerRot.z),
                    glm::vec3(0.0f, 0.0f, 1.0f));
    transform.modelMatrix = glm::scale(transform.modelMatrix, transform.scale);
    for (auto &child : children) {
      child->UpdateTransform();
    }
  }

  void DrawMesh(unsigned int modelLoc,
                Shader shaderProgram =
                    *(ShaderManager::Instance().defaultShaderProgram)) {
    ShapeRenderer::Instance().Draw(Shape, Layer, modelLoc,
                                   glm::value_ptr(transform.modelMatrix),
                                   material, shaderProgram);
  }

  void Move(glm::vec3 newPosition) {
    if (isStatic) {
      std::cerr << "You Can't move a static object!" << std::endl;
    } else {

      transform.pos = newPosition;
      UpdateTransform();
    }
  }

  void Rotate(glm::vec3 newRotation) {
    if (isStatic) {
      std::cerr << "You Can't rotate a static object!" << std::endl;
    } else {

      transform.eulerRot = newRotation;
      UpdateTransform();
    }
  }

  void Scale(glm::vec3 newScale) {
    if (isStatic) {
      std::cerr << "You Can't scale a static object!" << std::endl;
    } else {

      transform.scale = newScale;
      UpdateTransform();
    }
  }

  void SetAsStatic() { isStatic = true; }

  void SetAsDynamic() { isStatic = false; }
};
#endif