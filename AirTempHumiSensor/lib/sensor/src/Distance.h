#ifndef DISTANCE_H
#define DISTANCE_H

#include <Arduino.h>
#include <HCSR04.h>

#define US_TRIGGER_PIN 32
#define US_ECHO_PIN 33

#define US_MAX_DISTANCE_CM 15

boolean isInRange(int rangeCm = US_MAX_DISTANCE_CM);

#endif