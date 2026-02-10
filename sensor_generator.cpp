// sensor_generator.cpp - Реализация генератора данных датчиков

// Зависимости
#include "sensor_generator.h"
#include "config_manager.h"
#include <math.h>

// Определение глобальной переменной с текущими данными датчиков
// Инициализируется нулевыми значениями при запуске
SensorData currentSensorData = {0, 0};
// Статическая переменная для отслеживания времени последнего обновления
// static: видна только в этом файле, сохраняет значение между вызовами
static unsigned long lastUpdate = 0;

// Функция обновления данных датчиков
void updateSensorData() {
  unsigned long currentTime = millis();
  
  // Проверка, прошло ли достаточно времени с последнего обновления
  if (currentTime - lastUpdate >= (unsigned long)config.updateInterval * 1000) {
    
    // === ГЕНЕРАЦИЯ ТЕМПЕРАТУРЫ ===
    
    // Медленный цикл (изменения каждые 30 минут)
    // Создает суточные колебания температуры
    float slowCycle = sin(millis() / (30.0 * 60.0 * 1000.0)) * config.tempAmplitude;
    
    // Быстрый цикл (изменения каждые 2 минуты)  
    // Создает краткосрочные колебания
    float fastCycle = sin(millis() / (2.0 * 60.0 * 1000.0)) * (config.tempAmplitude * 0.3);
    
    // Случайные вариации (±0.5°C)
    // Имитирует шум и непредсказуемые изменения
    float randomVariation = (random(100) - 50) / 100.0;
    
    // Расчет итоговой температуры
    float temperature = config.tempBase + slowCycle + fastCycle + randomVariation;
    
    // Ограничение разумного диапазона температур (-10°C до +50°C)
    if (temperature < -10.0) temperature = -10.0;
    if (temperature > 50.0) temperature = 50.0;
    
    // === ГЕНЕРАЦИЯ ВЛАЖНОСТИ ===
    
    // Основной цикл влажности (каждые 45 минут)
    float humCycle = sin(millis() / (45.0 * 60.0 * 1000.0)) * config.humAmplitude;
    
    // Влияние температуры на влажность (противофаза)
    // Когда температура повышается, влажность обычно снижается
    float tempEffect = -sin(millis() / (30.0 * 60.0 * 1000.0)) * (config.humAmplitude * 0.4);
    
    // Случайные вариации влажности (±3%)
    int randomVariationHum = random(-3, 4);
    
    // Расчет итоговой влажности
    int humidity = (int)(config.humBase + humCycle + tempEffect + randomVariationHum);
    
    // Ограничение диапазона влажности (0% до 100%)
    if (humidity < 0) humidity = 0;
    if (humidity > 100) humidity = 100;
    
    // Сохранение сгенерированных значений
    currentSensorData.temperature = temperature;
    currentSensorData.humidity = humidity;
    
    // Обновление времени последнего обновления
    lastUpdate = currentTime;
  }
}

// Функция получения данных датчиков в формате JSON
String getSensorDataJSON() {
  String json = "{";
  // Температура с одной цифрой после запятой
  json += "\"temperature\":" + String(currentSensorData.temperature, 1);
  // Влажность как целое число
  json += ",\"humidity\":" + String(currentSensorData.humidity);
  json += "}";
  return json;
}