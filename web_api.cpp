

// Зависимости
#include "web_api.h"
#include "config_manager.h"
#include "sensor_generator.h"
#include "sensor_manager.h"    // ← ДОБАВЬ ЭТУ СТРОКУ
#include <ESP8266WiFi.h>      // ← ДОБАВЬ ЭТУ СТРОКУ
#include <ESP8266WiFi.h>
#include <LittleFS.h>

// Объявление внешнего объекта веб-сервера
extern ESP8266WebServer server;

// Обработчик скачивания файла конфигурации
void handleConfigFile() {
  if (!LittleFS.begin()) {
    server.send(500, "text/plain", "LittleFS error");
    return;
  }
  if (LittleFS.exists("/config.dat")) {
    File file = LittleFS.open("/config.dat", "r");
    if (file) {
      // Отправка бинарного файла с MIME-типом для скачивания
      server.streamFile(file, "application/octet-stream");
      file.close();
    } else {
      server.send(404, "text/plain", "File not found");
    }
  } else {
    server.send(404, "text/plain", "Config file not exists");
  }
}

// Обработчик системной информации и статуса подключения
void handleApiStatus() {
  String json = "{";
  json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"uptime\":" + String(millis() / 1000) + ",";
  json += "\"ipAddressAP\":\"" + WiFi.softAPIP().toString() + "\",";
  if (WiFi.status() == WL_CONNECTED) {
    // Данные при успешном подключении к сети
    json += "\"wifiConnected\":true,";
    json += "\"ssid\":\"" + String(WiFi.SSID()) + "\",";
    json += "\"ipAddressSTA\":\"" + WiFi.localIP().toString() + "\",";
    json += "\"macAddressSTA\":\"" + WiFi.macAddress() + "\",";
    json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
    json += "\"channel\":" + String(WiFi.channel());
  } else {
    // Данные при отсутствии подключения
    json += "\"wifiConnected\":false,";
    json += "\"ssid\":\"\",";
    json += "\"ipAddressSTA\":\"--.--.--.--\",";
    json += "\"macAddressSTA\":\"--:--:--:--:--:--\",";
    json += "\"rssi\":0,";
    json += "\"channel\":0";
  }
  json += ",\"macAddressAP\":\"" + WiFi.softAPmacAddress() + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

// Обработчик сканирования Wi-Fi сетей
void handleScanNetworks() {
  int n = WiFi.scanNetworks();
  String json = "{\"networks\":[";
  for (int i = 0; i < n; i++) {
    if (i > 0) json += ",";
    json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + ",\"encryption\":" + String(WiFi.encryptionType(i)) + "}";
  }
  json += "]}";
  server.send(200, "application/json", json);
}

// Обработчик сохранения настроек Wi-Fi
void handleSaveWiFi() {
  String requestBody = server.arg("plain");
  if (requestBody.length() > 0) {
    // Поиск позиций SSID и пароля в JSON-строке
    int ssidStart = requestBody.indexOf("\"ssid\":\"");
    int ssidEnd = requestBody.indexOf("\"", ssidStart + 9);
    int passStart = requestBody.indexOf("\"password\":\"");
    int passEnd = requestBody.indexOf("\"", passStart + 13);

    // Валидация найденных позиций
    if (ssidStart != -1 && ssidEnd > ssidStart + 8 && passStart != -1 && passEnd > passStart + 12) {
      // Извлечение значений с правильными смещениями
      String ssid = requestBody.substring(ssidStart + 8, ssidEnd);
      String password = requestBody.substring(passStart + 12, passEnd);

      // Сохранение в конфигурацию
      ssid.toCharArray(config.wifiSSID, 32);
      password.toCharArray(config.wifiPassword, 64);
      config.firstSetupDone = true;
      config.maxClients = 1;
      config.updateInterval = 5;
      saveConfig();

      server.send(200, "application/json", "{\"success\":true}");
      return;
    }
  }
  server.send(200, "application/json", "{\"success\":false,\"message\":\"Недостаточно параметров\"}");
}

// Обработчик данных датчиков
void handleApiSensors() {
  updateSensorData(); // Обновление данных перед отправкой
  String json = getSensorDataJSON();
  server.send(200, "application/json", json);
}

// Обработчик сохраненных настроек
void handleApiConfig() {
  String json = "{";
  json += "\"wifiSSID\":\"" + String(config.wifiSSID) + "\",";
  json += "\"maxClients\":" + String(config.maxClients) + ",";
  json += "\"updateInterval\":" + String(config.updateInterval) + ",";
  json += "\"firstSetupDone\":" + String(config.firstSetupDone ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}

// НОВЫЕ API ЭНДПОИНТЫ ДЛЯ ДАТЧИКОВ

void handleApiSensorsList() {
  String json = getSensorsJSON();
  server.send(200, "application/json", json);
}

void handleApiSensorToggle() {
  String idStr = server.pathArg(0);
  uint8_t sensorId = idStr.toInt();
  
  for (int i = 0; i < sensorCount; i++) {
    if (sensors[i].id == sensorId) {
      bool enabled = server.arg("plain").indexOf("true") != -1;
      sensors[i].enabled = enabled;
      saveSensors();
      server.send(200, "application/json", "{\"success\":true}");
      return;
    }
  }
  server.send(404, "application/json", "{\"success\":false,\"message\":\"Sensor not found\"}");
}

void handleApiSensorDelete() {
  String idStr = server.pathArg(0);
  uint8_t sensorId = idStr.toInt();
  
  for (int i = 0; i < sensorCount; i++) {
    if (sensors[i].id == sensorId) {
      // Сдвигаем массив
      for (int j = i; j < sensorCount - 1; j++) {
        sensors[j] = sensors[j + 1];
      }
      sensorCount--;
      saveSensors();
      server.send(200, "application/json", "{\"success\":true}");
      return;
    }
  }
  server.send(404, "application/json", "{\"success\":false,\"message\":\"Sensor not found\"}");
}

void handleApiSensorAdd() {
  String requestBody = server.arg("plain");
  
  // Парсим JSON вручную
  int nameStart = requestBody.indexOf("\"name\":\"") + 8;
  int nameEnd = requestBody.indexOf("\"", nameStart);
  int unitStart = requestBody.indexOf("\"unit\":\"") + 8;
  int unitEnd = requestBody.indexOf("\"", unitStart);
  int minStart = requestBody.indexOf("\"minValue\":") + 11;
  int minEnd = requestBody.indexOf(",", minStart);
  if (minEnd == -1) minEnd = requestBody.indexOf("}", minStart);
  int maxStart = requestBody.indexOf("\"maxValue\":") + 11;
  int maxEnd = requestBody.indexOf(",", maxStart);
  if (maxEnd == -1) maxEnd = requestBody.indexOf("}", maxStart);
  
  if (nameStart > 8 && nameEnd > nameStart && 
      unitStart > 8 && unitEnd > unitStart &&
      minStart > 11 && minEnd > minStart &&
      maxStart > 11 && maxEnd > maxStart) {
    
    if (sensorCount < MAX_SENSORS) {
      sensors[sensorCount].id = sensorCount + 1;
      requestBody.substring(nameStart, nameEnd).toCharArray(sensors[sensorCount].name, 32);
      requestBody.substring(unitStart, unitEnd).toCharArray(sensors[sensorCount].unit, 16);
      sensors[sensorCount].minValue = requestBody.substring(minStart, minEnd).toFloat();
      sensors[sensorCount].maxValue = requestBody.substring(maxStart, maxEnd).toFloat();
      sensors[sensorCount].enabled = true;
      sensors[sensorCount].currentValue = (sensors[sensorCount].minValue + sensors[sensorCount].maxValue) / 2.0;
      sensors[sensorCount].lastUpdate = 0;
      sensorCount++;
      saveSensors();
      server.send(200, "application/json", "{\"success\":true}");
      return;
    }
  }
  server.send(400, "application/json", "{\"success\":false,\"message\":\"Invalid parameters\"}");
}

void handleApiStats() {
  String json = getSensorStatsJSON();
  server.send(200, "application/json", json);
}

void handleApiResetConfig() {
  config.wifiSSID[0] = '\0';
  config.wifiPassword[0] = '\0';
  config.firstSetupDone = false;
  saveConfig();
  server.send(200, "application/json", "{\"success\":true}");
}

void handleApiFactoryReset() {
  // Сброс конфигурации
  config.wifiSSID[0] = '\0';
  config.wifiPassword[0] = '\0';
  config.firstSetupDone = false;
  config.updateInterval = 5;
  saveConfig();
  
  // Сброс датчиков
  initDefaultSensors();
  saveSensors();
  
  server.send(200, "application/json", "{\"success\":true}");
}

// web_api.cpp
// web_api.cpp
void handleApiAppData() {
  String json = "{";
  
  updateAllSensors();
  
  bool first = true;
  for (int i = 0; i < sensorCount; i++) {
    if (sensors[i].enabled) {
      if (!first) json += ",";
      // Используем dbName вместо name!
      json += "\"" + String(sensors[i].dbName) + "\":" + String(sensors[i].currentValue, 1);
      first = false;
    }
  }
  
  json += "}";
  server.send(200, "application/json", json);
}