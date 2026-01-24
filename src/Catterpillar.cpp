#include "glm/gtc/random.hpp"
#include <Caterpillar.h>
#include <CollisionController.h>
#include <Game.h>
#include <ParticleSystem.h>
#include <Producer.h>

Caterpillar::Caterpillar(CaterpillarEntityData Data)
    : _caterpillarTransform(Data.EntityTransform),
      _segmentCount(Data.SegmentCount), _moveSpeed(Data.MoveSpeed),
      _bodySize(Data.BodySize), _patrolAreaMin(Data.patrolAreaMin),
      _patrolAreaMax(Data.patrolAreaMax) {
  _caterpillarNode = new Node(_caterpillarTransform);
  _cBody =
      new Node(Transform(glm::vec3(), glm::vec3(), _bodySize), SPHERE, Brass);

  _caterpillarNode->AddChild(_cBody);

  float verticalAngle = -_hipLocationAsDegree * glm::pi<float>() /
                        180.0f; // Dikey açı (kürenin alt tarafına yakın)

  for (int i = 0; i < _segmentCount; i++) {
    Node *currentSegment =
        new Node(Transform(glm::vec3(0.0f, 0.0f, -(i + 1) * _bodySize.z / 3.0f),
                           glm::vec3(), _bodySize * 0.85f),
                 SPHERE, Obsidian);
    _caterpillarNode->AddChild(currentSegment);
  }

  _caterpillarCollider =
      new SphereCollider(this, CollisionController::Instance().ChunkSize);
  PickNewTarget();
  hungerBar.Setup(glm::vec2(0.8f, 0.1f), 0.8f);
}

Caterpillar::~Caterpillar() {
  if (_caterpillarNode) {
    _caterpillarNode->RemoveFromParent();
  }

  delete _caterpillarNode;

  if (_caterpillarCollider) {
    CollisionController::Instance().RemoveCollider(_caterpillarCollider);
    delete _caterpillarCollider;
  }
}

Node *Caterpillar::GetNode() { return _caterpillarNode; }

void Caterpillar::Move(float deltaTime) {
  float rotation = _caterpillarTransform.eulerRot.y * glm::pi<float>() / 180.0f;
  glm::vec3 moveDirection = glm::vec3(sin(rotation), 0.0f, cos(rotation));
  _caterpillarNode->Move(_caterpillarNode->transform.pos +
                         moveDirection * _moveSpeed * deltaTime);

  float distance =
      glm::distance(glm::vec2(_caterpillarNode->transform.pos.x,
                              _caterpillarNode->transform.pos.z),
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
        glm::vec3 dir = _caterpillarNode->children[i - 1]->transform.pos -
segment->transform.pos; float dist = glm::length(dir);

        if (dist > 0.0001f)
        {
            dir = glm::normalize(dir);
            // Segment, followDistance mesafesinde konumlanır
            segment->transform.pos.y = amplitude * sin(time * frequency + i *
0.5f); segment->Move(segment->transform.pos + g bool polygonMode =
false;lm::normalize(dir) * _moveSpeed * deltaTime);
        }

        // Sonraki segment için prevPos güncelle
    }
    // === 1. HEAD MOVEMENT ===

    float rotation = _caterpillarTransform.eulerRot.y * glm::pi<float>() /
180.0f; glm::vec3 moveDirection = glm::vec3(sin(rotation), 0.0f, cos(rotation));
    _caterpillarNode->Move(_caterpillarNode->transform.pos + moveDirection *
_moveSpeed * deltaTime);

    float distance = glm::distance(glm::vec2(_caterpillarNode->transform.pos.x,
_caterpillarNode->transform.pos.z), glm::vec2(_targetPosition.x,
_targetPosition.z));

    if (distance < 1.0f)
    {
        PickNewTarget();
    }

    // === 3. COLLISION UPDATE ===
    _caterpillarCollider->Update();
    CollisionController::Instance().UpdateCollider(_caterpillarCollider);
} */

void Caterpillar::SetCollider(SphereCollider *sphereCollider) {
  _caterpillarCollider = sphereCollider;
}

SphereCollider *Caterpillar::GetCollider() { return _caterpillarCollider; }

void Caterpillar::SetMoveSpeed(float moveSpeed) { _moveSpeed = moveSpeed; }

