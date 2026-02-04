#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <map>
#include "qrcode.h"

// Pin definitions for ESP32-2432S028
#define SD_CS 5
#define TFT_BL 21

// Create display object
TFT_eSPI tft = TFT_eSPI();

// DNS Server for wildcard DNS
DNSServer dnsServer;
const byte DNS_PORT = 53;

// Web Server
WebServer webServer(80);

// WebSocket Server
WebSocketsServer wsServer(81);
struct PlayerClient {
  String uuid;
  unsigned long lastSeen;
};
std::map<uint8_t, PlayerClient> clients; // clientNum -> PlayerClient info

// System state
bool sdCardMounted = false;
String wifiSSID = "LAN_Party_Arcade";
String wifiPassword = "";
int maxConnections = 20;
String actualSSID = "";
String hostname = "play"; // Default hostname for mDNS
String headerBMP = "Header.bmp"; // Default header image filename
int currentScreen = 0; // 0 = connection info, 1 = system stats

// Function declarations
void loadConfig();
String createUniqueSSID();
void startWiFiAP();
void updateDisplay();
void showConnectionScreen();
void showStatsScreen();
void setupWebServer();
void handleFileRequest();
String getContentType(String filename);
void webSocketEvent(uint8_t clientNum, WStype_t type, uint8_t * payload, size_t length);

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n=== LAN Party Arcade ===");
  Serial.println("Phase 6 & 7: Polish & Display\n");

  // Initialize backlight
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH); // Turn on backlight

  // Initialize display
  Serial.println("Initializing display...");
  tft.init();
  tft.setRotation(0); // Portrait mode (240x320)
  tft.fillScreen(TFT_BLACK);
  
  // Set touch calibration for ESP32-2432S028 (portrait mode)
  // These values work for rotation 0 (portrait)
  uint16_t calData[5] = {275, 3620, 264, 3532, 1};
  tft.setTouch(calData);
  
  // Display header
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(30, 10);
  tft.println("LAN PARTY");
  tft.setCursor(40, 30);
  tft.println("ARCADE");
  
  tft.setTextSize(1);
  tft.setCursor(10, 55);
  tft.println("Phase 6: Polish & Display");
  
  Serial.println("Display initialized!");

  // Draw separator line
  tft.drawLine(10, 75, 230, 75, TFT_CYAN);

  // Initialize SD card
  Serial.println("\nInitializing SD card...");
  tft.setCursor(10, 85);
  tft.print("SD Card: ");
  
  // ESP32-2432S028 uses VSPI for SD card (default SPI pins)
  // MOSI=23, MISO=19, SCK=18, CS=5
  delay(100); // Give SD card time to power up
  
  if (SD.begin(SD_CS, SPI, 25000000)) {
    sdCardMounted = true;
    Serial.println("SD card mounted successfully!");
    
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("OK");
    
    // Get card info
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    
    // Load config if available
    loadConfig();
    
  } else {
    sdCardMounted = false;
    Serial.println("SD card mount failed!");
    
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.println("FAILED");
    
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setCursor(20, 105);
    tft.println("Using default config");
  }

  // Start WiFi Access Point
  Serial.println("\n--- Starting WiFi Access Point ---");
  startWiFiAP();
  
  // Start DNS Server
  Serial.println("\n--- Starting DNS Server ---");
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  Serial.printf("DNS Server started on port %d\n", DNS_PORT);
  Serial.println("Wildcard DNS: ANY domain -> " + WiFi.softAPIP().toString());
  
  // Start Web Server
  Serial.println("\n--- Starting Web Server ---");
  setupWebServer();
  webServer.begin();
  Serial.println("Web Server started on port 80");
  Serial.println("Ready to serve files from SD card!");
  
  // Start mDNS responder
  Serial.println("\n--- Starting mDNS ---");
  if (MDNS.begin(hostname.c_str())) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("mDNS responder started");
    Serial.printf("Access at: http://%s.local\n", hostname.c_str());
  } else {
    Serial.println("Error setting up mDNS responder!");
  }
  
  // Start WebSocket Server
  Serial.println("\n--- Starting WebSocket Server ---");
  wsServer.begin();
  wsServer.onEvent(webSocketEvent);
  Serial.println("WebSocket Server started on port 81");
  Serial.println("Ready for real-time connections!");
  
  // Update display with final connection info
  updateDisplay();
  
  Serial.println("\n=== Phase 6 & 7 Complete ===");
  Serial.println("Display: OK");
  Serial.printf("SD Card: %s\n", sdCardMounted ? "OK" : "FAILED");
  Serial.printf("WiFi AP: %s\n", actualSSID.c_str());
  Serial.printf("IP Address: %s\n", WiFi.softAPIP().toString().c_str());
  Serial.printf("Free RAM: %d KB\n", ESP.getFreeHeap() / 1024);
  Serial.println("Ready!");
}

