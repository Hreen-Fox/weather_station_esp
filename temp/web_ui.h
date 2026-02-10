#ifndef WEB_UI_H
#define WEB_UI_H

#include <ESP8266WebServer.h>

extern const char SENSORS_PAGE[] PROGMEM;
extern const char STATISTICS_PAGE[] PROGMEM;
extern const char SETTINGS_PAGE[] PROGMEM;

void handleSensorsPage();
void handleStatisticsPage();
void handleSettingsPage();

#endif