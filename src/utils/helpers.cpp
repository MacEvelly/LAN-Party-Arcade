#include "helpers.h"

String Helpers::formatUptime(unsigned long ms) {
  int seconds = (ms / 1000) % 60;
  int minutes = (ms / 60000) % 60;
  int hours = (ms / 3600000);
  
  char buffer[16];
  sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
  return String(buffer);
}

String Helpers::formatBytes(uint64_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + " B";
  } else if (bytes < 1024 * 1024) {
    return String(bytes / 1024) + " KB";
  } else if (bytes < 1024 * 1024 * 1024) {
    return String(bytes / (1024 * 1024)) + " MB";
  } else {
    return String(bytes / (1024 * 1024 * 1024)) + " GB";
  }
}

String Helpers::truncateString(const String& str, int maxLen) {
  if (str.length() <= maxLen) {
    return str;
  }
  return str.substring(0, maxLen - 2) + "..";
}
