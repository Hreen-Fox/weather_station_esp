#include "config_manager.h"
#include <LittleFS.h>

// Инициализация с реалистичными значениями по умолчанию
SystemConfig config = {
  "",           // wifiSSID
  "",           // wifiPassword  
  1,            // maxClients
  5,            // updateInterval (5 секунд)
  false,        // firstSetupDone
  
  // Настройки генерации данных
  22.0,         // tempBase - базовая температура 22°C
  3.0,          // tempAmplitude - колебания ±3°C
  45.0,         // humBase - базовая влажность 45%
  10.0          // humAmplitude - колебания ±10%
};

void loadConfig() {
  if (!LittleFS.begin()) {
    return;
  }
  
  if (LittleFS.exists("/config.dat")) {
    File configFile = LittleFS.open("/config.dat", "r");
    if (configFile) {
      configFile.readBytes((char*)&config, sizeof(SystemConfig));
      configFile.close();
    }
  }
}

void saveConfig() {
  if (!LittleFS.begin()) {
    return;
  }
  
  File configFile = LittleFS.open("/config.dat", "w");
  if (configFile) {
    configFile.write((uint8_t*)&config, sizeof(SystemConfig));
    configFile.close();
  }
}

bool isFirstSetupDone() {
  return config.firstSetupDone;
}