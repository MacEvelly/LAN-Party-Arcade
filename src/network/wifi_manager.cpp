#include "wifi_manager.h"

String WiFiManager::currentSSID = "";

String WiFiManager::createUniqueSSID(const String& baseSSID) {
  String testSSID = baseSSID;
  int suffix = 1;
  
  Serial.println("\nScanning for SSID collisions...");
  
  // Scan for existing networks
  int networksFound = WiFi.scanNetworks();
  Serial.printf("Found %d networks\n", networksFound);
  
  bool collision = true;
  while (collision && suffix < 100) { // Max 99 attempts
    collision = false;
    
    for (int i = 0; i < networksFound; i++) {
      if (WiFi.SSID(i) == testSSID) {
        collision = true;
        Serial.printf("  Collision detected: %s\n", testSSID.c_str());
        suffix++;
        testSSID = baseSSID + "_" + String(suffix);
        break;
      }
    }
  }
  
  if (!collision) {
    Serial.printf("  Using SSID: %s\n", testSSID.c_str());
  }
  
  return testSSID;
}

bool WiFiManager::startAccessPoint(const SystemConfig& config, String& outActualSSID) {
  Serial.println("\n--- Starting WiFi Access Point ---");
  
  // Set WiFi to AP mode
  WiFi.mode(WIFI_AP);
  delay(100);
  
  // Create unique SSID
  currentSSID = createUniqueSSID(config.wifiSSID);
  outActualSSID = currentSSID;
  
  // Start AP
  bool success;
  if (config.wifiPassword.length() > 0) {
    success = WiFi.softAP(currentSSID.c_str(), config.wifiPassword.c_str(), 
                         1, 0, config.maxConnections);
    Serial.printf("Starting AP with password: %s\n", currentSSID.c_str());
  } else {
    success = WiFi.softAP(currentSSID.c_str(), NULL, 1, 0, config.maxConnections);
    Serial.printf("Starting open AP: %s\n", currentSSID.c_str());
  }
  
  if (success) {
    Serial.println("WiFi AP started successfully!");
    Serial.printf("  SSID: %s\n", currentSSID.c_str());
    Serial.printf("  IP: %s\n", WiFi.softAPIP().toString().c_str());
    Serial.printf("  Max clients: %d\n", config.maxConnections);
    return true;
  } else {
    Serial.println("ERROR: Failed to start WiFi AP!");
    return false;
  }
}

int WiFiManager::getConnectedClients() {
  return WiFi.softAPgetStationNum();
}

IPAddress WiFiManager::getIP() {
  return WiFi.softAPIP();
}