void Caterpillar::PickNewTarget() {
  float randomX = _patrolAreaMin.x + static_cast<float>(rand()) / RAND_MAX *
                                         (_patrolAreaMax.x - _patrolAreaMin.x);
  float randomZ = _patrolAreaMin.y + static_cast<float>(rand()) / RAND_MAX *
                                         (_patrolAreaMax.y - _patrolAreaMin.y);

  _targetPosition = glm::vec3(randomX, 0.0f, randomZ);

  glm::vec3 direction =
      glm::normalize(_targetPosition - _caterpillarNode->transform.pos);
  float targetAngle =
      atan2(direction.x, direction.z) * 180.0f / glm::pi<float>();
  _caterpillarTransform.eulerRot.y = targetAngle;
  _caterpillarNode->transform.eulerRot.y = targetAngle;
}

void Caterpillar::Die() {
  AnalyticsManager::Instance().LogDeath(EntityType::Caterpillar,
                                        _caterpillarNode->transform.pos);
  ParticleSystem::Instance().EmitPoof(_caterpillarNode->transform.pos);
  Game::destroyedEntities.insert(this);
}

void Caterpillar::OnCollisionEnter(IEntity *other) {
  if (Producer *producer = dynamic_cast<Producer *>(other)) {
    producer->Die();
    if (Eat(producer->NutiritionValue)) {
      SizeUp();
    }
  } else if (Caterpillar *otherCaterpillar =
                 dynamic_cast<Caterpillar *>(other)) {
    if (IsReadyToPropose(Hunger, MaxHunger, FeedLevel) &&
        otherCaterpillar->IsReadyToReproduce(otherCaterpillar->Hunger,
                                             otherCaterpillar->MaxHunger,
                                             otherCaterpillar->FeedLevel)) {
      Reproduce(otherCaterpillar);
    }
  }
}

void Caterpillar::OnCollisionStay(IEntity *other) {
  // Caterpillar'ın başka bir nesneyle çarpışma devam ettiğinde yapılacak
  // işlemler
}

void Caterpillar::OnCollisionExit(IEntity *other) {
  // Caterpillar'ın başka bir nesneyle çarpışma sona erdiğinde yapılacak
  // işlemler
}

void Caterpillar::SizeUp() {
  _segmentCount++;
  Node *currentSegment = new Node(
      Transform(glm::vec3(0.0f, 0.0f, -(_segmentCount)*_bodySize.z / 3.0f),
                glm::vec3(), _bodySize * 0.85f),
      SPHERE, Obsidian);
  _caterpillarNode->AddChild(currentSegment);
}

void Caterpillar::Reproduce(IReproducible *otherPartner) {
  Caterpillar *partner = static_cast<Caterpillar *>(otherPartner);
  float mRate = Game::data.caterpillarGenerationData.MutationRate;

  // 1. Ebeveynlerin durumunu güncelle (Enerji harcat ve seviye düşür)
  // Üreme sonrası hemen tekrar ürememeleri için Hunger değerini yükseltiyoruz
  this->Hunger = this->MaxHunger * 0.7f;
  this->FeedLevel = 1;
  partner->Hunger = partner->MaxHunger * 0.7f;
  partner->FeedLevel = 1;

  // 2. Bebek için veri yapısını hazırla
  CaterpillarEntityData babyData;

  // 3. Genetik Miras + Mutasyon Hesaplamaları

  // Segment Sayısı (Senin kodunda LegPairCount/SegmentCount olarak geçiyor)
  // Not: Isırgan bacak yerine segment (boğum) sayısı olarak CalculateTrait
  // kullanıyoruz
  babyData.SegmentCount = 2; // Minimum boğum sayısı kontrolü

  // Hareket Hızı
  babyData.MoveSpeed =
      CalculateTrait(this->_moveSpeed, partner->_moveSpeed, mRate);

  // Vücut Boyutu (glm::vec3)
  babyData.BodySize =
      CalculateTrait(this->_bodySize, partner->_bodySize, mRate);

  // Alan Sınırları (Ebeveynlerin devriye gezdiği alanı miras alır)
  babyData.patrolAreaMin = this->_patrolAreaMin;
  babyData.patrolAreaMax = this->_patrolAreaMax;

  // 4. Doğum Pozisyonu
  // Annenin (this) pozisyonunda, hafif bir rastgele sapma ile
  babyData.EntityTransform = this->_caterpillarNode->transform;
  babyData.EntityTransform.pos += glm::vec3(glm::linearRand(-0.5f, 0.5f), 0.0f,
                                            glm::linearRand(-0.5f, 0.5f));

  // 5. Simülasyona ekle
  Game::SpawnNewCaterpillar(babyData);

  // Görsel geri bildirim
  ParticleSystem::Instance().EmitPoof(babyData.EntityTransform.pos);

  std::cout << "[Evolution] A new caterpillar hatched with "
            << babyData.SegmentCount << " segments." << std::endl;
}