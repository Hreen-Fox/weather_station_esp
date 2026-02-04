#include "sensor_generator.h"
#include "config_manager.h"
#include <Arduino.h>
#include <math.h>

// Глобальная переменная с текущими данными
SensorData currentSensorData = {0, 0};

// Время последнего обновления
static unsigned long lastUpdate = 0;

/**
 * Обновление данных датчиков с учетом интервала
 * Вызывается в основном loop()
 */
void updateSensorData() {
  unsigned long currentTime = millis();
  
  // Проверяем, прошло ли достаточно времени с последнего обновления
  if (currentTime - lastUpdate >= (unsigned long)config.updateInterval * 1000) {
    currentSensorData.temperature = generateTemperature();
    currentSensorData.humidity = generateHumidity();
    lastUpdate = currentTime;
    
    Serial.print("🌡️ Данные обновлены: ");
    Serial.print("T="); Serial.print(currentSensorData.temperature);
    Serial.print("°C, H="); Serial.print(currentSensorData.humidity); 
    Serial.println("%");
  }
}

/**
 * Генерация реалистичной температуры
 * Использует синусоидальные колебания + случайные вариации
 */
float generateTemperature() {
  // Основной цикл (медленные изменения - каждые 30 минут)
  float slowCycle = sin(millis() / (30.0 * 60.0 * 1000.0)) * config.tempAmplitude;
  
  // Быстрые колебания (каждые 2 минуты)
  float fastCycle = sin(millis() / (2.0 * 60.0 * 1000.0)) * (config.tempAmplitude * 0.3);
  
  // Случайные вариации (±0.5°C)
  float randomVariation = (random(100) - 50) / 100.0;
  
  float temperature = config.tempBase + slowCycle + fastCycle + randomVariation;
  
  // Ограничение разумных значений
  if (temperature < -10.0) temperature = -10.0;
  if (temperature > 50.0) temperature = 50.0;
  
  return temperature;
}

/**
 * Генерация реалистичной влажности
 * Коррелирует с температурой (обычно когда теплее - суше)
 */
int generateHumidity() {
  // Основной цикл влажности
  float humCycle = sin(millis() / (45.0 * 60.0 * 1000.0)) * config.humAmplitude;
  
  // Влияние температуры (противофаза)
  float tempEffect = -sin(millis() / (30.0 * 60.0 * 1000.0)) * (config.humAmplitude * 0.4);
  
  // Случайные вариации
  int randomVariation = random(-3, 4);
  
  int humidity = (int)(config.humBase + humCycle + tempEffect + randomVariation);
  
  // Ограничение диапазона 0-100%
  if (humidity < 0) humidity = 0;
  if (humidity > 100) humidity = 100;
  
  return humidity;
}

/**
 * Получение данных в формате JSON
 * Используется веб-сервером и API
 */
String getSensorDataJSON() {
  String json = "{";
  json += "\"temperature\":" + String(currentSensorData.temperature, 1);
  json += ",\"humidity\":" + String(currentSensorData.humidity);
  json += "}";
  return json;
}