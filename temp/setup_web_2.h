// Интерфейс веб-сервера для настройки

// Защита от повторного включения файла (include guard)
#ifndef SETUP_WEB_H // Проверка сущесвует ли тег
#define SETUP_WEB_H // Создание тега

// Зависимости
#include <ESP8266WebServer.h> // Веб-сервера для ESP8266

// Объявление глобального объекта веб-сервера с внешней линковкой
extern ESP8266WebServer server;

// Функция инициализации и запуска веб-сервера
void initSetupWebServer();
// Функция обработки входящих веб-запросов в основном цикле
void handleSetupWebRequests();

// Обработчик главной страницы настройки (/)
void handleSetupPage();
// Обработчик API-эндпоинта сканирования Wi-Fi сетей (/api/scan)
void handleScanNetworks();
// Обработчик сохранения настроек Wi-Fi (/api/save-wifi)
void handleSaveWiFi();
// Обработчик API-эндпоинта статуса устройства (/api/status)
void handleApiStatus();

#endif