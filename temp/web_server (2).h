#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESP8266WebServer.h>

extern ESP8266WebServer server;

// Основные функции сервера
void initWebServer();
void handleWebRequests();
void stopWebServer(); // Для остановки сервера при переходе в режим STA

// Обработчики страниц
void handleStatus();
void handleAttributes();
void handleSensors();
void handleSettings();
void handleSetup(); // Специальная страница для первой настройки

// API эндпоинты
void handleApiStatus();
void handleApiSensorsList();
void handleApiSettings();
void handleApiSaveSetup(); // Сохранение настроек первой конфигурации

#endif