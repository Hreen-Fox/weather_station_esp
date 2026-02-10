#include "sensor_manager.h"
#include <LittleFS.h>
#include <math.h>
#include <ESP8266WiFi.h>

Sensor sensors[MAX_SENSORS];
uint8_t sensorCount = 0;

void loadSensors() {
  if (!LittleFS.begin()) return;
  
  if (LittleFS.exists("/sensors.dat")) {
    File file = LittleFS.open("/sensors.dat", "r");
    if (file) {
      file.readBytes((char*)&sensorCount, sizeof(uint8_t));
      if (sensorCount > MAX_SENSORS) sensorCount = 0;
      
      for (int i = 0; i < sensorCount; i++) {
        file.readBytes((char*)&sensors[i], sizeof(Sensor));
      }
      file.close();
    }
  } else {
    initDefaultSensors();
    saveSensors();
  }
}

void saveSensors() {
  if (!LittleFS.begin()) return;
  
  File file = LittleFS.open("/sensors.dat", "w");
  if (file) {
    file.write((uint8_t*)&sensorCount, sizeof(uint8_t));
    for (int i = 0; i < sensorCount; i++) {
      file.write((uint8_t*)&sensors[i], sizeof(Sensor));
    }
    file.close();
  }
}

void initDefaultSensors() {
  sensorCount = 0;
  
  // Датчик температуры
  sensors[sensorCount].id = 1;
  strcpy(sensors[sensorCount].name, "Температура");
  strcpy(sensors[sensorCount].dbName, "temperature"); 
  strcpy(sensors[sensorCount].unit, "°C");
  sensors[sensorCount].minValue = -10.0;
  sensors[sensorCount].maxValue = 50.0;
  sensors[sensorCount].enabled = true;
  sensors[sensorCount].currentValue = 22.0;
  sensors[sensorCount].lastUpdate = 0;
  sensorCount++;
  
  // Датчик влажности
  sensors[sensorCount].id = 2;
  strcpy(sensors[sensorCount].name, "Влажность");
  strcpy(sensors[sensorCount].dbName, "humidity"); 
  strcpy(sensors[sensorCount].unit, "%");
  sensors[sensorCount].minValue = 0.0;
  sensors[sensorCount].maxValue = 100.0;
  sensors[sensorCount].enabled = true;
  sensors[sensorCount].currentValue = 45.0;
  sensors[sensorCount].lastUpdate = 0;
  sensorCount++;
  
  // Датчик давления
  sensors[sensorCount].id = 3;
  strcpy(sensors[sensorCount].name, "Давление");
  strcpy(sensors[sensorCount].dbName, "pressure"); 
  strcpy(sensors[sensorCount].unit, "мм рт.ст.");
  sensors[sensorCount].minValue = 700.0;
  sensors[sensorCount].maxValue = 800.0;
  sensors[sensorCount].enabled = true;
  sensors[sensorCount].currentValue = 755.0;
  sensors[sensorCount].lastUpdate = 0;
  sensorCount++;
}

float generateSensorValue(const Sensor& sensor) {
  if (!sensor.enabled) return 0.0;
  
  // Базовое значение (середина диапазона)
  float baseValue = (sensor.minValue + sensor.maxValue) / 2.0;
  float amplitude = (sensor.maxValue - sensor.minValue) / 4.0;
  
  // Синусоидальные колебания
  float slowCycle = sin(millis() / (30.0 * 60.0 * 1000.0)) * amplitude;
  float fastCycle = sin(millis() / (5.0 * 60.0 * 1000.0)) * (amplitude * 0.3);
  float randomVariation = (random(100) - 50) / 100.0 * amplitude * 0.2;
  
  float value = baseValue + slowCycle + fastCycle + randomVariation;
  
  // Ограничение диапазона
  if (value < sensor.minValue) value = sensor.minValue;
  if (value > sensor.maxValue) value = sensor.maxValue;
  
  return value;
}

void updateAllSensors() {
  static unsigned long lastUpdate = 0;
  static uint16_t updateInterval = 5000; // 5 секунд
  
  if (millis() - lastUpdate >= updateInterval) {
    for (int i = 0; i < sensorCount; i++) {
      sensors[i].currentValue = generateSensorValue(sensors[i]);
      sensors[i].lastUpdate = millis();
    }
    lastUpdate = millis();
  }
}

String getSensorsJSON() {
  String json = "[";
  for (int i = 0; i < sensorCount; i++) {
    if (i > 0) json += ",";
    json += "{";
    json += "\"id\":" + String(sensors[i].id) + ",";
    json += "\"name\":\"" + String(sensors[i].name) + "\",";
    json += "\"unit\":\"" + String(sensors[i].unit) + "\",";
    json += "\"minValue\":" + String(sensors[i].minValue, 1) + ",";
    json += "\"maxValue\":" + String(sensors[i].maxValue, 1) + ",";
    json += "\"enabled\":" + String(sensors[i].enabled ? "true" : "false") + ",";
    json += "\"currentValue\":" + String(sensors[i].currentValue, 1);
    json += "}";
  }
  json += "]";
  return json;
}

String getSensorStatsJSON() {
  String json = "{";
  json += "\"sensorCount\":" + String(sensorCount) + ",";
  json += "\"sensors\":" + getSensorsJSON() + ",";
  json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"uptime\":" + String(millis() / 1000) + ",";
  json += "\"wifiStatus\":" + String(WiFi.status() == WL_CONNECTED ? "true" : "false") + ",";
  json += "\"ipAddress\":\"" + (WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString() : WiFi.softAPIP().toString()) + "\"";
  json += "}";
  return json;
}