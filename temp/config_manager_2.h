#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>

// Структура системных настроек
struct SystemConfig {
  char wifiSSID[32];           // SSID домашнего роутера
  char wifiPassword[64];       // Пароль от роутера
  uint8_t maxClients;          // Макс. клиентов к AP (1 по умолчанию)
  uint16_t updateInterval;     // Интервал обновления данных (сек)
  bool firstSetupDone;         // Флаг завершения первоначальной настройки
};

// Глобальная переменная конфигурации
extern SystemConfig config;

// Функции управления конфигурацией
void loadConfig();
void saveConfig();
bool isFirstSetupDone();

#endif