#include "config_manager.h"
#include <LittleFS.h>

DeviceConfig config = {"", "", false};

void loadConfig() {
  if (!LittleFS.begin()) {
    Serial.println("❌ LittleFS не инициализирован");
    return;
  }
  
  if (LittleFS.exists("/config.dat")) {
    File file = LittleFS.open("/config.dat", "r");
    if (file) {
      file.readBytes((char*)&config, sizeof(DeviceConfig));
      file.close();
      Serial.println("✅ Конфигурация загружена");
    }
  }
}

void saveConfig() {
  if (!LittleFS.begin()) return;
  
  File file = LittleFS.open("/config.dat", "w");
  if (file) {
    file.write((uint8_t*)&config, sizeof(DeviceConfig));
    file.close();
    Serial.println("💾 Конфигурация сохранена");
  }
}

bool isValidConfig() {
  return config.isConfigured && 
         strlen(config.wifiSSID) > 0 && 
         strlen(config.wifiPassword) > 0;
}