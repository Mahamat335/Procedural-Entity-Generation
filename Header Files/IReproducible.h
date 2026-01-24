#ifndef REPRODUCIBLE_H
#define REPRODUCIBLE_H

#include <cstdlib>
#include <glm/glm.hpp>

class IReproducible {
public:
  float MutationRate = 0.1f; // %10 mutasyon payı (Dışarıdan set edilebilir)

  // Üreme şartı kontrolü (Senin belirttiğin kurallar)
  bool IsReadyToReproduce(float hunger, float maxHunger, float feedLevel) {
    return (feedLevel >= 1.0f);
  }

  bool IsReadyToPropose(float hunger, float maxHunger, float feedLevel) {
    return (feedLevel >= 2.0f);
  }

  // Saf sanal fonksiyon: Her tür kendi "Bebek Data"sını oluşturup Game'e
  // ekleyecek
  virtual void Reproduce(IReproducible *partner) = 0;

  // Mutasyon Hesaplama Fonksiyonu (Float için)
  float CalculateTrait(float p1, float p2) {
    float average = (p1 + p2) / 2.0f;
    // -MutationRate ile +MutationRate arasında rastgele bir kayma (Örn: -0.1
    // ile +0.1)
    float mutationShift =
        ((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * MutationRate;
    return average + (average * mutationShift);
  }

  // Mutasyon Hesaplama Fonksiyonu (glm::vec3 için)
  float CalculateTrait(float p1, float p2, float mRate) {
    float average = (p1 + p2) / 2.0f;
    // Mutasyon: mRate kadar yukarı veya aşağı sapma (Örn: mRate 0.1 ise %10
    // sapma)
    float deviation =
        ((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * mRate;
    return average + (average * deviation);
  }

  glm::vec3 CalculateTrait(glm::vec3 p1, glm::vec3 p2, float mRate) {
    return glm::vec3(CalculateTrait(p1.x, p2.x, mRate),
                     CalculateTrait(p1.y, p2.y, mRate),
                     CalculateTrait(p1.z, p2.z, mRate));
  }

  // Tamsayı (Bacak sayısı vb.) için mutasyon
  int CalculateTrait(int p1, int p2) {
    float avg = (static_cast<float>(p1) + static_cast<float>(p2)) / 2.0f;
    float mutationShift =
        ((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * MutationRate;
    int result = static_cast<int>(avg + (avg * mutationShift));
    return result < 2 ? 2 : result; // En az 2 bacak/segment kuralı
  }
};

#endif