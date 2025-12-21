#ifndef ANALYTICS_MANAGER_H
#define ANALYTICS_MANAGER_H

#include "Game.h"
#include "Singleton.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>


enum class EntityType { Spider, Caterpillar, Producer };

struct SimulationSnapshot {
  float Time;
  int SpiderCount;
  int CaterpillarCount;
  int ProducerCount;
};

struct SimulationRunResult {
  std::string RunName;
  Game::GameData InitialConfig;
  std::vector<SimulationSnapshot> Snapshots;
  float TotalRunTime;
  int TotalSpiderDeaths = 0;
  int TotalCaterpillarDeaths = 0;
  int TotalProducerDeaths = 0;
};

class AnalyticsManager : public Singleton<AnalyticsManager> {
  friend Singleton<AnalyticsManager>;

public:
  AnalyticsManager();
  ~AnalyticsManager();

  void Initialize(); // Klasor kontrolu icin

  // Simulasyon Yonetimi
  void StartNewRun(const Game::GameData &initialConfig,
                   const std::string &runName);
  void LogSnapshot(float currentTime, int sCount, int cCount, int pCount);
  void LogDeath(EntityType type, glm::vec3 position);
  void EndCurrentRun(float currentTime);

  // Dosya Islemleri (YENI KISIM)
  std::vector<std::string> GetRunFileList(); // Kayitli dosya isimlerini doner
  void LoadRun(
      const std::string &filename); // Secilen dosyayi _loadedRunResult'a yukler

  // UI Erisimi
  const SimulationRunResult *GetCurrentRunResults() const {
    return _currentRunResult;
  }
  const SimulationRunResult *GetLoadedRunResults() const {
    return _loadedRunResult;
  } // Artik historical degil, loaded var

  // Plot Data (ImGui icin)
  void PrepareDataForPlotting(const SimulationRunResult *run);
  std::vector<float> PlotTimes;
  std::vector<float> PlotSpiderCounts;
  std::vector<float> PlotCaterpillarCounts;

private:
  const std::string folderName = "analytics";
  SimulationRunResult *_currentRunResult = nullptr; // Su an kaydettigimiz
  SimulationRunResult *_loadedRunResult =
      nullptr; // Dosyadan okuyup inceledigimiz

  float _logInterval =
      0.5f; // Yarim saniyede bir kayit al (Hassasiyeti arttirdim)
  float _lastLogTime = 0.0f;

  void SaveRunToDisk(const SimulationRunResult &run);
  void EnsureDirectoryExists();
};

#endif