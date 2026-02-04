#include "websocket_server.h"

WebSocketsServer WebSocketRelay::server(81);
std::map<uint8_t, PlayerClient> WebSocketRelay::clients;

void WebSocketRelay::onEvent(uint8_t clientNum, WStype_t type, uint8_t* payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      {
        Serial.printf("[WS] Client #%u disconnected\n", clientNum);
        
        // Remove from client registry
        if (clients.find(clientNum) != clients.end()) {
          String uuid = clients[clientNum].uuid;
          clients.erase(clientNum);
          Serial.printf("  Removed client with UUID: %s\n", uuid.c_str());
          Serial.printf("  Active clients: %d\n", clients.size());
          
          // Notify other clients about disconnect
          JsonDocument doc;
          doc["type"] = "player_disconnected";
          doc["uuid"] = uuid;
          doc["timestamp"] = millis();
          
          String message;
          serializeJson(doc, message);
          
          // Broadcast to all remaining clients
          for (auto& client : clients) {
            server.sendTXT(client.first, message);
          }
        }
      }
      break;
      
    case WStype_CONNECTED:
      {
        IPAddress ip = server.remoteIP(clientNum);
        Serial.printf("[WS] Client #%u connected from %s\n", clientNum, ip.toString().c_str());
        
        // Initialize client entry (UUID will be set when client sends it)
        PlayerClient newClient;
        newClient.uuid = "";
        newClient.lastSeen = millis();
        clients[clientNum] = newClient;
        
        // Send welcome message
        JsonDocument doc;
        doc["type"] = "connected";
        doc["message"] = "Welcome to LAN Party Arcade!";
        doc["clientNum"] = clientNum;
        doc["timestamp"] = millis();
        
        String message;
        serializeJson(doc, message);
        server.sendTXT(clientNum, message);
        
        Serial.printf("  Active clients: %d\n", clients.size());
      }
      break;
      
    case WStype_TEXT:
      {
        String message = String((char*)payload);
        Serial.printf("[WS] Client #%u sent: %s\n", clientNum, message.c_str());
        
        // Update last seen time
        if (clients.find(clientNum) != clients.end()) {
          clients[clientNum].lastSeen = millis();
        }
        
        // Try to parse as JSON to extract UUID
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, message);
        
        if (!error && doc["uuid"].is<String>()) {
          String uuid = doc["uuid"].as<String>();
          
          // Store/update UUID for this client
          if (clients.find(clientNum) != clients.end()) {
            if (clients[clientNum].uuid.length() == 0) {
              clients[clientNum].uuid = uuid;
              Serial.printf("  Registered UUID: %s\n", uuid.c_str());
            }
          }
        }
        
        // RELAY MODE: Broadcast to ALL other clients (pure relay)
        int relayCount = 0;
        for (auto& client : clients) {
          if (client.first != clientNum) { // Don't echo back to sender
            server.sendTXT(client.first, payload, length);
            relayCount++;
          }
        }
        
        Serial.printf("  Relayed to %d clients\n", relayCount);
      }
      break;
      
    case WStype_BIN:
      Serial.printf("[WS] Client #%u sent binary data (%u bytes) - ignored\n", clientNum, length);
      break;
      
    case WStype_ERROR:
      Serial.printf("[WS] Client #%u error\n", clientNum);
      break;
      
    case WStype_PING:
    case WStype_PONG:
      // Handled automatically by library
      break;
  }
}

bool WebSocketRelay::start(uint16_t port) {
  Serial.println("\n--- Starting WebSocket Server ---");
  
  server.begin();
  server.onEvent(onEvent);
  
  Serial.printf("WebSocket Server started on port %d\n", port);
  Serial.println("Ready for real-time connections!");
  
  return true;
}

void WebSocketRelay::process() {
  server.loop();
}

int WebSocketRelay::getClientCount() {
  return clients.size();
}

void WebSocketRelay::broadcastMessage(const String& message) {
  String msg = message; // Create mutable copy for WebSocket library
  for (auto& client : clients) {
    server.sendTXT(client.first, msg);
  }
}

void WebSocketRelay::stop() {
  server.close();
}
