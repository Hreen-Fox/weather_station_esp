#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>

struct SystemConfig {
  char wifiSSID[32];
  char wifiPassword[64];
  uint8_t maxClients;
  uint16_t updateInterval;     // Интервал обновления данных (секунды)
  bool firstSetupDone;
  
  // Настройки генерации данных датчиков
  float tempBase;              // Базовая температура (°C)
  float tempAmplitude;         // Амплитуда колебаний температуры
  float humBase;               // Базовая влажность (%)
  float humAmplitude;          // Амплитуда колебаний влажности
};

extern SystemConfig config;

void loadConfig();
void saveConfig();
bool isFirstSetupDone();

#endif