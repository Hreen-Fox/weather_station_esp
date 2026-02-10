// sensor_generator.h - Заголовочный файл генератора данных датчиков

// Защита от повторного включения файла (include guard)
#ifndef SENSOR_GENERATOR_H
#define SENSOR_GENERATOR_H

// Зависимости
#include <Arduino.h>

/**
 * Структура данных датчиков
 * 
 * Хранит текущие значения сгенерированных показаний:
 * - temperature: температура в градусах Цельсия (с плавающей точкой)
 * - humidity: относительная влажность в процентах (целое число)
*/
struct SensorData {
  float temperature;
  int humidity;
};

// Объявление глобальной переменной с текущими данными датчиков
// Ключевое слово 'extern' указывает, что переменная определена в другом файле (.cpp)
extern SensorData currentSensorData;

// Функция обновления данных датчиков
void updateSensorData();

// Функция получения данных датчиков в формате JSON
String getSensorDataJSON();

#endif