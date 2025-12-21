#include "ConfigManager.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <json/json.h> // Projendeki json kutuphanesi (nlohmann/json varsayildi)

// Eger kutuphanen nlohmann ise bu kolayligi saglar, degilse kaldirabilirsin
using json = nlohmann::json;
namespace fs = std::filesystem;

ConfigManager::ConfigManager() {}
ConfigManager::~ConfigManager() {}

void ConfigManager::Initialize() { EnsureDirectoryExists(); }

void ConfigManager::EnsureDirectoryExists() {
  if (!fs::exists(folderName)) {
    fs::create_directory(folderName);
  }
}

std::vector<std::string> ConfigManager::GetConfigFileList() {
  std::vector<std::string> files;
  EnsureDirectoryExists();

  for (const auto &entry : fs::directory_iterator(folderName)) {
    // Sadece .json veya .config uzantili dosyalari listele
    if (entry.path().extension() == ".json" ||
        entry.path().extension() == ".config") {
      files.push_back(entry.path().filename().string());
    }
  }
  return files;
}

void ConfigManager::SaveConfig(const std::string &filename,
                               const Game::GameData &data) {
  EnsureDirectoryExists();

  // Dosya uzantisi kontrolu
  std::string path = folderName + "/" + filename;
  if (path.find(".") == std::string::npos) {
    path += ".json"; // Varsayilan olarak json uzantisi ekle
  }

  json j;

  // --- General Settings ---
  j["General"]["PolygonMode"] = data.polygonMode;
  j["General"]["ShowColliders"] = data.showColliders;
  j["General"]["SpiderMovement"] = data.areSpidersMoving;

  // --- Spider Data ---
  j["Spider"]["Count"] = data.spiderGenerationData.EntityCount;
  j["Spider"]["LegMin"] = data.spiderGenerationData.LegPairCountMin;
  j["Spider"]["LegMax"] = data.spiderGenerationData.LegPairCountMax;
  j["Spider"]["SpeedMin"] = data.spiderGenerationData.MoveSpeedMin;
  j["Spider"]["SpeedMax"] = data.spiderGenerationData.MoveSpeedMax;
  j["Spider"]["ScaleUpperMin"] = data.spiderGenerationData.UpperLegSizeScaleMin;
  j["Spider"]["ScaleUpperMax"] = data.spiderGenerationData.UpperLegSizeScaleMax;
  j["Spider"]["ScaleMiddleMin"] =
      data.spiderGenerationData.MiddleLegSizeScaleMin;
  j["Spider"]["ScaleMiddleMax"] =
      data.spiderGenerationData.MiddleLegSizeScaleMax;
  j["Spider"]["ScaleLowerMin"] = data.spiderGenerationData.LowerLegSizeScaleMin;
  j["Spider"]["ScaleLowerMax"] = data.spiderGenerationData.LowerLegSizeScaleMax;

  // --- Caterpillar Data ---
  j["Caterpillar"]["Count"] = data.caterpillarGenerationData.EntityCount;
  j["Caterpillar"]["SegMin"] = data.caterpillarGenerationData.LegPairCountMin;
  j["Caterpillar"]["SegMax"] = data.caterpillarGenerationData.LegPairCountMax;
  j["Caterpillar"]["SpeedMin"] = data.caterpillarGenerationData.MoveSpeedMin;
  j["Caterpillar"]["SpeedMax"] = data.caterpillarGenerationData.MoveSpeedMax;

  // --- Producer Data ---
  j["Producer"]["Count"] = data.producerGenerationData.EntityCount;
  // Producer structinda diger veriler varsa onlari da ekle...

  // Dosyaya yazma
  std::ofstream file(path);
  if (file.is_open()) {
    file << j.dump(4); // 4 space indentation ile guzel formatta yaz
    file.close();
    std::cout << "[ConfigManager] Saved to " << path << std::endl;
  } else {
    std::cerr << "[ConfigManager] Error: Could not save to " << path
              << std::endl;
  }
}

void ConfigManager::LoadConfig(const std::string &filename,
                               Game::GameData &data) {
  std::string path = folderName + "/" + filename;
  std::ifstream file(path);

  if (!file.is_open()) {
    std::cerr << "[ConfigManager] Error: Could not open " << path << std::endl;
    return;
  }

  try {
    json j;
    file >> j;

    // --- General Settings ---
    if (j.contains("General")) {
      data.polygonMode = j["General"].value("PolygonMode", false);
      data.showColliders = j["General"].value("ShowColliders", false);
      data.areSpidersMoving = j["General"].value("SpiderMovement", true);
    }

    // --- Spider Data ---
    if (j.contains("Spider")) {
      auto &s = j["Spider"];
      data.spiderGenerationData.EntityCount = s.value("Count", 16);
      data.spiderGenerationData.LegPairCountMin = s.value("LegMin", 1);
      data.spiderGenerationData.LegPairCountMax = s.value("LegMax", 5);
      data.spiderGenerationData.MoveSpeedMin = s.value("SpeedMin", 0.2f);
      data.spiderGenerationData.MoveSpeedMax = s.value("SpeedMax", 0.8f);
      data.spiderGenerationData.UpperLegSizeScaleMin =
          s.value("ScaleUpperMin", 0.25f);
      data.spiderGenerationData.UpperLegSizeScaleMax =
          s.value("ScaleUpperMax", 0.25f);
      data.spiderGenerationData.MiddleLegSizeScaleMin =
          s.value("ScaleMiddleMin", 0.25f);
      data.spiderGenerationData.MiddleLegSizeScaleMax =
          s.value("ScaleMiddleMax", 0.25f);
      data.spiderGenerationData.LowerLegSizeScaleMin =
          s.value("ScaleLowerMin", 0.25f);
      data.spiderGenerationData.LowerLegSizeScaleMax =
          s.value("ScaleLowerMax", 0.25f);
    }

    // --- Caterpillar Data ---
    if (j.contains("Caterpillar")) {
      auto &c = j["Caterpillar"];
      data.caterpillarGenerationData.EntityCount = c.value("Count", 8);
      data.caterpillarGenerationData.LegPairCountMin = c.value("SegMin", 3);
      data.caterpillarGenerationData.LegPairCountMax = c.value("SegMax", 8);
      data.caterpillarGenerationData.MoveSpeedMin = c.value("SpeedMin", 0.4f);
      data.caterpillarGenerationData.MoveSpeedMax = c.value("SpeedMax", 1.2f);
    }

    // --- Producer Data ---
    if (j.contains("Producer")) {
      data.producerGenerationData.EntityCount = j["Producer"].value("Count", 8);
    }

    std::cout << "[ConfigManager] Loaded configuration from " << path
              << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "[ConfigManager] JSON Parse Error: " << e.what() << std::endl;
  }

  file.close();
}