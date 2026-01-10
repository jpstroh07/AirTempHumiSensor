#ifndef MYDISPLAY_H
#define MYDISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1

#define SDA_PIN 21
#define SCL_PIN 22

void initDisplay();
void showIPAddress(const char* ip);
void showReadings(float temperature, float humidity);
void clearDisplay();
void showError(const char* message);

#endif