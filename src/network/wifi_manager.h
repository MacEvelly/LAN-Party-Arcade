#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "storage/config.h"

class WiFiManager {
public:
  // Start WiFi Access Point with automatic SSID collision detection
  static bool startAccessPoint(const SystemConfig& config, String& outActualSSID);
  
  // Create unique SSID by detecting collisions
  static String createUniqueSSID(const String& baseSSID);
  
  // Get number of connected clients
  static int getConnectedClients();
  
  // Get AP IP address
  static IPAddress getIP();

private:
  static String currentSSID;
};

#endif
