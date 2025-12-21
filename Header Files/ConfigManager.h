#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "Game.h"      // GameData yapisini tanimasi icin
#include "Singleton.h" // Senin Singleton.h dosyan
#include <string>
#include <vector>


class ConfigManager : public Singleton<ConfigManager> {
  friend Singleton<ConfigManager>;

public:
  ConfigManager();
  ~ConfigManager();

  // Klasorun varligini kontrol eder, yoksa olusturur
  void Initialize();

  // Mevcut GameData'yi JSON olarak kaydeder
  void SaveConfig(const std::string &filename, const Game::GameData &data);

  // JSON dosyasindan okuyup GameData referansini gunceller
  void LoadConfig(const std::string &filename, Game::GameData &data);

  // "configs" klasoru icindeki .config dosyalarinin isimlerini doner
  std::vector<std::string> GetConfigFileList();

private:
  const std::string folderName = "configs";
  void EnsureDirectoryExists();
};

#endif