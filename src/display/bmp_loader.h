#ifndef BMP_LOADER_H
#define BMP_LOADER_H

#include <TFT_eSPI.h>
#include <SD.h>

class BMPLoader {
public:
  // Draw BMP image from SD card at specified position
  // Supports 24-bit uncompressed BMP files
  static bool draw(TFT_eSPI& tft, const char* filename, int16_t x, int16_t y);
  
  // Validate if file is a proper BMP
  static bool validate(const char* filename);
};

#endif
