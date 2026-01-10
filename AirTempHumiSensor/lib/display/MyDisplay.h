#ifndef MYDISPLAY_H
#define MYDISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

#define SDA_PIN 21
#define SCL_PIN 22

void initDisplay();
void showIPAddress(const char* ip);
void showReadings(float temperature, float humidity);
void clearDisplay();
void showError(const char* message);

#endif