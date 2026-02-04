#include "web_server.h"
#include <SD.h>

WebServer HTTPServer::server(80);

void HTTPServer::setupRoutes() {
  // Captive portal detection endpoints - redirect to test page
  // (Currently commented out as per user preference)
  /*
  server.on("/generate_204", []() { // Android
    server.sendHeader("Location", "http://play.local/test.html", true);
    server.send(302, "text/plain", "");
  });
  
  server.on("/hotspot-detect.html", []() { // iOS/macOS
    server.sendHeader("Location", "http://play.local/test.html", true);
    server.send(302, "text/plain", "");
  });
  
  server.on("/connecttest.txt", []() { // Windows
    server.sendHeader("Location", "http://play.local/test.html", true);
    server.send(302, "text/plain", "");
  });
  */
  
  // Handle all other requests with file serving
  server.onNotFound(handleFileRequest);
  
  Serial.println("Web server routes configured");
}

String HTTPServer::getContentType(const String& filename) {
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

void HTTPServer::handleFileRequest() {
  String path = server.uri();
  
  Serial.printf("HTTP Request: %s\n", path.c_str());
  
  // Default to index.html for root
  if (path == "/" || path == "") {
    path = "/index.html";
  }
  
  // Check if SD card is available
  if (!SDCard::isMounted()) {
    server.send(503, "text/html", 
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
      
      server.streamFile(file, contentType);
      file.close();
    } else {
      server.send(500, "text/html", 
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
    
    server.send(404, "text/html", message);
    Serial.printf("  -> 404: File not found: %s\n", path.c_str());
  }
}

bool HTTPServer::start(uint16_t port) {
  Serial.println("\n--- Starting Web Server ---");
  
  setupRoutes();
  server.begin();
  
  Serial.printf("Web Server started on port %d\n", port);
  Serial.println("Ready to serve files from SD card!");
  
  return true;
}

void HTTPServer::process() {
  server.handleClient();
}

void HTTPServer::stop() {
  server.stop();
}
