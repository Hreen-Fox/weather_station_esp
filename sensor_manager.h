#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>

// Максимальное количество датчиков
#define MAX_SENSORS 10

// Структура датчика
struct Sensor {
  uint8_t id;              // Уникальный ID (1-255)
  char name[32];          // Название датчика
  char dbName[32];
  char unit[16];          // Единица измерения
  float minValue;         // Минимальное значение
  float maxValue;         // Максимальное значение  
  bool enabled;           // Включен/выключен
  float currentValue;     // Текущее значение
  unsigned long lastUpdate; // Время последнего обновления
};

// Глобальные переменные
extern Sensor sensors[MAX_SENSORS];
extern uint8_t sensorCount;

// Функции управления
void loadSensors();
void saveSensors();
void initDefaultSensors();
void updateAllSensors();
float generateSensorValue(const Sensor& sensor);
String getSensorsJSON();
String getSensorStatsJSON();

#endif