#ifndef QR_GENERATOR_H
#define QR_GENERATOR_H

#include <TFT_eSPI.h>
#include "qrcode.h"

class QRGenerator {
public:
  // Draw WiFi connection QR code
  static void drawWiFiQR(TFT_eSPI& tft, int x, int y, int moduleSize, 
                         const String& ssid, const String& password);
  
  // Draw URL QR code
  static void drawURLQR(TFT_eSPI& tft, int x, int y, int moduleSize, 
                        const String& url);
};

#endif