void loop() {
  // Process DNS requests (wildcard DNS)
  dnsServer.processNextRequest();
  
  // Process web server requests
  webServer.handleClient();
  
  // Process WebSocket events
  wsServer.loop();
  
  // Check for touch input (simple detection - touch anywhere)
  static unsigned long lastTouchTime = 0;
  static bool wasTouched = false;
  uint16_t touchX = 0, touchY = 0;
  
  bool isTouched = tft.getTouch(&touchX, &touchY);
  
  // Detect touch press (rising edge)
  if (isTouched && !wasTouched && (millis() - lastTouchTime > 500)) {
    lastTouchTime = millis();
    wasTouched = true;
    
    Serial.printf("=== SCREEN TAP DETECTED at (%d, %d) ===\n", touchX, touchY);
    
    currentScreen = (currentScreen + 1) % 2; // Toggle between 0 and 1
    
    if (currentScreen == 0) {
      Serial.println("Showing: Connection Screen");
      showConnectionScreen();
    } else {
      Serial.println("Showing: Stats Screen");
      showStatsScreen();
    }
  }
  
  // Track release
  if (!isTouched) {
    wasTouched = false;
  }
  
  // Update stats screen if showing
  static unsigned long lastStatsUpdate = 0;
  if (currentScreen == 1 && (millis() - lastStatsUpdate > 2000)) {
    lastStatsUpdate = millis();
    showStatsScreen(); // Refresh stats
  }
  
  // Show connected clients count (less frequent)
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 5000) { // Every 5 seconds
    lastUpdate = millis();
    
    int wifiClients = WiFi.softAPgetStationNum();
    int wsClients = clients.size();
    if (wifiClients > 0 || wsClients > 0) {
      Serial.printf("WiFi clients: %d | WebSocket clients: %d\n", wifiClients, wsClients);
    }
  }
  
  delay(10);
}

// Load configuration from SD card
void loadConfig() {
  if (!sdCardMounted) return;
  
  Serial.println("\nChecking for config.json...");
  
  if (SD.exists("/config.json")) {
    File configFile = SD.open("/config.json", FILE_READ);
    if (configFile) {
      Serial.println("Found config.json, loading...");
      
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, configFile);
      
      if (!error) {
        if (doc["wifiSSID"].is<String>()) {
          wifiSSID = doc["wifiSSID"].as<String>();
          Serial.printf("  Custom SSID: %s\n", wifiSSID.c_str());
        }
        if (doc["wifiPassword"].is<String>()) {
          wifiPassword = doc["wifiPassword"].as<String>();
          Serial.printf("  Password: %s\n", wifiPassword.length() > 0 ? "***" : "(none)");
        }
        if (doc["maxConnections"].is<int>()) {
          maxConnections = doc["maxConnections"];
          Serial.printf("  Max connections: %d\n", maxConnections);
        }
        if (doc["hostname"].is<String>()) {
          hostname = doc["hostname"].as<String>();
          Serial.printf("  Hostname: %s.local\n", hostname.c_str());
        }
        if (doc["headerBMP"].is<String>()) {
          headerBMP = doc["headerBMP"].as<String>();
          Serial.printf("  Header BMP: %s\n", headerBMP.c_str());
        }
      } else {
        Serial.printf("JSON parse error: %s\n", error.c_str());
      }
      
      configFile.close();
    }
  } else {
    Serial.println("No config.json found, using defaults");
  }
}

