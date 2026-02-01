#include "config_manager.h"
#include "setup_web.h"
// web_ui.h подключается автоматически через setup_web.h

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== ESP8266 Setup Mode ===");
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  loadConfig();
  
  if (isValidConfig()) {
    Serial.println("✅ Найдена валидная конфигурация");
    Serial.print("SSID: ");
    Serial.println(config.wifiSSID);
  }
  
  initSetupWebServer();
}
 
void loop() {
  handleSetupRequests();
  
  static unsigned long lastBlink = 0;
  static bool ledState = false;
  if (millis() - lastBlink > 500) {
    lastBlink = millis();
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState ? LOW : HIGH);
  }
}