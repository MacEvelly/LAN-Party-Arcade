# main.cpp Splitting Strategy

## 📊 Current State Analysis

**File:** `src/main.cpp`
**Total Lines:** 879 lines
**Status:** Monolithic - all functionality in one file

### Code Breakdown by Functionality

| Section | Lines | % | Purpose |
|---------|-------|---|---------|
| Includes & Globals | 1-51 | 6% | Libraries, pins, objects, state variables |
| setup() | 52-177 | 14% | Initialize all subsystems |
| loop() | 178-234 | 6% | Main event loop (DNS, web, WS, touch, stats) |
| loadConfig() | 236-279 | 5% | SD card config.json parsing |
| createUniqueSSID() | 281-312 | 4% | WiFi SSID collision detection |
| startWiFiAP() | 314-346 | 4% | WiFi Access Point initialization |
| drawBmpFromSD() | 348-403 | 6% | BMP image rendering from SD |
| showConnectionScreen() | 404-543 | 16% | Display QR codes & connection info |
| showStatsScreen() | 545-657 | 13% | Display system stats screen |
| webSocketEvent() | 659-787 | 15% | WebSocket message relay logic |
| setupWebServer() | 789-815 | 3% | HTTP server route configuration |
| getContentType() | 817-829 | 1% | MIME type helper |
| handleFileRequest() | 831-879 | 6% | HTTP file serving from SD |

---

## 🎯 Proposed Module Structure

### **Module 1: Storage Manager**
**File:** `src/storage/sd_card.cpp` + `sd_card.h`
**Lines:** ~100 lines
**Purpose:** All SD card operations

```cpp
// sd_card.h
#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h>
#include <SD.h>

class SDCard {
public:
  static bool init(uint8_t cs_pin);
  static bool isMounted();
  static uint64_t getCardSize();
  static bool fileExists(const String& path);
  static File openFile(const String& path, const char* mode);
};

#endif
```

**Functions to move:**
- SD initialization logic (from `setup()`)
- `drawBmpFromSD()` → `SDCard::loadBMP()`
- SD card checking utilities

**Why separate:**
- Testable SD operations
- Reusable for other projects
- Clear boundary for storage concerns

---

### **Module 2: Configuration Manager**
**File:** `src/storage/config.cpp` + `config.h`
**Lines:** ~80 lines
**Purpose:** Config loading and validation

```cpp
// config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

struct SystemConfig {
  String wifiSSID = "LAN_Party_Arcade";
  String wifiPassword = "";
  String hostname = "play";
  String headerBMP = "Header.bmp";
  int maxConnections = 20;
};

class ConfigManager {
public:
  static bool loadFromSD(const char* path, SystemConfig& config);
  static void printConfig(const SystemConfig& config);
};

#endif
```

**Functions to move:**
- `loadConfig()` → `ConfigManager::loadFromSD()`
- JSON parsing logic
- Config struct definition

**Why separate:**
- Clear config schema
- Easy to add new config options
- Validation in one place

---

### **Module 3: WiFi Manager**
**File:** `src/network/wifi_manager.cpp` + `wifi_manager.h`
**Lines:** ~100 lines
**Purpose:** WiFi Access Point management

```cpp
// wifi_manager.h
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "storage/config.h"

class WiFiManager {
public:
  static bool startAccessPoint(const SystemConfig& config, String& outActualSSID);
  static String createUniqueSSID(const String& baseSSID);
  static int getConnectedClients();
  static IPAddress getIP();
};

#endif
```

**Functions to move:**
- `createUniqueSSID()` → `WiFiManager::createUniqueSSID()`
- `startWiFiAP()` → `WiFiManager::startAccessPoint()`
- WiFi initialization from `setup()`

**Why separate:**
- Network concerns isolated
- Can swap WiFi modes (AP/STA) easily
- Testable SSID collision logic

---

### **Module 4: DNS Server**
**File:** `src/network/dns_server.cpp` + `dns_server.h`
**Lines:** ~50 lines
**Purpose:** Wildcard DNS server

```cpp
// dns_server.h
#ifndef DNS_SERVER_WRAPPER_H
#define DNS_SERVER_WRAPPER_H

#include <DNSServer.h>
#include <WiFi.h>

class DNSManager {
private:
  static DNSServer server;
  
public:
  static bool start(uint16_t port);
  static void process();
  static void stop();
};

#endif
```