// Create unique SSID by detecting collisions
String createUniqueSSID() {
  String baseSSID = wifiSSID;
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

// Start WiFi Access Point
void startWiFiAP() {
  // Set WiFi to AP mode
  WiFi.mode(WIFI_AP);
  delay(100);
  
  // Create unique SSID
  actualSSID = createUniqueSSID();
  
  // Start AP
  bool success;
  if (wifiPassword.length() > 0) {
    success = WiFi.softAP(actualSSID.c_str(), wifiPassword.c_str(), 1, 0, maxConnections);
    Serial.printf("Starting AP with password: %s\n", actualSSID.c_str());
  } else {
    success = WiFi.softAP(actualSSID.c_str(), NULL, 1, 0, maxConnections);
    Serial.printf("Starting open AP: %s\n", actualSSID.c_str());
  }
  
  if (success) {
    Serial.println("WiFi AP started successfully!");
    Serial.printf("  SSID: %s\n", actualSSID.c_str());
    Serial.printf("  IP: %s\n", WiFi.softAPIP().toString().c_str());
    Serial.printf("  Max clients: %d\n", maxConnections);
  } else {
    Serial.println("ERROR: Failed to start WiFi AP!");
  }
}

// BMP image loader from SD card
void drawBmpFromSD(const char* filename, int16_t x, int16_t y) {
  File bmpFile = SD.open(filename);
  if (!bmpFile) {
    Serial.printf("Failed to open %s\n", filename);
    return;
  }
  
  // Read BMP header (simple 24-bit uncompressed BMP reader)
  if (bmpFile.read() != 'B' || bmpFile.read() != 'M') {
    Serial.println("Not a BMP file");
    bmpFile.close();
    return;
  }
  
  bmpFile.seek(10);
  uint32_t dataOffset = bmpFile.read() | (bmpFile.read() << 8) | (bmpFile.read() << 16) | (bmpFile.read() << 24);
  
  bmpFile.seek(18);
  int32_t width = bmpFile.read() | (bmpFile.read() << 8) | (bmpFile.read() << 16) | (bmpFile.read() << 24);
  int32_t height = bmpFile.read() | (bmpFile.read() << 8) | (bmpFile.read() << 16) | (bmpFile.read() << 24);
  
  bmpFile.seek(28);
  uint16_t depth = bmpFile.read() | (bmpFile.read() << 8);
  
  if (depth != 24) {
    Serial.printf("Unsupported bit depth: %d (need 24-bit BMP)\n", depth);
    bmpFile.close();
    return;
  }
  
  // BMPs are stored bottom-to-top
  uint32_t rowSize = ((width * 3 + 3) & ~3); // Row size padded to 4 bytes
  uint8_t row[rowSize];
  
  bmpFile.seek(dataOffset);
  for (int row_idx = height - 1; row_idx >= 0; row_idx--) {
    bmpFile.seek(dataOffset + row_idx * rowSize);
    bmpFile.read(row, rowSize);
    
    for (int col = 0; col < width; col++) {
      uint8_t b = row[col * 3];
      uint8_t g = row[col * 3 + 1];
      uint8_t r = row[col * 3 + 2];
      uint16_t color = tft.color565(r, g, b);
      tft.drawPixel(x + col, y + (height - 1 - row_idx), color);
    }
  }
  
  bmpFile.close();
}

// Update display with connection information
void updateDisplay() {
  showConnectionScreen();
}

// Show connection info screen
void showConnectionScreen() {
  // Clear screen
  tft.fillScreen(TFT_BLACK);
  
  // Draw logo if available (200x64, centered at top)
  int headerY = 10;
  String headerPath = "/" + headerBMP;
  if (SD.exists(headerPath.c_str())) {
    int logoX = (240 - 200) / 2; // Center horizontally (20px)
    drawBmpFromSD(headerPath.c_str(), logoX, 5);
    headerY = 75; // Move content below logo
  } else {
    // Fallback text header if no logo
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(20, 10);
    tft.println("JOIN GAME");
    headerY = 35;
  }
  
  // Draw separator
  tft.drawLine(10, headerY, 230, headerY, TFT_CYAN);
  headerY += 5;
  
  // ========== QR CODE 1: WiFi Connection ==========
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(15, headerY);
  tft.println("1. Join WiFi");
  headerY += 17; // Space after label (12 + 5 extra)
  
  // Generate WiFi QR code
  String qrDataWifi = "WIFI:T:";
  qrDataWifi += (wifiPassword.length() > 0) ? "WPA" : "nopass";
  qrDataWifi += ";S:" + actualSSID + ";";
  if (wifiPassword.length() > 0) {
    qrDataWifi += "P:" + wifiPassword + ";";
  }
  qrDataWifi += ";";
  
  QRCode qrcodeWifi;
  uint8_t qrcodeDataWifi[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcodeWifi, qrcodeDataWifi, 3, ECC_LOW, qrDataWifi.c_str());
  
  // Draw WiFi QR code (left side, smaller)
  int qrSize = 3; // 3 pixels per module (smaller to fit both)
  int qrX1 = 15; // Left position
  int qrY = headerY;
  
  // White border for WiFi QR
  tft.fillRect(qrX1 - 4, qrY - 4, qrcodeWifi.size * qrSize + 8, qrcodeWifi.size * qrSize + 8, TFT_WHITE);
  tft.fillRect(qrX1 - 2, qrY - 2, qrcodeWifi.size * qrSize + 4, qrcodeWifi.size * qrSize + 4, TFT_BLACK);
  
  for (uint8_t y = 0; y < qrcodeWifi.size; y++) {
    for (uint8_t x = 0; x < qrcodeWifi.size; x++) {
      if (qrcode_getModule(&qrcodeWifi, x, y)) {
        tft.fillRect(qrX1 + x * qrSize, qrY + y * qrSize, qrSize, qrSize, TFT_WHITE);
      }
    }
  }
  
  // ========== QR CODE 2: URL ==========
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(1);
  int labelY = headerY - 17; // Align with first label (before spacing was added)
  tft.setCursor(135, labelY);
  tft.println("2. Open URL");
  
  // Generate URL QR code
  String qrDataURL = "http://" + hostname + ".local";
  
  QRCode qrcodeURL;
  uint8_t qrcodeDataURL[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcodeURL, qrcodeDataURL, 3, ECC_LOW, qrDataURL.c_str());
  
  // Draw URL QR code (right side)
  int qrX2 = 135; // Right position
  
  // White border for URL QR
  tft.fillRect(qrX2 - 4, qrY - 4, qrcodeURL.size * qrSize + 8, qrcodeURL.size * qrSize + 8, TFT_WHITE);
  tft.fillRect(qrX2 - 2, qrY - 2, qrcodeURL.size * qrSize + 4, qrcodeURL.size * qrSize + 4, TFT_BLACK);
  
  for (uint8_t y = 0; y < qrcodeURL.size; y++) {
    for (uint8_t x = 0; x < qrcodeURL.size; x++) {
      if (qrcode_getModule(&qrcodeURL, x, y)) {
        tft.fillRect(qrX2 + x * qrSize, qrY + y * qrSize, qrSize, qrSize, TFT_WHITE);
      }
    }
  }
  
  int nextY = qrY + (qrcodeWifi.size * qrSize) + 8;
  
  // WiFi details under left QR code
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(15, nextY);
  tft.println("SSID:");
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(15, nextY + 10);
  String displaySSID = actualSSID;
  if (displaySSID.length() > 15) {
    displaySSID = displaySSID.substring(0, 13) + "..";
  }
  tft.println(displaySSID);
  
  // Only show password line if there is one
  if (wifiPassword.length() > 0) {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setCursor(15, nextY + 20);
    tft.println("Password:");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(15, nextY + 30);
    String displayPass = wifiPassword;
    if (displayPass.length() > 15) {
      displayPass = displayPass.substring(0, 13) + "..";
    }
    tft.println(displayPass);
  }
  
  // URL details under right QR code
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setCursor(135, nextY);
  tft.println("URL:");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(135, nextY + 10);
  String displayURL = hostname + ".local";
  if (displayURL.length() > 15) {
    displayURL = displayURL.substring(0, 13) + "..";
  }
  tft.println(displayURL);
}

// Show system stats screen
void showStatsScreen() {
  // Clear screen
  tft.fillScreen(TFT_BLACK);
  
  // Header
  tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(20, 10);
  tft.println("SYSTEM");
  tft.setCursor(35, 30);
  tft.println("STATUS");
  
  // Draw separator
  tft.drawLine(10, 55, 230, 55, TFT_MAGENTA);
  
  int y = 70;
  
  // WiFi Status
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, y);
  tft.println("WiFi Access Point:");
  y += 15;
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(15, y);
  tft.print("SSID: ");
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  
  String displaySSID = actualSSID;
  if (displaySSID.length() > 16) {
    displaySSID = displaySSID.substring(0, 13) + "...";
  }
  tft.println(displaySSID);
  y += 12;
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(15, y);
  tft.print("IP: ");
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println(WiFi.softAPIP().toString());
  y += 20;
  
  // Connections
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setCursor(10, y);
  tft.println("Connections:");
  y += 15;
  
  int wifiClients = WiFi.softAPgetStationNum();
  int wsClients = clients.size();
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(15, y);
  tft.print("WiFi: ");
  tft.setTextColor(wifiClients > 0 ? TFT_GREEN : TFT_YELLOW, TFT_BLACK);
  tft.printf("%d / %d", wifiClients, maxConnections);
  y += 12;
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(15, y);
  tft.print("WebSocket: ");
  tft.setTextColor(wsClients > 0 ? TFT_GREEN : TFT_YELLOW, TFT_BLACK);
  tft.println(wsClients);
  y += 20;
  
  // Memory
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setCursor(10, y);
  tft.println("Memory:");
  y += 15;
  
  uint32_t freeHeap = ESP.getFreeHeap();
  uint32_t totalHeap = ESP.getHeapSize();
  uint32_t usedHeap = totalHeap - freeHeap;
  float heapPercent = (float)usedHeap / totalHeap * 100;
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(15, y);
  tft.print("Free: ");
  tft.setTextColor(freeHeap < 50000 ? TFT_RED : TFT_GREEN, TFT_BLACK);
  tft.printf("%d KB", freeHeap / 1024);
  y += 12;
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(15, y);
  tft.print("Used: ");
  tft.setTextColor(heapPercent > 80 ? TFT_RED : TFT_YELLOW, TFT_BLACK);
  tft.printf("%.1f%%", heapPercent);
  y += 20;
  
  // Storage
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setCursor(10, y);
  tft.println("Storage:");
  y += 15;
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(15, y);
  tft.print("SD Card: ");
  if (sdCardMounted) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    tft.printf("%lluMB", cardSize);
  } else {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.println("FAILED");
  }
  y += 20;
  
  // Uptime
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setCursor(10, y);
  tft.println("Uptime:");
  y += 15;
  
  unsigned long uptimeMs = millis();
  int seconds = (uptimeMs / 1000) % 60;
  int minutes = (uptimeMs / 60000) % 60;
  int hours = (uptimeMs / 3600000);
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(15, y);
  tft.printf("%02d:%02d:%02d", hours, minutes, seconds);
  
  // Footer
  y = 295;
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, y);
  tft.println("All Systems: OK");
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(10, y + 12);
  tft.print("Tap for connection info");
}

