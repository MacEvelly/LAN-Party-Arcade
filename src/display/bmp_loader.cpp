#include "bmp_loader.h"

bool BMPLoader::draw(TFT_eSPI& tft, const char* filename, int16_t x, int16_t y) {
  File bmpFile = SD.open(filename);
  if (!bmpFile) {
    Serial.printf("Failed to open %s\n", filename);
    return false;
  }
  
  // Read BMP header (simple 24-bit uncompressed BMP reader)
  if (bmpFile.read() != 'B' || bmpFile.read() != 'M') {
    Serial.println("Not a BMP file");
    bmpFile.close();
    return false;
  }
  
  bmpFile.seek(10);
  uint32_t dataOffset = bmpFile.read() | (bmpFile.read() << 8) | 
                       (bmpFile.read() << 16) | (bmpFile.read() << 24);
  
  bmpFile.seek(18);
  int32_t width = bmpFile.read() | (bmpFile.read() << 8) | 
                 (bmpFile.read() << 16) | (bmpFile.read() << 24);
  int32_t height = bmpFile.read() | (bmpFile.read() << 8) | 
                  (bmpFile.read() << 16) | (bmpFile.read() << 24);
  
  bmpFile.seek(28);
  uint16_t depth = bmpFile.read() | (bmpFile.read() << 8);
  
  if (depth != 24) {
    Serial.printf("Unsupported bit depth: %d (need 24-bit BMP)\n", depth);
    bmpFile.close();
    return false;
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
  return true;
}

bool BMPLoader::validate(const char* filename) {
  File bmpFile = SD.open(filename);
  if (!bmpFile) {
    return false;
  }
  
  // Check BMP signature
  bool valid = (bmpFile.read() == 'B' && bmpFile.read() == 'M');
  bmpFile.close();
  
  return valid;
}
