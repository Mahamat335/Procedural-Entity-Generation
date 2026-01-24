#include "glm/gtc/random.hpp"
#include <Caterpillar.h>
#include <CollisionController.h>
#include <Game.h>
#include <ParticleSystem.h>
#include <Spider.h>
#include <ctime>

Spider::Spider(SpiderEntityData Data)
    : _spiderTransform(Data.EntityTransform), _legCount(Data.LegCount),
      _hipLocationAsDegree(Data.HipLocationAsDegree),
      _moveSpeed(Data.MoveSpeed), _bodySize(Data.BodySize),
      _upperLegSize(Data.UpperLegSize), _middleLegSize(Data.MiddleLegSize),
      _lowerLegSize(Data.LowerLegSize),
      _upperLegRotationAngle(Data.UpperLegRotationAngle),
      _middleLegRotationAngle(Data.MiddleLegRotationAngle),
      _lowerLegRotationAngle(Data.LowerLegRotationAngle),
      _patrolAreaMin(Data.patrolAreaMin), _patrolAreaMax(Data.patrolAreaMax) {
  _spiderNode = new Node(_spiderTransform);
  _sBody = new Node(Transform(glm::vec3(), glm::vec3(), _bodySize), SPHERE,
                    Obsidian);
  _patrolDirection = 1.0f;

  _spiderNode->AddChild(_sBody);

  float verticalAngle = -_hipLocationAsDegree * glm::pi<float>() /
                        180.0f; // Dikey açı (kürenin alt tarafına yakın)
  float horizontalStep =
      glm::two_pi<float>() / _legCount; // her bacak için eşit yatay açı

  for (int i = 0; i < _legCount; i++) {
    float theta =
        i * horizontalStep + horizontalStep / 2; // Her bacak için yatay açı
    float phi = verticalAngle;                   // Sabit dikey açı

    // Kürenin yüzeyindeki noktaları hesapla
    float x = (_bodySize.x * 0.5f) * cos(phi) * sin(theta);
    float y = (_bodySize.y * 0.5f) * sin(phi); // Dikey açı y bileşeni
    float z = (_bodySize.z * 0.5f) * cos(phi) * cos(theta);

    // Pivot noktasını tanımla
    float zUpper = (float)(i - _legCount / 2);
    _sUpperLegsPivot.emplace_back(new Node(
        Transform(glm::vec3(x, y, z),
                  _upperLegRotationAngle +
                      glm::vec3(0.0f, (i >= _legCount / 2 ? 0.0f : 180.0f),
                                zUpper * (15.0f / (float)(_legCount))))));
    _sUpperLegs.emplace_back(
        new Node(Transform(_upperLegSize * glm::vec3(0.0f, 0.5f, 0.0f),
                           glm::vec3(), _upperLegSize),
                 SPHERE, Ruby));
    _sMiddleLegsPivot.emplace_back(new Node(Transform(
        _upperLegSize * glm::vec3(0.0f, 1.0f, 0.0f), _middleLegRotationAngle)));
    _sMiddleLegs.emplace_back(
        new Node(Transform(_middleLegSize * glm::vec3(0.0f, 0.5f, 0.0f),
                           glm::vec3(), _middleLegSize),
                 SPHERE, Ruby));
    _sLowerLegsPivot.emplace_back(new Node(Transform(
        _middleLegSize * glm::vec3(0.0f, 1.0f, 0.0f), _lowerLegRotationAngle)));
    _sLowerLegs.emplace_back(
        new Node(Transform(_lowerLegSize * glm::vec3(0.0f, 0.5f, 0.0f),
                           glm::vec3(), _lowerLegSize),
                 SPHERE, Ruby));

    _spiderNode->AddChild(_sUpperLegsPivot.at(i));
    _sUpperLegsPivot.at(i)->AddChild(_sUpperLegs.at(i));
    _sUpperLegsPivot.at(i)->AddChild(_sMiddleLegsPivot.at(i));
    _sMiddleLegsPivot.at(i)->AddChild(_sLowerLegsPivot.at(i));
    _sMiddleLegsPivot.at(i)->AddChild(_sMiddleLegs.at(i));
    _sLowerLegsPivot.at(i)->AddChild(_sLowerLegs.at(i));
  }
  _rotationDirections =
      std::vector<glm::vec2>(_sUpperLegsPivot.size(), glm::vec2(1.0f, 1.0f));
  _spiderCollider =
      new SphereCollider(this, CollisionController::Instance().ChunkSize);
  PickNewTarget();
  hungerBar.Setup(glm::vec2(1.2f, 0.15f), 1.5f);
}

