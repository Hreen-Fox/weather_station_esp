#ifndef SENSOR_GENERATOR_H
#define SENSOR_GENERATOR_H

// Структура для хранения текущих значений датчиков
struct SensorData {
  float temperature;
  int humidity;
};

// Глобальная переменная с последними данными
extern SensorData currentSensorData;

// Функции генерации данных
void updateSensorData();
float generateTemperature();
int generateHumidity();
String getSensorDataJSON();

#endif