#include <Arduino.h>
#include <ESPmDNS.h>

// Module includes
#include "storage/sd_card.h"
#include "storage/config.h"
#include "network/wifi_manager.h"
#include "network/dns_server.h"
#include "network/web_server.h"
#include "network/websocket_server.h"
#include "display/display.h"
#include "utils/helpers.h"

// Pin definitions for ESP32-2432S028
#define SD_CS 5

// System state
SystemConfig config;
String actualSSID = "";
bool sdCardMounted = false;

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n=== LAN Party Arcade ===");
  Serial.println("Modular Architecture V1.0\n");

  // 1. Initialize display
  DisplayManager::init();
  
  // Show "SD Card: " on display
  // (Display module handles its own TFT operations internally)

  // 2. Initialize SD card
  Serial.println("\nInitializing SD card...");
  sdCardMounted = SDCard::init(SD_CS);
  
  // 3. Load configuration from SD card
  if (sdCardMounted) {
    ConfigManager::loadFromSD("/config.json", config);
  } else {
    Serial.println("Using default configuration");
  }

  // 4. Start WiFi Access Point
  WiFiManager::startAccessPoint(config, actualSSID);
  
  // 5. Start DNS Server
  DNSManager::start(53);
  
  // 6. Start Web Server
  HTTPServer::start(80);
  
  // 7. Start mDNS responder
  Serial.println("\n--- Starting mDNS ---");
  if (MDNS.begin(config.hostname.c_str())) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("mDNS responder started");
    Serial.printf("Access at: http://%s.local\n", config.hostname.c_str());
  } else {
    Serial.println("Error setting up mDNS responder!");
  }
  
  // 8. Start WebSocket Server
  WebSocketRelay::start(81);
  
  // 9. Show connection screen
  DisplayManager::showConnectionScreen(config, actualSSID);
  
  Serial.println("\n=== Ready! ===");
  Serial.println("Display: OK");
  Serial.printf("SD Card: %s\n", sdCardMounted ? "OK" : "FAILED");
  Serial.printf("WiFi AP: %s\n", actualSSID.c_str());
  Serial.printf("IP Address: %s\n", WiFiManager::getIP().toString().c_str());
  Serial.printf("Free RAM: %d KB\n", ESP.getFreeHeap() / 1024);
  Serial.println("All systems operational!");
}

void loop() {
  // Process network services
  DNSManager::process();
  HTTPServer::process();
  WebSocketRelay::process();
  
  // Handle touch input (simple detection - touch anywhere)
  static unsigned long lastTouchTime = 0;
  static bool wasTouched = false;
  uint16_t touchX = 0, touchY = 0;
  
  bool isTouched = DisplayManager::checkTouch(touchX, touchY);
  
  // Detect touch press (rising edge)
  if (isTouched && !wasTouched && (millis() - lastTouchTime > 500)) {
    lastTouchTime = millis();
    wasTouched = true;
    
    Serial.printf("=== SCREEN TAP DETECTED at (%d, %d) ===\n", touchX, touchY);
    
    DisplayManager::toggleScreen();
    
    if (DisplayManager::getCurrentScreen() == Screen::CONNECTION) {
      Serial.println("Showing: Connection Screen");
      DisplayManager::showConnectionScreen(config, actualSSID);
    } else {
      Serial.println("Showing: Stats Screen");
      int wifiClients = WiFiManager::getConnectedClients();
      int wsClients = WebSocketRelay::getClientCount();
      DisplayManager::showStatsScreen(wifiClients, wsClients, sdCardMounted, config, actualSSID);
    }
  }
  
  // Track release
  if (!isTouched) {
    wasTouched = false;
  }
  
  // Update stats screen if showing
  static unsigned long lastStatsUpdate = 0;
  if (DisplayManager::getCurrentScreen() == Screen::STATS && (millis() - lastStatsUpdate > 2000)) {
    lastStatsUpdate = millis();
    int wifiClients = WiFiManager::getConnectedClients();
    int wsClients = WebSocketRelay::getClientCount();
    DisplayManager::showStatsScreen(wifiClients, wsClients, sdCardMounted, config, actualSSID);
  }
  
  // Show connected clients count (less frequent)
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 5000) { // Every 5 seconds
    lastUpdate = millis();
    
    int wifiClients = WiFiManager::getConnectedClients();
    int wsClients = WebSocketRelay::getClientCount();
    if (wifiClients > 0 || wsClients > 0) {
      Serial.printf("WiFi clients: %d | WebSocket clients: %d\n", wifiClients, wsClients);
    }
  }
  
  delay(10);
}
