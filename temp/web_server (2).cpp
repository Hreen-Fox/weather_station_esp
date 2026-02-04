#include "web_server.h"
#include "config_manager.h"
#include <ESP8266WiFi.h>
#include <LittleFS.h>

ESP8266WebServer server(80);

// HTML для страницы первой настройки
const char SETUP_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>🔧 Первоначальная настройка</title>
    <style>
        body{font-family:sans-serif;padding:20px;background:#f5f5f5}
        .container{max-width:500px;margin:0 auto;background:white;padding:20px;border-radius:10px}
        h2{color:#1a73e8;text-align:center}
        .form-group{margin:15px 0}
        label{display:block;margin-bottom:5px;font-weight:bold}
        input{width:100%;padding:10px;border:1px solid #ddd;border-radius:5px}
        button{width:100%;padding:12px;background:#1a73e8;color:white;border:none;border-radius:5px;cursor:pointer}
        .network{padding:10px;margin:10px 0;background:#e8f4fd;border-radius:5px}
    </style>
</head>
<body>
    <div class='container'>
        <h2>🔧 Первоначальная настройка</h2>
        <p>Настройте подключение к вашему Wi-Fi роутеру:</p>
        <form method='POST' action='/api/save-setup'>
            <div class='form-group'>
                <label>Имя сети (SSID):</label>
                <input type='text' name='ssid' placeholder='Введите SSID' required>
            </div>
            <div class='form-group'>
                <label>Пароль:</label>
                <input type='password' name='password' placeholder='Введите пароль' required>
            </div>
            <button type='submit'>💾 Сохранить и перезагрузить</button>
        </form>
    </div>
</body>
</html>
)rawliteral";

// Тестовые данные для датчиков
String getSensorDataJSON() {
  String json = "{\"temperature\":22.5,\"humidity\":45,\"pressure\":755}";
  return json;
}

void handleSetup() {
  server.send_P(200, "text/html", SETUP_PAGE);
}

void handleStatus() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Статус</title></head><body>";
  html += "<h1>Статус устройства</h1>";
  html += "<p>RAM: " + String(ESP.getFreeHeap()) + " байт свободно</p>";
  html += "<p>Время работы: " + String(millis()/1000) + " сек</p>";
  html += "<p>Датчики: " + getSensorDataJSON() + "</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleAttributes() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Характеристики</title></head><body>";
  html += "<h1>Характеристики ESP8266</h1>";
  html += "<p>Микроконтроллер: ESP8266EX</p>";
  html += "<p>RAM: 80 КБ</p>";
  html += "<p>Flash: 4 МБ</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleSensors() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Датчики</title></head><body>";
  html += "<h1>Датчики</h1>";
  html += "<p>Температура: 22.5°C</p>";
  html += "<p>Влажность: 45%</p>";
  html += "<p>Давление: 755 мм рт.ст.</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleSettings() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Настройки</title></head><body>";
  html += "<h1>Настройки</h1>";
  html += "<p>SSID: " + String(config.wifiSSID) + "</p>";
  html += "<p>Макс. клиентов: " + String(config.maxClients) + "</p>";
  html += "<p>Интервал: " + String(config.updateInterval) + " сек</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleApiStatus() {
  String json = "{";
  json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"uptime\":" + String(millis() / 1000) + ",";
  json += "\"sensors\":" + getSensorDataJSON();
  json += "}";
  server.send(200, "application/json", json);
}

void handleApiSensorsList() {
  String json = "{\"sensors\":[{\"id\":1,\"name\":\"Температура\",\"enabled\":true}]}";
  server.send(200, "application/json", json);
}

void handleApiSettings() {
  String json = "{";
  json += "\"ssid\":\"" + String(config.wifiSSID) + "\",";
  json += "\"maxClients\":" + String(config.maxClients) + ",";
  json += "\"updateInterval\":" + String(config.updateInterval) + ",";
  json += "\"firstSetupDone\":" + String(config.firstSetupDone ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}

void handleApiSaveSetup() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    
    // Сохраняем настройки
    ssid.toCharArray(config.wifiSSID, 32);
    password.toCharArray(config.wifiPassword, 64);
    config.firstSetupDone = true;
    config.maxClients = 1;
    config.updateInterval = 5;
    saveConfig();
    
    server.send(200, "text/html", 
      "<html><body style='font-family:Arial;text-align:center;padding:50px;'>"
      "<h2>✅ Настройки сохранены!</h2>"
      "<p>Устройство перезагрузится через 3 секунды...</p>"
      "</body></html>");
    
    delay(3000);
    ESP.restart();
  } else {
    server.send(400, "text/plain", "Ошибка: недостаточно параметров");
  }
}

void initWebServer() {
  // Останавливаем текущий сервер если он запущен
  server.stop();
  
  if (!isFirstSetupDone()) {
    // Режим первой настройки - только страница настройки
    server.on("/", handleSetup);
    server.on("/api/save-setup", HTTP_POST, handleApiSaveSetup);
  } else {
    // Нормальный режим - все страницы
    server.on("/", handleStatus);
    server.on("/attributes", handleAttributes);
    server.on("/sensors", handleSensors);
    server.on("/settings", handleSettings);
    server.on("/api/status", handleApiStatus);
    server.on("/api/sensors/list", handleApiSensorsList);
    server.on("/api/settings", handleApiSettings);
  }
  
  server.begin();
  Serial.println("🌐 Веб-сервер запущен");
}

void stopWebServer() {
  server.stop();
  Serial.println("⏹️ Веб-сервер остановлен");
}

void handleWebRequests() {
  server.handleClient();
}