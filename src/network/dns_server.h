#ifndef DNS_SERVER_WRAPPER_H
#define DNS_SERVER_WRAPPER_H

#include <DNSServer.h>
#include <WiFi.h>

class DNSManager {
public:
  // Start DNS server with wildcard redirect
  static bool start(uint16_t port = 53);
  
  // Process DNS requests (call in loop)
  static void process();
  
  // Stop DNS server
  static void stop();

private:
  static DNSServer server;
};

#endif
