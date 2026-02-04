#include "config_manager.h"
#include "setup_web.h"
#include "sensor_generator.h"  // Добавь эту строку
#include <ESP8266WiFi.h>
#include <LittleFS.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=== ESP8266 Weather Station ===");
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  // Инициализация генератора случайных чисел
  randomSeed(analogRead(A0));
  
  loadConfig();
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