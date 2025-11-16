#include <CollisionController.h>
#include <Game.h>
#include <Producer.h>
#include <ctime>

Producer::Producer(ProducerEntityData Data)
    : _producerTransform(Data.EntityTransform), _bodySize(Data.BodySize) {
  _producerNode = new Node(_producerTransform);
  _pBody =
      new Node(Transform(glm::vec3(), glm::vec3(), _bodySize), SPHERE, Ruby);

  _producerNode->AddChild(_pBody);

  _producerCollider =
      new SphereCollider(this, CollisionController::Instance().ChunkSize);
}

Producer::~Producer() {
  if (_producerNode) {
    _producerNode->RemoveFromParent();
  }

  delete _producerNode;

  if (_producerCollider) {
    CollisionController::Instance().RemoveCollider(_producerCollider);
    delete _producerCollider;
  }
}

Node *Producer::GetNode() { return _producerNode; }

void Producer::Move(float deltaTime) {}

void Producer::SetCollider(SphereCollider *sphereCollider) {
  _producerCollider = sphereCollider;
}

SphereCollider *Producer::GetCollider() { return _producerCollider; }

void Producer::Die() { Game::destroyedEntities.insert(this); }

void Producer::OnCollisionEnter(IEntity *other) {}

void Producer::OnCollisionStay(IEntity *other) {}

void Producer::OnCollisionExit(IEntity *other) {}