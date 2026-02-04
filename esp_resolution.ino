#include "config_manager.h"
#include "setup_web.h"
#include "sensor_generator.h"  // Добавь эту строку
#include <ESP8266WiFi.h>
#include <LittleFS.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=== НАЧАЛО DIAGNOSTICS ===");
  
  // Тест 1: Проверка Serial
  Serial.println("1. Serial работает");
  
  // Тест 2: Проверка LittleFS
  Serial.println("2. Инициализация LittleFS...");
  bool fsOK = LittleFS.begin();
  Serial.println(fsOK ? "2. OK" : "2. ERROR");
  
  if (fsOK) {
    Serial.println("3. Проверка файла конфигурации...");
    bool fileExists = LittleFS.exists("/config.dat");
    Serial.println(fileExists ? "3. Файл существует" : "3. Файл не найден");
    LittleFS.end();
  }
  
  // Тест 3: Проверка Wi-Fi
  Serial.println("4. Инициализация Wi-Fi...");
  WiFi.mode(WIFI_OFF);
  delay(100);
  Serial.println("4. Wi-Fi готов");
  
  Serial.println("=== DIAGNOSTICS ЗАВЕРШЕНЫ ===");
  
  // Теперь основной код
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  randomSeed(analogRead(A0));
  initSetupWebServer();
}

void loop() {
  handleSetupWebRequests();
  
  // ОБЯЗАТЕЛЬНО: обновление данных датчиков
  updateSensorData();
  
  // Мигание светодиодом
  static unsigned long lastBlink = 0;
  static bool ledState = false;
  if (millis() - lastBlink > 1000) {
    lastBlink = millis();
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState ? LOW : HIGH);
  }
}