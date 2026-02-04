#ifndef HELPERS_H
#define HELPERS_H

#include <Arduino.h>

class Helpers {
public:
  // Format uptime in HH:MM:SS
  static String formatUptime(unsigned long ms);
  
  // Format bytes to human readable
  static String formatBytes(uint64_t bytes);
  
  // Truncate string to max length with ellipsis
  static String truncateString(const String& str, int maxLen);
};

#endif
