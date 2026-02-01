#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>

struct DeviceConfig {
  char wifiSSID[32];
  char wifiPassword[64];
  bool isConfigured;
};

extern DeviceConfig config;

void loadConfig();
void saveConfig();
bool isValidConfig();

#endif 