#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// System configuration structure
struct SystemConfig {
  String wifiSSID = "LAN_Party_Arcade";
  String wifiPassword = "";
  String hostname = "play";
  String headerBMP = "Header.bmp";
  int maxConnections = 20;
};

class ConfigManager {
public:
  // Load configuration from SD card JSON file
  static bool loadFromSD(const char* path, SystemConfig& config);
  
  // Print configuration to Serial
  static void printConfig(const SystemConfig& config);
};

#endif
