#include <Spider.h>

Spider::Spider(glm::vec3 SpiderLocation, int LegCount, float HipLocationAsDegree, glm::vec3 BodySize, glm::vec3 UpperLegSize, glm::vec3 LowerLegSize, glm::vec3 UpperLegRotationAngle, glm::vec3 LowerLegRotationAngle) : _spiderLocation(SpiderLocation), _legCount(LegCount), _hipLocationAsDegree(HipLocationAsDegree), _bodySize(BodySize), _upperLegSize(UpperLegSize), _lowerLegSize(LowerLegSize), _upperLegRotationAngle(UpperLegRotationAngle), _lowerLegRotationAngle(LowerLegRotationAngle)
{
    std::cout << 1;
    _spiderEntity = new Entity(Transform(SpiderLocation));
    _sBody = new Entity(Transform(glm::vec3(), glm::vec3(), _bodySize), SPHERE, Obsidian);
    std::cout << 1;
    _spiderEntity->AddChild(_sBody);
    std::cout << 1;
    float verticalAngle = -_hipLocationAsDegree * glm::pi<float>() / 180.0f; // Dikey açı (kürenin alt tarafına yakın)
    float horizontalStep = glm::two_pi<float>() / _legCount;                 // her bacak için eşit yatay açı
    std::cout << 1;
    for (int i = 0; i < _legCount; i++)
    {
        std::cout << 1;
        float theta = i * horizontalStep; // Her bacak için yatay açı
        float phi = verticalAngle;        // Sabit dikey açı

        // Kürenin yüzeyindeki noktaları hesapla
        float x = (_bodySize.x * 0.5f) * cos(phi) * cos(theta);
        float y = (_bodySize.y * 0.5f) * sin(phi); // Dikey açı y bileşeni
        float z = (_bodySize.z * 0.5f) * cos(phi) * sin(theta);

        // Pivot noktasını tanımla
        std::cout << 88;
        _sUpperLegsPivot.emplace_back(new Entity(Transform(glm::vec3(x, y, z), _upperLegRotationAngle * (i >= _legCount / 2 ? 1.0f : -1.0f))));
        std::cout << 88;
        _sUpperLegs.emplace_back(new Entity(Transform(_upperLegSize * glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(), _upperLegSize), SPHERE, Ruby));
        _sLowerLegsPivot.emplace_back(new Entity(Transform(_upperLegSize * glm::vec3(0.0f, 1.0f, 0.0f), _lowerLegRotationAngle * (i >= _legCount / 2 ? 1.0f : -1.0f))));
        _sLowerLegs.emplace_back(new Entity(Transform(_lowerLegSize * glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(), _lowerLegSize), SPHERE, Ruby));

        _spiderEntity->AddChild(_sUpperLegsPivot.at(i));
        _sUpperLegsPivot.at(i)->AddChild(_sUpperLegs.at(i));
        _sUpperLegsPivot.at(i)->AddChild(_sLowerLegsPivot.at(i));
        _sLowerLegsPivot.at(i)->AddChild(_sLowerLegs.at(i));
    }
}

Entity *Spider::GetEntity()
{
    return _spiderEntity;
}

void Spider::Move()
{
    // move
}