#include "setup_web.h"
#include "config_manager.h"
#include "web_ui.h"  // ← Подключаем новый файл
#include <ESP8266WiFi.h>

ESP8266WebServer server(80);

String scanNetworksHTML() {
  String networks = "";
  int n = WiFi.scanNetworks();
  
  if (n == 0) {
    networks = "<p>Нет доступных сетей</p>";
  } else {
    for (int i = 0; i < n; i++) {
      networks += "<div class='network'>";
      networks += "<label><input type='radio' name='ssid' value='" + WiFi.SSID(i) + "'> ";
      networks += WiFi.SSID(i) + " (" + String(WiFi.RSSI(i)) + " dBm)</label>";
      networks += "</div>";
    }
  }
  return networks;
} 

// Генерация тестовых данных в реальном времени
String getRealTimeData() {
  String data = "{";
  
  float temperature = 20.0 + (sin(millis() / 10000.0) * 5);
  int humidity = 40 + (millis() / 60000) % 20;
  int pressure = 750 + random(-10, 11);
  
  data += "\"temperature\":" + String(temperature, 1) + ",";
  data += "\"humidity\":" + String(humidity) + ",";
  data += "\"pressure\":" + String(pressure) + ",";
  data += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
  data += "\"uptime\":" + String(millis() / 1000) + ",";
  data += "\"connectedDevices\":" + String(WiFi.softAPgetStationNum());
  
  data += "}";
  return data;
}

void handleRoot() {
  String html = String(SETUP_PAGE);
  html.replace("%NETWORKS%", scanNetworksHTML());
  server.send(200, "text/html", html);
}

void handleSave() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    
    ssid.toCharArray(config.wifiSSID, 32);
    password.toCharArray(config.wifiPassword, 64);
    config.isConfigured = true;
    saveConfig();
    
    server.send(200, "text/html", String(SUCCESS_PAGE));
    
    delay(3000);
    ESP.restart();
  } else {
    server.send(400, "text/plain", "Ошибка: недостаточно параметров");
  }
}

void initSetupWebServer() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP8266_Setup");
  
  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/api/data", HTTP_GET, []() {
    server.send(200, "application/json", getRealTimeData());
  });
  server.begin();
  
  Serial.println("🌐 Веб-сервер настройки запущен");
  Serial.println("Подключитесь к Wi-Fi: ESP8266_Setup");
  Serial.println("И откройте: http://192.168.4.1");
}

void handleSetupRequests() {
  server.handleClient();
}