// WebSocket event handler
void webSocketEvent(uint8_t clientNum, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      {
        Serial.printf("[WS] Client #%u disconnected\n", clientNum);
        
        // Remove from client registry
        if (clients.find(clientNum) != clients.end()) {
          String uuid = clients[clientNum].uuid;
          clients.erase(clientNum);
          Serial.printf("  Removed client with UUID: %s\n", uuid.c_str());
          Serial.printf("  Active clients: %d\n", clients.size());
          
          // Notify other clients about disconnect
          JsonDocument doc;
          doc["type"] = "player_disconnected";
          doc["uuid"] = uuid;
          doc["timestamp"] = millis();
          
          String message;
          serializeJson(doc, message);
          
          // Broadcast to all remaining clients
          for (auto& client : clients) {
            wsServer.sendTXT(client.first, message);
          }
        }
      }
      break;
      
    case WStype_CONNECTED:
      {
        IPAddress ip = wsServer.remoteIP(clientNum);
        Serial.printf("[WS] Client #%u connected from %s\n", clientNum, ip.toString().c_str());
        
        // Initialize client entry (UUID will be set when client sends it)
        PlayerClient newClient;
        newClient.uuid = "";
        newClient.lastSeen = millis();
        clients[clientNum] = newClient;
        
        // Send welcome message
        JsonDocument doc;
        doc["type"] = "connected";
        doc["message"] = "Welcome to LAN Party Arcade!";
        doc["clientNum"] = clientNum;
        doc["timestamp"] = millis();
        
        String message;
        serializeJson(doc, message);
        wsServer.sendTXT(clientNum, message);
        
        Serial.printf("  Active clients: %d\n", clients.size());
      }
      break;
      
    case WStype_TEXT:
      {
        String message = String((char*)payload);
        Serial.printf("[WS] Client #%u sent: %s\n", clientNum, message.c_str());
        
        // Update last seen time
        if (clients.find(clientNum) != clients.end()) {
          clients[clientNum].lastSeen = millis();
        }
        
        // Try to parse as JSON to extract UUID
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, message);
        
        if (!error && doc["uuid"].is<String>()) {
          String uuid = doc["uuid"].as<String>();
          
          // Store/update UUID for this client
          if (clients.find(clientNum) != clients.end()) {
            if (clients[clientNum].uuid.length() == 0) {
              clients[clientNum].uuid = uuid;
              Serial.printf("  Registered UUID: %s\n", uuid.c_str());
            }
          }
        }
        
        // RELAY MODE: Broadcast to ALL other clients (pure relay)
        int relayCount = 0;
        for (auto& client : clients) {
          if (client.first != clientNum) { // Don't echo back to sender
            wsServer.sendTXT(client.first, payload, length);
            relayCount++;
          }
        }
        
        Serial.printf("  Relayed to %d clients\n", relayCount);
      }
      break;
      
    case WStype_BIN:
      Serial.printf("[WS] Client #%u sent binary data (%u bytes) - ignored\n", clientNum, length);
      break;
      
    case WStype_ERROR:
      Serial.printf("[WS] Client #%u error\n", clientNum);
      break;
      
    case WStype_PING:
      // Handled automatically by library
      break;
      
    case WStype_PONG:
      // Handled automatically by library
      break;
  }
}

