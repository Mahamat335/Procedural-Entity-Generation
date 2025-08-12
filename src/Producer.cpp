#include <Producer.h>
#include <ctime>
#include <CollisionController.h>

Producer::Producer(ProducerEntityData Data) : _producerTransform(Data.EntityTransform), _bodySize(Data.BodySize)
{
    _producerNode = new Node(_producerTransform);
    _pBody = new Node(Transform(glm::vec3(), glm::vec3(), _bodySize), SPHERE, Ruby);

    _producerNode->AddChild(_pBody);

    _producerCollider = new SphereCollider(_producerNode, CollisionController::Instance().ChunkSize);
}

Node *Producer::GetNode()
{
    return _producerNode;
}

void Producer::Move(float deltaTime)
{
}

void Producer::SetCollider(SphereCollider *sphereCollider)
{
    _producerCollider = sphereCollider;
}

SphereCollider *Producer::GetCollider()
{
    return _producerCollider;
}