// web_api.h - Заголовочный файл REST API метеостанции

// Защита от повторного включения файла (include guard)
#ifndef WEB_API_H
#define WEB_API_H

// Зависимости
#include <ESP8266WebServer.h>

// Объявление обработчиков API-эндпоинтов
void handleApiStatus();     // Обработчик системной информации и статуса подключения
void handleScanNetworks();  // Обработчик сканирования доступных Wi-Fi сетей
void handleSaveWiFi();      // Обработчик сохранения настроек Wi-Fi (POST-запрос)
void handleApiSensors();    // Обработчик данных датчиков (температура и влажность)
void handleApiConfig();     // Обработчик сохраненных настроек конфигурации
void handleConfigFile();    // Обработчик скачивания бинарного файла конфигурации

void handleApiSensorsList();
void handleApiSensorToggle();
void handleApiSensorDelete();
void handleApiSensorAdd();
void handleApiStats();
void handleApiResetConfig();
void handleApiFactoryReset();
void handleApiAppData();

#endif