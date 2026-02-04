#include "dns_server.h"

DNSServer DNSManager::server;

bool DNSManager::start(uint16_t port) {
  Serial.println("\n--- Starting DNS Server ---");
  
  server.start(port, "*", WiFi.softAPIP());
  
  Serial.printf("DNS Server started on port %d\n", port);
  Serial.println("Wildcard DNS: ANY domain -> " + WiFi.softAPIP().toString());
  
  return true;
}

void DNSManager::process() {
  server.processNextRequest();
}

void DNSManager::stop() {
  server.stop();
}
