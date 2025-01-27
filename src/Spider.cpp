#include <Spider.h>
#include <ctime>

Spider::Spider(glm::vec3 SpiderLocation, int LegCount, float HipLocationAsDegree, glm::vec3 BodySize, glm::vec3 UpperLegSize, glm::vec3 MiddleLegSize, glm::vec3 LowerLegSize, glm::vec3 UpperLegRotationAngle, glm::vec3 MiddleLegRotationAngle, glm::vec3 LowerLegRotationAngle) : _spiderLocation(SpiderLocation), _legCount(LegCount), _hipLocationAsDegree(HipLocationAsDegree), _bodySize(BodySize), _upperLegSize(UpperLegSize), _middleLegSize(MiddleLegSize), _lowerLegSize(LowerLegSize), _upperLegRotationAngle(UpperLegRotationAngle), _middleLegRotationAngle(MiddleLegRotationAngle), _lowerLegRotationAngle(LowerLegRotationAngle)
{
    _spiderEntity = new Entity(Transform(SpiderLocation));
    _sBody = new Entity(Transform(glm::vec3(), glm::vec3(), _bodySize), SPHERE, Obsidian);

    _spiderEntity->AddChild(_sBody);

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
        _sUpperLegsPivot.emplace_back(new Entity(Transform(glm::vec3(x, y, z), _upperLegRotationAngle + glm::vec3(0.0f, (i >= _legCount / 2 ? 0.0f : 180.0f), (float)(i - _legCount / 2) * (15.0f / (float)(_legCount))))));
        _sUpperLegs.emplace_back(new Entity(Transform(_upperLegSize * glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(), _upperLegSize), SPHERE, Ruby));
        _sMiddleLegsPivot.emplace_back(new Entity(Transform(_upperLegSize * glm::vec3(0.0f, 1.0f, 0.0f), _middleLegRotationAngle)));
        _sMiddleLegs.emplace_back(new Entity(Transform(_middleLegSize * glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(), _middleLegSize), SPHERE, Ruby));
        _sLowerLegsPivot.emplace_back(new Entity(Transform(_middleLegSize * glm::vec3(0.0f, 1.0f, 0.0f), _lowerLegRotationAngle)));
        _sLowerLegs.emplace_back(new Entity(Transform(_lowerLegSize * glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(), _lowerLegSize), SPHERE, Ruby));

        _spiderEntity->AddChild(_sUpperLegsPivot.at(i));
        _sUpperLegsPivot.at(i)->AddChild(_sUpperLegs.at(i));
        _sUpperLegsPivot.at(i)->AddChild(_sMiddleLegsPivot.at(i));
        _sMiddleLegsPivot.at(i)->AddChild(_sLowerLegsPivot.at(i));
        _sMiddleLegsPivot.at(i)->AddChild(_sMiddleLegs.at(i));
        _sLowerLegsPivot.at(i)->AddChild(_sLowerLegs.at(i));
    }
    _rotationDirections = std::vector<float>(_sUpperLegsPivot.size(), 1.0f);
}

Entity *Spider::GetEntity()
{
    return _spiderEntity;
}

void Spider::Move(float deltaTime)
{
    float RotationAngle = 5.0f;  // Bacakların maksimum dönüş açısı
    float RotationSpeed = 20.0f; // Dönüş hızı

    for (size_t i = 0; i < _sUpperLegsPivot.size(); ++i)
    {
        Entity *pivot = _sUpperLegsPivot[i];
        float currentRotationZ = pivot->transform.eulerRot.z;

        // Dönüş yönünü kontrol et ve değiştir
        if (currentRotationZ > RotationAngle + _upperLegRotationAngle.z)
        {
            _rotationDirections[i] = -1.0f; // Yön değiştir
        }
        else if (currentRotationZ < -RotationAngle + _upperLegRotationAngle.z)
        {
            _rotationDirections[i] = 1.0f; // Yön değiştir
        }

        // Pivot'u yeni yön ve hız ile döndür
        pivot->Rotate(pivot->transform.eulerRot +
                      glm::vec3(0.0f, 0.0f, _rotationDirections[i] * RotationSpeed * deltaTime));
    }
}
