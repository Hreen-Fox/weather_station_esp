#include "config_manager.h"
#include <LittleFS.h>

// Инициализация глобальной переменной с настройками по умолчанию
SystemConfig config = {
  "",           // wifiSSID
  "",           // wifiPassword  
  1,            // maxClients
  5,            // updateInterval
  false         // firstSetupDone
};

/**
 * Загрузка конфигурации из файловой системы
 * Если файл не существует, возвращаются настройки по умолчанию
 */
void loadConfig() {
  // Инициализация LittleFS если еще не сделана
  if (!LittleFS.begin()) {
    Serial.println("❌ Не удалось инициализировать LittleFS");
    return;
  }
  
  // Проверяем существование файла конфигурации
  if (LittleFS.exists("/config.dat")) {
    File configFile = LittleFS.open("/config.dat", "r");
    if (configFile) {
      // Читаем структуру из файла
      configFile.readBytes((char*)&config, sizeof(SystemConfig));
      configFile.close();
      Serial.println("✅ Конфигурация загружена");
    }
  } else {
    Serial.println("ℹ️ Файл конфигурации не найден (первая настройка)");
    // Оставляем настройки по умолчанию
    config.firstSetupDone = false;
  }
}

/**
 * Сохранение конфигурации в файловую систему
 */
void saveConfig() {
  if (!LittleFS.begin()) return;
  
  File configFile = LittleFS.open("/config.dat", "w");
  if (configFile) {
    // Записываем структуру в файл
    configFile.write((uint8_t*)&config, sizeof(SystemConfig));
    configFile.close();
    Serial.println("💾 Конфигурация сохранена");
  }
}

/**
 * Проверка, была ли выполнена первоначальная настройка
 * @return true если настройка выполнена, false если требуется первая настройка
 */
bool isFirstSetupDone() {
  return config.firstSetupDone;
}