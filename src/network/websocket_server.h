#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <map>

struct PlayerClient {
  String uuid;
  unsigned long lastSeen;
};

class WebSocketRelay {
public:
  // Start WebSocket server
  static bool start(uint16_t port = 81);
  
  // Process WebSocket events (call in loop)
  static void process();
  
  // Get number of connected clients
  static int getClientCount();
  
  // Broadcast message to all clients
  static void broadcastMessage(const String& message);
  
  // Stop WebSocket server
  static void stop();

private:
  static WebSocketsServer server;
  static std::map<uint8_t, PlayerClient> clients;
  
  // Event handler
  static void onEvent(uint8_t clientNum, WStype_t type, uint8_t* payload, size_t length);
};

#endif
