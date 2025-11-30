#ifndef THWIFI_H
#define THWIFI_H

#include <Arduino.h>
#include <WiFi.h>

#define WIFI_SSID "FRITZ!Box 7590 BZ"
#define WIFI_PASSWORD "06372148180061539730"

void initWiFi();
String getIPAddress();

#endif