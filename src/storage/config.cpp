#include "config.h"
#include <SD.h>
#include <ArduinoJson.h>

bool ConfigManager::loadFromSD(const char* path, SystemConfig& config) {
  Serial.println("\nChecking for config.json...");
  
  if (!SD.exists(path)) {
    Serial.println("No config.json found, using defaults");
    return false;
  }
  
  File configFile = SD.open(path, FILE_READ);
  if (!configFile) {
    Serial.println("Could not open config.json");
    return false;
  }
  
  Serial.println("Found config.json, loading...");
  
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, configFile);
  configFile.close();
  
  if (error) {
    Serial.printf("JSON parse error: %s\n", error.c_str());
    return false;
  }
  
  // Load each config field if present
  if (doc["wifiSSID"].is<String>()) {
    config.wifiSSID = doc["wifiSSID"].as<String>();
    Serial.printf("  Custom SSID: %s\n", config.wifiSSID.c_str());
  }
  
  if (doc["wifiPassword"].is<String>()) {
    config.wifiPassword = doc["wifiPassword"].as<String>();
    Serial.printf("  Password: %s\n", config.wifiPassword.length() > 0 ? "***" : "(none)");
  }
  
  if (doc["maxConnections"].is<int>()) {
    config.maxConnections = doc["maxConnections"];
    Serial.printf("  Max connections: %d\n", config.maxConnections);
  }
  
  if (doc["hostname"].is<String>()) {
    config.hostname = doc["hostname"].as<String>();
    Serial.printf("  Hostname: %s.local\n", config.hostname.c_str());
  }
  
  if (doc["headerBMP"].is<String>()) {
    config.headerBMP = doc["headerBMP"].as<String>();
    Serial.printf("  Header BMP: %s\n", config.headerBMP.c_str());
  }
  
  return true;
}

void ConfigManager::printConfig(const SystemConfig& config) {
  Serial.println("\n=== Current Configuration ===");
  Serial.printf("  SSID: %s\n", config.wifiSSID.c_str());
  Serial.printf("  Password: %s\n", config.wifiPassword.length() > 0 ? "***" : "(none)");
  Serial.printf("  Hostname: %s\n", config.hostname.c_str());
  Serial.printf("  Header BMP: %s\n", config.headerBMP.c_str());
  Serial.printf("  Max Connections: %d\n", config.maxConnections);
  Serial.println("============================\n");
}
