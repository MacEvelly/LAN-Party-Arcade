#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h>
#include <SD.h>

class SDCard {
public:
  // Initialize SD card
  static bool init(uint8_t cs_pin);
  
  // Check if SD card is mounted
  static bool isMounted();
  
  // Get card size in MB
  static uint64_t getCardSizeMB();
  
  // Check if file exists
  static bool fileExists(const String& path);
  
  // Open file for reading
  static File openFile(const String& path, const char* mode = FILE_READ);

private:
  static bool mounted;
};

#endif