Spider::~Spider() {
  if (_spiderNode) {
    _spiderNode->RemoveFromParent();
  }

  delete _spiderNode;

  if (_spiderCollider) {
    CollisionController::Instance().RemoveCollider(_spiderCollider);
    delete _spiderCollider;
  }
}

Node *Spider::GetNode() { return _spiderNode; }

void Spider::Move(float deltaTime) {
  float RotationAngleZ = 5.0f; // Bacakların maksimum dönüş açısı
  float RotationAngleY = 5.0f; // Bacakların maksimum dönüş açısı
  float RotationSpeed = 20.0f; // Dönüş hızı

  for (size_t i = 0; i < _sUpperLegsPivot.size(); ++i) {
    Node *pivot = _sUpperLegsPivot[i];
    glm::vec3 currentRotation = pivot->transform.eulerRot;

    // Dönüş yönünü kontrol et ve değiştir
    if (currentRotation.z > _upperLegRotationAngle.z) {
      _rotationDirections[i][0] = -1.0f; // Yön değiştir
    } else if (currentRotation.z <
               _upperLegRotationAngle.z - RotationAngleZ * 2) {
      _rotationDirections[i][0] = 1.0f; // Yön değiştir
    }
    /*
            if (currentRotation.y > RotationAngleY + _upperLegRotationAngle.y +
       (i >= _legCount / 2 ? 0.0f : 180.0f))
            {
                _rotationDirections[i][1] = -1.0f; // Yön değiştir
            }
            else if (currentRotation.y < -RotationAngleY +
       _upperLegRotationAngle.y + (i >= _legCount / 2 ? 0.0f : 180.0f))
            {
                _rotationDirections[i][1] = 1.0f; // Yön değiştir
            }

            // Pivot'u yeni yön ve hız ile döndür
            pivot->Rotate(pivot->transform.eulerRot +
                          glm::vec3(0.0f, _rotationDirections[i][1] *
       RotationSpeed * deltaTime, _rotationDirections[i][0] * RotationSpeed *
       deltaTime)); */

    pivot->Rotate(
        pivot->transform.eulerRot +
        glm::vec3(0.0f, 0.0f,
                  _rotationDirections[i][0] * RotationSpeed * deltaTime));

    // Yürüme kısmı
    float rotation = _spiderTransform.eulerRot.y * glm::pi<float>() / 180.0f;
    glm::vec3 moveDirection = glm::vec3(sin(rotation), 0.0f, cos(rotation));
    _spiderNode->Move(_spiderNode->transform.pos + moveDirection * _moveSpeed *
                                                       _patrolDirection *
                                                       deltaTime);

    float distance = glm::distance(
        glm::vec2(_spiderNode->transform.pos.x, _spiderNode->transform.pos.z),
        glm::vec2(_targetPosition.x, _targetPosition.z));

    if (distance < 1.0f) // 1 birimden küçükse yeni hedef seç
    {
      PickNewTarget();
    }
  }
  _spiderCollider->Update();
  CollisionController::Instance().UpdateCollider(_spiderCollider);
}

void Spider::SetCollider(SphereCollider *sphereCollider) {
  _spiderCollider = sphereCollider;
}

SphereCollider *Spider::GetCollider() { return _spiderCollider; }

void Spider::SetMoveSpeed(float moveSpeed) { _moveSpeed = moveSpeed; }

void Spider::PickNewTarget() {
  float randomX = _patrolAreaMin.x + static_cast<float>(rand()) / RAND_MAX *
                                         (_patrolAreaMax.x - _patrolAreaMin.x);
  float randomZ = _patrolAreaMin.y + static_cast<float>(rand()) / RAND_MAX *
                                         (_patrolAreaMax.y - _patrolAreaMin.y);

  _targetPosition = glm::vec3(randomX, 0.0f, randomZ);

  glm::vec3 direction =
      glm::normalize(_targetPosition - _spiderNode->transform.pos);
  float targetAngle =
      atan2(direction.x, direction.z) * 180.0f / glm::pi<float>();
  _spiderTransform.eulerRot.y = targetAngle;
  _spiderNode->transform.eulerRot.y = targetAngle;
}

