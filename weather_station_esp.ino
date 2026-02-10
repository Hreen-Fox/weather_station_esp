// Просто главный исполняемый файл

// Зависимости
#include "config_manager.h"
#include "sensor_manager.h"
#include "sensor_generator.h"
#include "web_server.h"
#include "web_pages.h" 

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== ESP8266 Weather Station ===");
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  randomSeed(analogRead(A0));
  
  loadConfig();
  loadSensors(); // НОВАЯ СТРОКА
  
  initWebServer();
}

void loop() {
  handleWebRequests();
  updateAllSensors(); // Используем новый менеджер датчиков
  
  // Индикация режима работы:
  // - Одиночное мигание (1 сек): STA режим (клиент)
  // - Двойное мигание (0.3 сек + пауза 0.2 сек + 0.3 сек): AP режим (точка доступа)
  static unsigned long lastBlink = 0;
  static bool ledState = false;
  static int blinkPhase = 0; // 0 = ожидание, 1 = первое мигание, 2 = пауза, 3 = второе мигание
  
  unsigned long currentMillis = millis();
  
  if (WiFi.getMode() == WIFI_STA) {
    // STA режим - одиночное мигание
    if (currentMillis - lastBlink > 1000) {
      lastBlink = currentMillis;
      ledState = !ledState;
      digitalWrite(LED_BUILTIN, ledState ? LOW : HIGH);
    }
  } else {
    // AP или AP+STA режим - двойное мигание
    if (blinkPhase == 0 && currentMillis - lastBlink > 1000) {
      // Начало двойного мигания
      blinkPhase = 1;
      digitalWrite(LED_BUILTIN, LOW); // Включаем
      lastBlink = currentMillis;
    }
    else if (blinkPhase == 1 && currentMillis - lastBlink > 300) {
      // Выключаем после первого мигания
      digitalWrite(LED_BUILTIN, HIGH);
      blinkPhase = 2;
      lastBlink = currentMillis;
    }
    else if (blinkPhase == 2 && currentMillis - lastBlink > 200) {
      // Второе мигание
      digitalWrite(LED_BUILTIN, LOW);
      blinkPhase = 3;
      lastBlink = currentMillis;
    }
    else if (blinkPhase == 3 && currentMillis - lastBlink > 300) {
      // Завершение двойного мигания
      digitalWrite(LED_BUILTIN, HIGH);
      blinkPhase = 0;
      lastBlink = currentMillis;
    }
  }
}

