#ifndef TEMPHUMI_H
#define TEMPHUMI_H

#include <Arduino.h>
#include <SHT1x.h>

#define TH_DATA_PIN  27
#define TH_CLOCK_PIN 26

float tempReading();
float humiReading();

#endif