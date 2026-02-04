#ifndef SETUP_WEB_H
#define SETUP_WEB_H

#include <ESP8266WebServer.h>

extern ESP8266WebServer server;

void initSetupWebServer();
void handleSetupRequests();

#endif 