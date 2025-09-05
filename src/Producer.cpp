#include <Producer.h>
#include <ctime>
#include <CollisionController.h>
#include <Game.h>

Producer::Producer(ProducerEntityData Data) : _producerTransform(Data.EntityTransform), _bodySize(Data.BodySize)
{
    _producerNode = new Node(_producerTransform);
    _pBody = new Node(Transform(glm::vec3(), glm::vec3(), _bodySize), SPHERE, Ruby);

    _producerNode->AddChild(_pBody);

    _producerCollider = new SphereCollider(this, CollisionController::Instance().ChunkSize);
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

void Producer::Die()
{
    Game::destroyedEntities.insert(this);
}

void Producer::Destroy()
{
    Game::producers.erase(std::remove(Game::producers.begin(), Game::producers.end(), this), Game::producers.end());
    _producerNode->Destroy();
    delete _producerCollider;
}

void Producer::OnCollisionEnter(IEntity *other)
{
}

void Producer::OnCollisionStay(IEntity *other)
{
}

void Producer::OnCollisionExit(IEntity *other)
{
}