void Spider::Die() {
  AnalyticsManager::Instance().LogDeath(EntityType::Spider,
                                        _spiderNode->transform.pos);
  ParticleSystem::Instance().EmitPoof(_spiderNode->transform.pos);
  Game::destroyedEntities.insert(this);
}

void Spider::OnCollisionEnter(IEntity *other) {
  if (Caterpillar *caterpillar = dynamic_cast<Caterpillar *>(other)) {
    caterpillar->Die();
    if (Eat(caterpillar->NutiritionValue)) {
      SizeUp();
    }
  } else if (Spider *otherSpider = dynamic_cast<Spider *>(other)) {
    if (IsReadyToPropose(Hunger, MaxHunger, FeedLevel) &&
        otherSpider->IsReadyToReproduce(otherSpider->Hunger,
                                        otherSpider->MaxHunger,
                                        otherSpider->FeedLevel)) {
      Reproduce(otherSpider);
    }
  }
}

void Spider::OnCollisionStay(IEntity *other) {
  // Sürekli temas halinde yapılacak işlemler
}

void Spider::OnCollisionExit(IEntity *other) {
  // Temas sona erdiğinde yapılacak işlemler
}

void Spider::SizeUp() {
  _bodySize *= glm::vec3(1.2f, 1.2f, 1.2f);
  _sBody->transform.scale = _bodySize;
}

void Spider::Reproduce(IReproducible *otherPartner) {
  Spider *partner = static_cast<Spider *>(otherPartner);
  float mRate = Game::data.spiderGenerationData.MutationRate;

  // Üreme sonrası ebeveynleri bitkin düşür (Sonsuz üreme döngüsünü kırmak için)
  this->Hunger = this->MaxHunger * 0.6f;
  this->FeedLevel = 1;
  partner->Hunger = partner->MaxHunger * 0.6f;
  partner->FeedLevel = 1;

  // Yeni genetik verileri hesapla
  SpiderEntityData babyData;

  // 1. Leg Count (Çift sayısını korumak için önce çift sayısını hesaplayıp 2
  // ile çarpıyoruz)
  int myPairs = this->_legCount / 2;
  int partnerPairs = partner->_legCount / 2;
  babyData.LegCount = CalculateTrait(myPairs, partnerPairs, mRate) * 2;
  if (babyData.LegCount < 2)
    babyData.LegCount = 2;

  // 2. Hareket Hızı
  babyData.MoveSpeed =
      CalculateTrait(this->_moveSpeed, partner->_moveSpeed, mRate);

  // 3. Vücut Boyutu
  babyData.BodySize = glm::vec3(0.8f, 0.4f, 1.0f);

  // 4. Bacak Boyutları
  babyData.UpperLegSize =
      CalculateTrait(this->_upperLegSize, partner->_upperLegSize, mRate);
  babyData.MiddleLegSize =
      CalculateTrait(this->_middleLegSize, partner->_middleLegSize, mRate);
  babyData.LowerLegSize =
      CalculateTrait(this->_lowerLegSize, partner->_lowerLegSize, mRate);

  // 5. Sabit/Kalıtsal Diğer Veriler
  babyData.HipLocationAsDegree =
      this->_hipLocationAsDegree; // Genelde tür bazlı sabit kalır
  babyData.UpperLegRotationAngle = this->_upperLegRotationAngle;
  babyData.MiddleLegRotationAngle = this->_middleLegRotationAngle;
  babyData.LowerLegRotationAngle = this->_lowerLegRotationAngle;
  babyData.patrolAreaMin = this->_patrolAreaMin;
  babyData.patrolAreaMax = this->_patrolAreaMax;

  // 6. Doğum Konumu (Annenin yanında)
  babyData.EntityTransform = this->_spiderNode->transform;
  babyData.EntityTransform.pos += glm::vec3(glm::linearRand(-1.0f, 1.0f), 0.0f,
                                            glm::linearRand(-1.0f, 1.0f));

  // Game sınıfına spawn emri ver
  Game::SpawnNewSpider(babyData);

  // Görsel geri bildirim (Opsiyonel: Doğumda ufak bir poof efekti)
  ParticleSystem::Instance().EmitPoof(babyData.EntityTransform.pos);
}