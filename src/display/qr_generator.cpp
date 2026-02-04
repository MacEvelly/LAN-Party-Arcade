#include "qr_generator.h"

void QRGenerator::drawWiFiQR(TFT_eSPI& tft, int x, int y, int moduleSize, 
                              const String& ssid, const String& password) {
  // Generate WiFi QR code data
  String qrData = "WIFI:T:";
  qrData += (password.length() > 0) ? "WPA" : "nopass";
  qrData += ";S:" + ssid + ";";
  if (password.length() > 0) {
    qrData += "P:" + password + ";";
  }
  qrData += ";";
  
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcode, qrcodeData, 3, ECC_LOW, qrData.c_str());
  
  // Draw white border
  tft.fillRect(x - 4, y - 4, qrcode.size * moduleSize + 8, 
               qrcode.size * moduleSize + 8, TFT_WHITE);
  tft.fillRect(x - 2, y - 2, qrcode.size * moduleSize + 4, 
               qrcode.size * moduleSize + 4, TFT_BLACK);
  
  // Draw QR code modules
  for (uint8_t qy = 0; qy < qrcode.size; qy++) {
    for (uint8_t qx = 0; qx < qrcode.size; qx++) {
      if (qrcode_getModule(&qrcode, qx, qy)) {
        tft.fillRect(x + qx * moduleSize, y + qy * moduleSize, 
                    moduleSize, moduleSize, TFT_WHITE);
      }
    }
  }
}

void QRGenerator::drawURLQR(TFT_eSPI& tft, int x, int y, int moduleSize, 
                            const String& url) {
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcode, qrcodeData, 3, ECC_LOW, url.c_str());
  
  // Draw white border
  tft.fillRect(x - 4, y - 4, qrcode.size * moduleSize + 8, 
               qrcode.size * moduleSize + 8, TFT_WHITE);
  tft.fillRect(x - 2, y - 2, qrcode.size * moduleSize + 4, 
               qrcode.size * moduleSize + 4, TFT_BLACK);
  
  // Draw QR code modules
  for (uint8_t qy = 0; qy < qrcode.size; qy++) {
    for (uint8_t qx = 0; qx < qrcode.size; qx++) {
      if (qrcode_getModule(&qrcode, qx, qy)) {
        tft.fillRect(x + qx * moduleSize, y + qy * moduleSize, 
                    moduleSize, moduleSize, TFT_WHITE);
      }
    }
  }
}
