#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WebServer.h>
#include "storage/sd_card.h"

class HTTPServer {
public:
  // Start HTTP server
  static bool start(uint16_t port = 80);
  
  // Process HTTP requests (call in loop)
  static void process();
  
  // Stop HTTP server
  static void stop();

private:
  static WebServer server;
  
  // Route handlers
  static void setupRoutes();
  static void handleFileRequest();
  
  // Helper functions
  static String getContentType(const String& filename);
};

#endif
