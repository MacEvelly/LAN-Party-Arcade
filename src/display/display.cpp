#include "display.h"
#include "bmp_loader.h"
#include "qr_generator.h"
#include <WiFi.h>

TFT_eSPI DisplayManager::tft = TFT_eSPI();
Screen DisplayManager::currentScreen = Screen::CONNECTION;

void DisplayManager::init() {
  Serial.println("Initializing display...");
  
  // Initialize backlight
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, HIGH); // Turn on backlight
  
  // Initialize display
  tft.init();
  tft.setRotation(0); // Portrait mode (240x320)
  tft.fillScreen(TFT_BLACK);
  
  // Set touch calibration for ESP32-2432S028 (portrait mode)
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
}

void DisplayManager::setBrightness(uint8_t level) {
  analogWrite(BACKLIGHT_PIN, level);
}

void DisplayManager::showConnectionScreen(const SystemConfig& config, const String& actualSSID) {
  // Clear screen
  tft.fillScreen(TFT_BLACK);
  
  // Draw logo if available (200x64, centered at top)
  int headerY = 10;
  String headerPath = "/" + config.headerBMP;
  if (SDCard::fileExists(headerPath)) {
    int logoX = (240 - 200) / 2; // Center horizontally (20px)
    BMPLoader::draw(tft, headerPath.c_str(), logoX, 5);
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
  
  int qrSize = 3; // 3 pixels per module
  int qrX1 = 15; // Left position
  int qrY = headerY;
  
  // Generate URL for QR code
  String urlForQR = "http://" + config.hostname + ".local";
  
  // Draw WiFi QR code
  QRGenerator::drawWiFiQR(tft, qrX1, qrY, qrSize, actualSSID, config.wifiPassword);
  
  // ========== QR CODE 2: URL ==========
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(1);
  int labelY = headerY - 17; // Align with first label (before spacing was added)
  tft.setCursor(135, labelY);
  tft.println("2. Open URL");
  
  int qrX2 = 135; // Right position
  
  // Draw URL QR code
  QRGenerator::drawURLQR(tft, qrX2, qrY, qrSize, urlForQR);
  
  // Calculate next Y position (after QR codes)
  QRCode tempQR;
  uint8_t tempData[qrcode_getBufferSize(3)];
  qrcode_initText(&tempQR, tempData, 3, ECC_LOW, "test");
  int nextY = qrY + (tempQR.size * qrSize) + 8;
  
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
  if (config.wifiPassword.length() > 0) {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setCursor(15, nextY + 20);
    tft.println("Password:");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(15, nextY + 30);
    String displayPass = config.wifiPassword;
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
  String displayURL = config.hostname + ".local";
  if (displayURL.length() > 15) {
    displayURL = displayURL.substring(0, 13) + "..";
  }
  tft.println(displayURL);
}

void DisplayManager::showStatsScreen(int wifiClients, int wsClients, bool sdMounted,
                                     const SystemConfig& config, const String& actualSSID) {
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
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(15, y);
  tft.print("WiFi: ");
  tft.setTextColor(wifiClients > 0 ? TFT_GREEN : TFT_YELLOW, TFT_BLACK);
  tft.printf("%d / %d", wifiClients, config.maxConnections);
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
  if (sdMounted) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    uint64_t cardSize = SDCard::getCardSizeMB();
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

void DisplayManager::toggleScreen() {
  currentScreen = (currentScreen == Screen::CONNECTION) ? 
                  Screen::STATS : Screen::CONNECTION;
}

bool DisplayManager::checkTouch(uint16_t& x, uint16_t& y) {
  return tft.getTouch(&x, &y);
}

Screen DisplayManager::getCurrentScreen() {
  return currentScreen;
}
