#ifndef THSERVER_H
#define THSERVER_H

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

void initServer();
void updatePageContent(float temperature, float humidity);
void sensorOffline();

#endif