//web_server.h - Заголовочный файл веб-сервера метеостанции

// Защита от повторного включения файла (include guard)
#ifndef SETUP_WEB_H // Проверка сущесвует ли тег
#define SETUP_WEB_H // Создание тега

// Зависимости
#include <ESP8266WebServer.h>
#include "web_pages.h"

// Объявление глобального объекта веб-сервера с внешней линковкой
extern ESP8266WebServer server;

// Функция инициализации и запуска веб-сервера
void initWebServer();
// Функция обработки входящих веб-запросов в основном цикле
void handleWebRequests();

#endif