**Functions to move:**
- DNS initialization from `setup()`
- DNS processing from `loop()`
- Wildcard DNS configuration

**Why separate:**
- Single responsibility
- Easy to disable/enable
- Clear DNS logic

---

### **Module 5: HTTP Server**
**File:** `src/network/web_server.cpp` + `web_server.h`
**Lines:** ~120 lines
**Purpose:** HTTP file serving

```cpp
// web_server.h
#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WebServer.h>
#include "storage/sd_card.h"

class HTTPServer {
private:
  static WebServer server;
  
  static String getContentType(const String& filename);
  static void handleFileRequest();
  static void setupRoutes();
  
public:
  static bool start(uint16_t port);
  static void process();
  static void stop();
};

#endif
```

**Functions to move:**
- `setupWebServer()` → `HTTPServer::setupRoutes()`
- `handleFileRequest()` → `HTTPServer::handleFileRequest()`
- `getContentType()` → `HTTPServer::getContentType()`
- Captive portal routes (commented out)

**Why separate:**
- HTTP concerns isolated
- Easy to add new routes
- Can switch to AsyncWebServer later

---

### **Module 6: WebSocket Server**
**File:** `src/network/websocket_server.cpp` + `websocket_server.h`
**Lines:** ~150 lines
**Purpose:** Real-time message relay

```cpp
// websocket_server.h
#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <WebSocketsServer.h>
#include <map>
#include <ArduinoJson.h>

struct PlayerClient {
  String uuid;
  unsigned long lastSeen;
};

class WebSocketRelay {
private:
  static WebSocketsServer server;
  static std::map<uint8_t, PlayerClient> clients;
  
  static void onEvent(uint8_t clientNum, WStype_t type, uint8_t* payload, size_t length);
  
public:
  static bool start(uint16_t port);
  static void process();
  static int getClientCount();
  static void broadcastMessage(const String& message);
};

#endif
```

**Functions to move:**
- `webSocketEvent()` → `WebSocketRelay::onEvent()`
- `clients` map management
- WebSocket initialization from `setup()`
- Client tracking logic

**Why separate:**
- Core game relay logic isolated
- Easy to test message routing
- Can add features (rooms, auth) easily

---

### **Module 7: Display Manager**
**File:** `src/display/display.cpp` + `display.h`
**Lines:** ~250 lines
**Purpose:** TFT display rendering

```cpp
// display.h
#ifndef DISPLAY_H
#define DISPLAY_H

#include <TFT_eSPI.h>
#include "storage/config.h"

enum class Screen {
  CONNECTION,
  STATS
};

class DisplayManager {
private:
  static TFT_eSPI tft;
  static Screen currentScreen;
  
public:
  static void init();
  static void setBrightness(uint8_t level);
  static void showConnectionScreen(const SystemConfig& config, const String& actualSSID);
  static void showStatsScreen(int wifiClients, int wsClients, bool sdMounted);
  static void toggleScreen();
  static bool checkTouch(uint16_t& x, uint16_t& y);
};

#endif
```

**Functions to move:**
- Display initialization from `setup()`
- `showConnectionScreen()` → `DisplayManager::showConnectionScreen()`
- `showStatsScreen()` → `DisplayManager::showStatsScreen()`
- `updateDisplay()` → `DisplayManager::showConnectionScreen()`
- Touch handling logic from `loop()`

**Why separate:**
- Display concerns isolated
- Easy to swap display libraries
- Can add animations/transitions
- Touch logic centralized

---

### **Module 8: QR Code Generator**
**File:** `src/display/qr_generator.cpp` + `qr_generator.h`
**Lines:** ~80 lines
**Purpose:** QR code generation and rendering

```cpp
// qr_generator.h
#ifndef QR_GENERATOR_H
#define QR_GENERATOR_H

#include <TFT_eSPI.h>
#include "qrcode.h"

class QRGenerator {
public:
  static void drawWiFiQR(TFT_eSPI& tft, int x, int y, int size, 
                         const String& ssid, const String& password);
  static void drawURLQR(TFT_eSPI& tft, int x, int y, int size, 
                        const String& url);
};

#endif
```

**Functions to move:**
- QR code generation logic from `showConnectionScreen()`
- WiFi QR format (WIFI:T:...)
- URL QR format

