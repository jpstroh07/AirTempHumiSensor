#ifndef displaylogic_h
#define displaylogic_h

#include <Arduino.h>
#include <U8g2lib.h>

void showSplashScreen();
void updateDisplay(float temp, float humi);
void blankDisplay();
void showErrorMessage();

#endif