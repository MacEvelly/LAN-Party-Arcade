#include "sd_card.h"
#include <SPI.h>

bool SDCard::mounted = false;

bool SDCard::init(uint8_t cs_pin) {
  Serial.println("\nInitializing SD card...");
  
  // ESP32-2432S028 uses VSPI for SD card (default SPI pins)
  // MOSI=23, MISO=19, SCK=18, CS=5
  delay(100); // Give SD card time to power up
  
  if (SD.begin(cs_pin, SPI, 25000000)) {
    mounted = true;
    Serial.println("SD card mounted successfully!");
    
    uint64_t cardSize = getCardSizeMB();
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    
    return true;
  } else {
    mounted = false;
    Serial.println("SD card mount failed!");
    return false;
  }
}

bool SDCard::isMounted() {
  return mounted;
}

uint64_t SDCard::getCardSizeMB() {
  if (!mounted) return 0;
  return SD.cardSize() / (1024 * 1024);
}

bool SDCard::fileExists(const String& path) {
  if (!mounted) return false;
  return SD.exists(path);
}

File SDCard::openFile(const String& path, const char* mode) {
  if (!mounted) return File();
  return SD.open(path, mode);
}