**Why separate:**
- Reusable QR generation
- Can test QR formatting independently
- Easy to change QR library

---

### **Module 9: BMP Loader**
**File:** `src/display/bmp_loader.cpp` + `bmp_loader.h`
**Lines:** ~70 lines
**Purpose:** Load BMP images from SD

```cpp
// bmp_loader.h
#ifndef BMP_LOADER_H
#define BMP_LOADER_H

#include <TFT_eSPI.h>
#include <SD.h>

class BMPLoader {
public:
  static bool draw(TFT_eSPI& tft, const char* filename, int16_t x, int16_t y);
  static bool validate(const char* filename);
};

#endif
```

**Functions to move:**
- `drawBmpFromSD()` → `BMPLoader::draw()`
- BMP header parsing
- Pixel-by-pixel rendering

**Why separate:**
- Isolated image loading
- Can add JPEG/PNG later
- Testable without display

---

### **Module 10: Utilities**
**File:** `src/utils/helpers.cpp` + `helpers.h`
**Lines:** ~30 lines
**Purpose:** Shared utility functions

```cpp
// helpers.h
#ifndef HELPERS_H
#define HELPERS_H

#include <Arduino.h>

class Helpers {
public:
  static String getMIMEType(const String& filename);
  static String formatUptime(unsigned long ms);
  static String formatBytes(uint64_t bytes);
  static String truncateString(const String& str, int maxLen);
};

#endif
```

**Functions to move:**
- `getContentType()` → `Helpers::getMIMEType()`
- Uptime formatting from `showStatsScreen()`
- String truncation logic

**Why separate:**
- Reusable utilities
- No dependencies
- Easy to unit test

---

### **Module 11: Main (Orchestrator)**
**File:** `src/main.cpp`
**Lines:** ~150 lines (reduced from 879!)
**Purpose:** Initialize and coordinate all modules

```cpp
// main.cpp - NEW SIMPLIFIED VERSION
#include "storage/sd_card.h"
#include "storage/config.h"
#include "network/wifi_manager.h"
#include "network/dns_server.h"
#include "network/web_server.h"
#include "network/websocket_server.h"
#include "display/display.h"

SystemConfig config;
String actualSSID;

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== LAN Party Arcade ===");
  
  // 1. Initialize display
  DisplayManager::init();
  
  // 2. Initialize SD card
  bool sdMounted = SDCard::init(SD_CS);
  
  // 3. Load configuration
  if (sdMounted) {
    ConfigManager::loadFromSD("/config.json", config);
  }
  
  // 4. Start WiFi Access Point
  WiFiManager::startAccessPoint(config, actualSSID);
  
  // 5. Start DNS server
  DNSManager::start(53);
  
  // 6. Start web server
  HTTPServer::start(80);
  
  // 7. Start WebSocket server
  WebSocketRelay::start(81);
  
  // 8. Show connection screen
  DisplayManager::showConnectionScreen(config, actualSSID);
  
  Serial.println("=== Ready! ===");
}

void loop() {
  // Process network services
  DNSManager::process();
  HTTPServer::process();
  WebSocketRelay::process();
  
  // Handle touch input
  uint16_t x, y;
  if (DisplayManager::checkTouch(x, y)) {
    DisplayManager::toggleScreen();
  }
  
  // Update stats screen periodically
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 2000) {
    lastUpdate = millis();
    DisplayManager::showStatsScreen(
      WiFiManager::getConnectedClients(),
      WebSocketRelay::getClientCount(),
      SDCard::isMounted()
    );
  }
  
  delay(10);
}
```

---

## 📁 New File Structure

```
src/
├── main.cpp                    # Orchestrator (150 lines)
├── storage/
│   ├── sd_card.h
│   ├── sd_card.cpp            # SD card operations (100 lines)
│   ├── config.h
│   └── config.cpp             # Config loading (80 lines)
├── network/
│   ├── wifi_manager.h
│   ├── wifi_manager.cpp       # WiFi AP (100 lines)
│   ├── dns_server.h
│   ├── dns_server.cpp         # DNS wildcard (50 lines)
│   ├── web_server.h
│   ├── web_server.cpp         # HTTP file serving (120 lines)
│   ├── websocket_server.h
│   └── websocket_server.cpp   # WebSocket relay (150 lines)
├── display/
│   ├── display.h
│   ├── display.cpp            # Screen rendering (250 lines)
│   ├── qr_generator.h
│   ├── qr_generator.cpp       # QR code generation (80 lines)
│   ├── bmp_loader.h
│   └── bmp_loader.cpp         # BMP image loading (70 lines)
└── utils/
    ├── helpers.h
    └── helpers.cpp            # Utilities (30 lines)
```

