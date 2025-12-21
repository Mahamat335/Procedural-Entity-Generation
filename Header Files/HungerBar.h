#ifndef HUNGER_BAR_H
#define HUNGER_BAR_H

#include "BarRenderer.h"
#include <algorithm> // clamp icin
#include <glm/glm.hpp>


class HungerBar {
private:
  glm::vec2 _size;       // Barin boyutu
  float _verticalOffset; // Yaratigin kafasinin ne kadar ustunde duracagi

  // Rengi yuzdeye gore hesaplayan gizli fonksiyon
  glm::vec3 CalculateColor(float percentage) {
    // Renkler
    glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 yellow = glm::vec3(1.0f, 1.0f, 0.0f);
    glm::vec3 green = glm::vec3(0.0f, 1.0f, 0.0f);

    // Eger %50'den azsa Kirmizi -> Sari gecisi
    if (percentage < 0.5f) {
      // 0.0 - 0.5 arasini 0.0 - 1.0 araligina genisletiyoruz
      float t = percentage * 2.0f;
      return glm::mix(red, yellow, t);
    }
    // Eger %50'den fazlaysa Sari -> Yesil gecisi
    else {
      // 0.5 - 1.0 arasini 0.0 - 1.0 araligina genisletiyoruz
      float t = (percentage - 0.5f) * 2.0f;
      return glm::mix(yellow, green, t);
    }
  }

public:
  // Default degerler
  HungerBar() : _size(1.0f, 0.1f), _verticalOffset(1.0f) {}

  // Sadece boyut ve konum ayari (Entity'ler bunu constructor'da bir kez
  // cagirir)
  void Setup(glm::vec2 size, float offset) {
    _size = size;
    _verticalOffset = offset;
  }

  // Her frame cagirilacak cizim fonksiyonu
  void Render(glm::vec3 entityPosition, float currentHunger, float maxHunger,
              const glm::mat4 &view, const glm::mat4 &projection) {

    // --- MANTIK KISMI ---
    // Senin sisteminde Hunger yukseldikce (100 olunca) oluyordu.
    // Biz bar dolu gozuksun diye "Kalan Enerji"yi hesapliyoruz.
    // currentHunger 0 ise (Tok) -> Enerji Full (Max)
    // currentHunger 100 ise (Ac) -> Enerji 0
    float remainingEnergy = maxHunger - currentHunger;

    // 0.0 ile 1.0 arasinda bir oran bul (Yuzde)
    float percentage = glm::clamp(remainingEnergy / maxHunger, 0.0f, 1.0f);

    // Rengi otomatik hesapla
    glm::vec3 barColor = CalculateColor(percentage);

    // Pozisyonu ayarla
    glm::vec3 barPos = entityPosition + glm::vec3(0.0f, _verticalOffset, 0.0f);

    // Renderer'a gonder
    // Dikkat: artik drawBar'a 'remainingEnergy' gonderiyoruz ki dolu kismi
    // cizsin
    BarRenderer::Instance().DrawBar(barPos, remainingEnergy, maxHunger, _size,
                                    barColor, view, projection);
  }
};

#endif