// Setup web server routes
void setupWebServer() {
  // Captive portal detection endpoints - redirect to test page
  /*
  webServer.on("/generate_204", []() { // Android
    webServer.sendHeader("Location", "http://play.local/test.html", true);
    webServer.send(302, "text/plain", "");
  });
  
  webServer.on("/hotspot-detect.html", []() { // iOS/macOS
    webServer.sendHeader("Location", "http://play.local/test.html", true);
    webServer.send(302, "text/plain", "");
  });
  
  webServer.on("/connecttest.txt", []() { // Windows
    webServer.sendHeader("Location", "http://play.local/test.html", true);
    webServer.send(302, "text/plain", "");
  });
  */
  // Handle all other requests with file serving
  webServer.onNotFound(handleFileRequest);
  
  Serial.println("Web server routes configured");
}

// Get MIME type based on file extension
String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".json")) return "application/json";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".svg")) return "image/svg+xml";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

// Handle file requests from SD card
void handleFileRequest() {
  String path = webServer.uri();
  
  Serial.printf("HTTP Request: %s\n", path.c_str());
  
  // Default to index.html for root
  if (path == "/" || path == "") {
    path = "/index.html";
  }
  
  // Check if SD card is available
  if (!sdCardMounted) {
    webServer.send(503, "text/html", 
      "<html><body><h1>SD Card Error</h1><p>SD card not available</p></body></html>");
    Serial.println("  -> 503: SD card not available");
    return;
  }
  
  // Try to open file
  if (SD.exists(path)) {
    File file = SD.open(path, FILE_READ);
    
    if (file) {
      String contentType = getContentType(path);
      size_t fileSize = file.size();
      
      Serial.printf("  -> 200: Serving %s (%d bytes, %s)\n", 
                    path.c_str(), fileSize, contentType.c_str());
      
      webServer.streamFile(file, contentType);
      file.close();
    } else {
      webServer.send(500, "text/html", 
        "<html><body><h1>File Error</h1><p>Could not open file</p></body></html>");
      Serial.println("  -> 500: Could not open file");
    }
  } else {
    // File not found - send 404
    String message = "<html><body style='font-family: Arial; padding: 20px;'>";
    message += "<h1>404 - Not Found</h1>";
    message += "<p>File not found: <code>" + path + "</code></p>";
    message += "<p>Make sure files are on the SD card</p>";
    message += "</body></html>";
    
    webServer.send(404, "text/html", message);
    Serial.printf("  -> 404: File not found: %s\n", path.c_str());
  }
}