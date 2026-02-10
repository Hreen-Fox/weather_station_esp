#ifndef WEB_PAGES_H
#define WEB_PAGES_H

#include <Arduino.h>

extern const char SETUP_PAGE[] PROGMEM;
extern const char SENSORS_PAGE[] PROGMEM;
extern const char STATISTICS_PAGE[] PROGMEM;
extern const char SETTINGS_PAGE[] PROGMEM;

void handleSetupPage();
void handleSensorsPage();
void handleStatisticsPage();
void handleSettingsPage();

#endif