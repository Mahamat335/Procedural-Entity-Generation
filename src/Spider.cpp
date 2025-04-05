#include <Spider.h>
#include <ctime>
#include <CollisionController.h>

Spider::Spider(SpiderEntityData Data) : _spiderTransform(Data.EntityTransform), _legCount(Data.LegCount), _hipLocationAsDegree(Data.HipLocationAsDegree), _moveSpeed(Data.MoveSpeed), _bodySize(Data.BodySize), _upperLegSize(Data.UpperLegSize), _middleLegSize(Data.MiddleLegSize), _lowerLegSize(Data.LowerLegSize), _upperLegRotationAngle(Data.UpperLegRotationAngle), _middleLegRotationAngle(Data.MiddleLegRotationAngle), _lowerLegRotationAngle(Data.LowerLegRotationAngle)
{
    _spiderNode = new Node(_spiderTransform);
    _sBody = new Node(Transform(glm::vec3(), glm::vec3(), _bodySize), SPHERE, Obsidian);

    _spiderNode->AddChild(_sBody);

    float verticalAngle = -_hipLocationAsDegree * glm::pi<float>() / 180.0f; // Dikey açı (kürenin alt tarafına yakın)
    float horizontalStep = glm::two_pi<float>() / _legCount;                 // her bacak için eşit yatay açı

    for (int i = 0; i < _legCount; i++)
    {
        float theta = i * horizontalStep + horizontalStep / 2; // Her bacak için yatay açı
        float phi = verticalAngle;                             // Sabit dikey açı

        // Kürenin yüzeyindeki noktaları hesapla
        float x = (_bodySize.x * 0.5f) * cos(phi) * sin(theta);
        float y = (_bodySize.y * 0.5f) * sin(phi); // Dikey açı y bileşeni
        float z = (_bodySize.z * 0.5f) * cos(phi) * cos(theta);

        // Pivot noktasını tanımla
        float zUpper = (float)(i - _legCount / 2);
        _sUpperLegsPivot.emplace_back(new Node(Transform(glm::vec3(x, y, z), _upperLegRotationAngle + glm::vec3(0.0f, (i >= _legCount / 2 ? 0.0f : 180.0f), zUpper * (15.0f / (float)(_legCount))))));
        _sUpperLegs.emplace_back(new Node(Transform(_upperLegSize * glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(), _upperLegSize), SPHERE, Ruby));
        _sMiddleLegsPivot.emplace_back(new Node(Transform(_upperLegSize * glm::vec3(0.0f, 1.0f, 0.0f), _middleLegRotationAngle)));
        _sMiddleLegs.emplace_back(new Node(Transform(_middleLegSize * glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(), _middleLegSize), SPHERE, Ruby));
        _sLowerLegsPivot.emplace_back(new Node(Transform(_middleLegSize * glm::vec3(0.0f, 1.0f, 0.0f), _lowerLegRotationAngle)));
        _sLowerLegs.emplace_back(new Node(Transform(_lowerLegSize * glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(), _lowerLegSize), SPHERE, Ruby));

        _spiderNode->AddChild(_sUpperLegsPivot.at(i));
        _sUpperLegsPivot.at(i)->AddChild(_sUpperLegs.at(i));
        _sUpperLegsPivot.at(i)->AddChild(_sMiddleLegsPivot.at(i));
        _sMiddleLegsPivot.at(i)->AddChild(_sLowerLegsPivot.at(i));
        _sMiddleLegsPivot.at(i)->AddChild(_sMiddleLegs.at(i));
        _sLowerLegsPivot.at(i)->AddChild(_sLowerLegs.at(i));
    }
    _rotationDirections = std::vector<glm::vec2>(_sUpperLegsPivot.size(), glm::vec2(1.0f, 1.0f));
    _spiderCollider = new SphereCollider(_spiderNode, _bodySize.z);
}

Node *Spider::GetNode()
{
    return _spiderNode;
}

void Spider::Move(float deltaTime)
{
    float RotationAngleZ = 5.0f; // Bacakların maksimum dönüş açısı
    float RotationAngleY = 5.0f; // Bacakların maksimum dönüş açısı
    float RotationSpeed = 20.0f; // Dönüş hızı

    for (size_t i = 0; i < _sUpperLegsPivot.size(); ++i)
    {
        Node *pivot = _sUpperLegsPivot[i];
        glm::vec3 currentRotation = pivot->transform.eulerRot;

        // Dönüş yönünü kontrol et ve değiştir
        if (currentRotation.z > _upperLegRotationAngle.z)
        {
            _rotationDirections[i][0] = -1.0f; // Yön değiştir
        }
        else if (currentRotation.z < _upperLegRotationAngle.z - RotationAngleZ * 2)
        {
            _rotationDirections[i][0] = 1.0f; // Yön değiştir
        }
        /*
                if (currentRotation.y > RotationAngleY + _upperLegRotationAngle.y + (i >= _legCount / 2 ? 0.0f : 180.0f))
                {
                    _rotationDirections[i][1] = -1.0f; // Yön değiştir
                }
                else if (currentRotation.y < -RotationAngleY + _upperLegRotationAngle.y + (i >= _legCount / 2 ? 0.0f : 180.0f))
                {
                    _rotationDirections[i][1] = 1.0f; // Yön değiştir
                }

                // Pivot'u yeni yön ve hız ile döndür
                pivot->Rotate(pivot->transform.eulerRot +
                              glm::vec3(0.0f, _rotationDirections[i][1] * RotationSpeed * deltaTime, _rotationDirections[i][0] * RotationSpeed * deltaTime)); */

        pivot->Rotate(pivot->transform.eulerRot +
                      glm::vec3(0.0f, 0.0f, _rotationDirections[i][0] * RotationSpeed * deltaTime));
        float rotation = _spiderTransform.eulerRot.y * glm::pi<float>() / 180.0f;
        glm::vec3 direction = glm::vec3(sin(rotation), 0.0f, cos(rotation));
        _spiderNode->Move(_spiderNode->transform.pos + direction * _moveSpeed * deltaTime);
    }
    CollisionController::Instance().UpdateCollider(_spiderCollider, _spiderTransform.pos);
}

void Spider::Patrol()
{
    _moveSpeed *= -1.0f;
}

void Spider::SetCollider(SphereCollider *sphereCollider)
{
    _spiderCollider = sphereCollider;
}

SphereCollider *Spider::GetCollider()
{
    return _spiderCollider;
}