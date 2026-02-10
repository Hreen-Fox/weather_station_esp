#include "web_server.h"
#include "web_pages.h"
#include "web_api.h"
#include "config_manager.h"
#include "sensor_manager.h"
#include <ESP8266WiFi.h>
#include <LittleFS.h>

ESP8266WebServer server(80);

void initWebServer() {
  Serial.println("=== ИНИЦИАЛИЗАЦИЯ ВЕБ-СЕРВЕРА ===");
  
  loadConfig();
  loadSensors();
  
  if (strlen(config.wifiSSID) > 0) {
    Serial.print("Попытка подключения к: ");
    Serial.println(config.wifiSSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(config.wifiSSID, config.wifiPassword);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      Serial.print(".");
      delay(1000);
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n✅ Подключено!");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("\n❌ Не удалось подключиться, создаём AP");
      WiFi.mode(WIFI_AP);
      WiFi.softAP("ESP8266_Setup", "12345678");
      Serial.print("AP IP: ");
      Serial.println(WiFi.softAPIP());
    }
  } else {
    Serial.println("Создаём точку доступа для настройки...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP8266_Setup", "12345678");
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
  }
  
  // Маршруты страниц
  server.on("/", handleSetupPage);
  server.on("/sensors", handleSensorsPage);
  server.on("/statistics", handleStatisticsPage);
  server.on("/settings", handleSettingsPage);
  
  // API маршруты
  server.on("/api/status", handleApiStatus);
  server.on("/api/scan", handleScanNetworks);
  server.on("/api/save-wifi", HTTP_POST, handleSaveWiFi);
  server.on("/api/sensors", HTTP_GET, handleApiSensorsList);
  server.on("/api/sensors/:id/toggle", HTTP_POST, handleApiSensorToggle);
  server.on("/api/sensors/:id", HTTP_DELETE, handleApiSensorDelete);
  server.on("/api/sensors", HTTP_POST, handleApiSensorAdd);
  server.on("/api/stats", handleApiStats);
  server.on("/api/config", handleApiConfig);
  server.on("/api/reset-config", HTTP_POST, handleApiResetConfig);
  server.on("/api/factory-reset", HTTP_POST, handleApiFactoryReset);
  server.on("/config.dat", handleConfigFile);
server.on("/api/app/data", handleApiAppData); 
  
  server.begin();
  Serial.println("🌐 Веб-сервер запущен");
}

void handleWebRequests() {
  server.handleClient();
}