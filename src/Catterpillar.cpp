#include <Caterpillar.h>
#include <ctime>
#include <CollisionController.h>

Caterpillar::Caterpillar(CaterpillarEntityData Data) : _caterpillarTransform(Data.EntityTransform), _segmentCount(Data.SegmentCount), _moveSpeed(Data.MoveSpeed), _bodySize(Data.BodySize), _patrolAreaMin(Data.patrolAreaMin), _patrolAreaMax(Data.patrolAreaMax)
{
    _caterpillarNode = new Node(_caterpillarTransform);
    _cBody = new Node(Transform(glm::vec3(), glm::vec3(), _bodySize), SPHERE, Brass);

    _caterpillarNode->AddChild(_cBody);

    float verticalAngle = -_hipLocationAsDegree * glm::pi<float>() / 180.0f; // Dikey açı (kürenin alt tarafına yakın)

    for (int i = 0; i < _segmentCount; i++)
    {
        Node *currentSegment = new Node(Transform(glm::vec3(0.0f, 0.0f, -(i + 1) * _bodySize.z / 3.0f), glm::vec3(), _bodySize * 0.85f), SPHERE, Obsidian);
        _caterpillarNode->AddChild(currentSegment);
    }

    _caterpillarCollider = new SphereCollider(_caterpillarNode, CollisionController::Instance().ChunkSize);
    PickNewTarget();
}

Node *Caterpillar::GetNode()
{
    return _caterpillarNode;
}

void Caterpillar::Move(float deltaTime)
{
    float rotation = _caterpillarTransform.eulerRot.y * glm::pi<float>() / 180.0f;
    glm::vec3 moveDirection = glm::vec3(sin(rotation), 0.0f, cos(rotation));
    _caterpillarNode->Move(_caterpillarNode->transform.pos + moveDirection * _moveSpeed * deltaTime);

    float distance = glm::distance(glm::vec2(_caterpillarNode->transform.pos.x, _caterpillarNode->transform.pos.z),
                                   glm::vec2(_targetPosition.x, _targetPosition.z));

    if (distance < 1.0f) // 1 birimden küçükse yeni hedef seç
    {
        PickNewTarget();
    }
    _caterpillarCollider->Update();
    CollisionController::Instance().UpdateCollider(_caterpillarCollider);
}

/* void Caterpillar::Move(float deltaTime)
{

    // Head pozisyonunu referans al
    float time = static_cast<float>(clock()) / CLOCKS_PER_SEC;
    // === 2. BODY FOLLOW LOGIC ===
    float amplitude = 0.1f;                    // Y dalgalanma miktarı
    float frequency = 4.0f;                    // Dalgalanma hızı
    float followDistance = _bodySize.z / 3.0f; // Segmentler arası mesafe
    // i=1'den başlıyoruz çünkü 0. segment head
    for (size_t i = _segmentCount; i > 0; i--)
    {
        Node *segment = _caterpillarNode->children[i];

        // Önceki segment pozisyonu ile aradaki mesafe
        glm::vec3 dir = _caterpillarNode->children[i - 1]->transform.pos - segment->transform.pos;
        float dist = glm::length(dir);

        if (dist > 0.0001f)
        {
            dir = glm::normalize(dir);
            // Segment, followDistance mesafesinde konumlanır
            segment->transform.pos.y = amplitude * sin(time * frequency + i * 0.5f);
            segment->Move(segment->transform.pos + glm::normalize(dir) * _moveSpeed * deltaTime);
        }

        // Sonraki segment için prevPos güncelle
    }
    // === 1. HEAD MOVEMENT ===

    float rotation = _caterpillarTransform.eulerRot.y * glm::pi<float>() / 180.0f;
    glm::vec3 moveDirection = glm::vec3(sin(rotation), 0.0f, cos(rotation));
    _caterpillarNode->Move(_caterpillarNode->transform.pos + moveDirection * _moveSpeed * deltaTime);

    float distance = glm::distance(glm::vec2(_caterpillarNode->transform.pos.x, _caterpillarNode->transform.pos.z),
                                   glm::vec2(_targetPosition.x, _targetPosition.z));

    if (distance < 1.0f)
    {
        PickNewTarget();
    }

    // === 3. COLLISION UPDATE ===
    _caterpillarCollider->Update();
    CollisionController::Instance().UpdateCollider(_caterpillarCollider);
} */

void Caterpillar::SetCollider(SphereCollider *sphereCollider)
{
    _caterpillarCollider = sphereCollider;
}

SphereCollider *Caterpillar::GetCollider()
{
    return _caterpillarCollider;
}

void Caterpillar::SetMoveSpeed(float moveSpeed)
{
    _moveSpeed = moveSpeed;
}

void Caterpillar::PickNewTarget()
{
    float randomX = _patrolAreaMin.x + static_cast<float>(rand()) / RAND_MAX * (_patrolAreaMax.x - _patrolAreaMin.x);
    float randomZ = _patrolAreaMin.y + static_cast<float>(rand()) / RAND_MAX * (_patrolAreaMax.y - _patrolAreaMin.y);

    _targetPosition = glm::vec3(randomX, 0.0f, randomZ);

    glm::vec3 direction = glm::normalize(_targetPosition - _caterpillarNode->transform.pos);
    float targetAngle = atan2(direction.x, direction.z) * 180.0f / glm::pi<float>();
    _caterpillarTransform.eulerRot.y = targetAngle;
    _caterpillarNode->transform.eulerRot.y = targetAngle;
}