#include "AnalyticsManager.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <json/json.h> // ConfigManager'daki ayni kutuphane

using json = nlohmann::json;
namespace fs = std::filesystem;

AnalyticsManager::AnalyticsManager() {}

AnalyticsManager::~AnalyticsManager() {
  if (_currentRunResult)
    delete _currentRunResult;
  if (_loadedRunResult)
    delete _loadedRunResult;
}

void AnalyticsManager::Initialize() { EnsureDirectoryExists(); }

void AnalyticsManager::EnsureDirectoryExists() {
  if (!fs::exists(folderName)) {
    fs::create_directory(folderName);
  }
}

void AnalyticsManager::StartNewRun(const Game::GameData &initialConfig,
                                   const std::string &runName) {
  if (_currentRunResult)
    EndCurrentRun(0); // Varsa oncekini kapat

  _currentRunResult = new SimulationRunResult();
  _currentRunResult->InitialConfig = initialConfig;
  _currentRunResult->RunName = runName;
  _currentRunResult->TotalRunTime = 0.0f;
  _currentRunResult->Snapshots.clear();

  _lastLogTime = -10.0f; // Ilk frame'de kesin log almasi icin

  std::cout << "[Analytics] New run started: " << runName << std::endl;
}

// DUZELTİLEN FONKSİYON: Artik veriyi Game.cpp'den aliyor ve zamanlamasi dogru
void AnalyticsManager::LogSnapshot(float currentTime, int sCount, int cCount,
                                   int pCount) {
  if (!_currentRunResult)
    return;

  // "No data points" hatasinin cozumu burasi:
  // Eger son loglamadan beri _logInterval (0.5 sn) gectiyse kaydet.
  if (currentTime - _lastLogTime >= _logInterval) {
    SimulationSnapshot snapshot;
    snapshot.Time = currentTime;
    snapshot.SpiderCount = sCount;
    snapshot.CaterpillarCount = cCount;
    snapshot.ProducerCount = pCount;

    _currentRunResult->Snapshots.push_back(snapshot);
    _lastLogTime = currentTime;
  }
}

void AnalyticsManager::LogDeath(EntityType type, glm::vec3 position) {
  if (!_currentRunResult)
    return;
  switch (type) {
  case EntityType::Spider:
    _currentRunResult->TotalSpiderDeaths++;
    break;
  case EntityType::Caterpillar:
    _currentRunResult->TotalCaterpillarDeaths++;
    break;
  case EntityType::Producer:
    _currentRunResult->TotalProducerDeaths++;
    break;
  }
}

void AnalyticsManager::EndCurrentRun(float currentTime) {
  if (_currentRunResult) {
    _currentRunResult->TotalRunTime =
        !_currentRunResult->Snapshots.empty()
            ? _currentRunResult->Snapshots.back().Time
            : 0.0f;

    // Dosyaya kaydet
    SaveRunToDisk(*_currentRunResult);

    delete _currentRunResult;
    _currentRunResult = nullptr;
  }
}

// --- DOSYA ISLEMLERI ---

void AnalyticsManager::SaveRunToDisk(const SimulationRunResult &run) {
  EnsureDirectoryExists();
  std::string filename = folderName + "/" + run.RunName + ".json";

  json j;
  j["RunName"] = run.RunName;
  j["TotalTime"] = run.TotalRunTime;
  j["Deaths"]["Spider"] = run.TotalSpiderDeaths;
  j["Deaths"]["Caterpillar"] = run.TotalCaterpillarDeaths;
  j["Deaths"]["Producer"] = run.TotalProducerDeaths;

  // Snapshots (Grafik verisi)
  j["Snapshots"] = json::array();
  for (const auto &s : run.Snapshots) {
    j["Snapshots"].push_back({{"t", s.Time},
                              {"s", s.SpiderCount},
                              {"c", s.CaterpillarCount},
                              {"p", s.ProducerCount}});
  }

  // Config verisini de kaydetmek istersen ConfigManager'daki mantigi buraya
  // kopyalayabilirsin Simdilik sadelik adina config detayini atliyorum.

  std::ofstream file(filename);
  if (file.is_open()) {
    file << j.dump(4);
    std::cout << "[Analytics] Saved run to " << filename << std::endl;
  }
}

std::vector<std::string> AnalyticsManager::GetRunFileList() {
  std::vector<std::string> files;
  EnsureDirectoryExists();
  for (const auto &entry : fs::directory_iterator(folderName)) {
    if (entry.path().extension() == ".json") {
      files.push_back(entry.path().filename().string()); // Sadece dosya adi
    }
  }
  return files;
}

void AnalyticsManager::LoadRun(const std::string &filename) {
  std::string path = folderName + "/" + filename;
  std::ifstream file(path);
  if (!file.is_open())
    return;

  if (_loadedRunResult)
    delete _loadedRunResult;
  _loadedRunResult = new SimulationRunResult();

  try {
    json j;
    file >> j;

    _loadedRunResult->RunName = j["RunName"];
    _loadedRunResult->TotalRunTime = j["TotalTime"];
    _loadedRunResult->TotalSpiderDeaths = j["Deaths"]["Spider"];
    _loadedRunResult->TotalCaterpillarDeaths = j["Deaths"]["Caterpillar"];
    _loadedRunResult->TotalProducerDeaths = j["Deaths"]["Producer"];

    for (const auto &s : j["Snapshots"]) {
      SimulationSnapshot snap;
      snap.Time = s["t"];
      snap.SpiderCount = s["s"];
      snap.CaterpillarCount = s["c"];
      snap.ProducerCount = s["p"];
      _loadedRunResult->Snapshots.push_back(snap);
    }

    // Grafik icin hazirla
    PrepareDataForPlotting(_loadedRunResult);

  } catch (const std::exception &e) {
    std::cerr << "[Analytics] Load Error: " << e.what() << std::endl;
  }
}

void AnalyticsManager::PrepareDataForPlotting(const SimulationRunResult *run) {
  PlotTimes.clear();
  PlotSpiderCounts.clear();
  PlotCaterpillarCounts.clear();

  if (!run)
    return;

  // Baslangic zamani (t0) sifirlamak icin ilk snapshot'in zamanini alabiliriz
  // Ama direkt raw time basalim.
  for (const auto &s : run->Snapshots) {
    PlotTimes.push_back(s.Time);
    PlotSpiderCounts.push_back((float)s.SpiderCount);
    PlotCaterpillarCounts.push_back((float)s.CaterpillarCount);
  }
}