**Total:** 1,180 lines (vs 879 in monolith)
- **Why more?** Headers, class definitions, better organization
- **Overhead:** ~34% more code for modularity
- **Worth it?** YES - maintainability >>> size

---

## 🎯 Benefits of Splitting

### **1. Maintainability**
- Find code instantly (no 879-line scrolling)
- Change one module without breaking others
- Clear responsibilities

### **2. Testability**
- Test WiFi logic without display
- Test display without network
- Mock dependencies easily

### **3. Reusability**
- Use `SDCard` class in other projects
- Share `WebSocketRelay` with community
- Extract `DisplayManager` for other ESP32 projects

### **4. Collaboration**
- Multiple people can work simultaneously
- Clear PR boundaries ("changed WebSocket relay")
- Easy code review (small files)

### **5. Scalability**
- Add new displays (LVGL) without touching network
- Swap WebServer → AsyncWebServer in one file
- Add authentication to WebSocket without touching display

---

## ⚠️ Considerations

### **Pros:**
✅ Much easier to navigate
✅ Testable components
✅ Reusable modules
✅ Clear dependencies
✅ Better for teams
✅ Industry standard

### **Cons:**
❌ More files to manage
❌ Headers need forward declarations
❌ Slight memory overhead (minimal)
❌ Need to understand module boundaries
❌ Initial refactor time investment

### **Mitigations:**
- PlatformIO handles includes automatically
- Create good documentation
- Use namespaces to avoid conflicts
- Keep headers small (inline simple functions)

---

## 🚀 Migration Strategy

### **Phase 1: Create structure (no logic changes)**
1. Create folder structure
2. Create empty header files
3. Add include guards
4. Define class interfaces

### **Phase 2: Move storage modules**
1. Extract `SDCard` class
2. Extract `ConfigManager`
3. Update main.cpp includes
4. Test: should compile and work identically

### **Phase 3: Move network modules**
1. Extract `WiFiManager`
2. Extract `DNSManager`
3. Extract `HTTPServer`
4. Extract `WebSocketRelay`
5. Test after each module

### **Phase 4: Move display modules**
1. Extract `DisplayManager`
2. Extract `QRGenerator`
3. Extract `BMPLoader`
4. Test

### **Phase 5: Create utilities**
1. Extract `Helpers`
2. Clean up main.cpp
3. Final testing

### **Phase 6: Optimization**
1. Remove duplicate code
2. Add error handling
3. Document each module
4. Add examples

---

## 📊 Estimated Effort

| Phase | Effort | Risk |
|-------|--------|------|
| 1. Structure | 1 hour | Low |
| 2. Storage | 2 hours | Low |
| 3. Network | 4 hours | Medium |
| 4. Display | 3 hours | Medium |
| 5. Utilities | 1 hour | Low |
| 6. Optimization | 2 hours | Low |
| **Total** | **13 hours** | **Medium** |

**Risk factors:**
- Static members need careful initialization
- Display and touch state management
- WebSocket client map migration
- Global variables → class members

---

## 🎯 Recommendation

**YES, split main.cpp** - The benefits far outweigh the costs.

**Start with:** Storage modules (lowest risk, clear boundaries)

**Order:** Storage → Network → Display → Utils

**Test after each module** - Don't move everything at once.

**Keep git commits small** - One module per commit for easy rollback.

---

## 🔄 Rollback Plan

If something breaks:
1. Each module is one git commit
2. Revert last commit: `git revert HEAD`
3. Fix issue in isolation
4. Re-apply with fixes

---

## 📝 Next Steps

1. **Review this document** - Discuss structure
2. **Agree on module boundaries** - Adjust if needed
3. **Create folder structure** - Empty folders + headers
4. **Start with storage** - Low risk, immediate benefit
5. **Test continuously** - Never break working code

**Ready to proceed?**
