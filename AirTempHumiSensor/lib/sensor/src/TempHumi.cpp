#include "TempHumi.h"

SHT1x ht_sensor(TH_DATA_PIN, TH_CLOCK_PIN);

float tempReading()
{
    return ht_sensor.readTemperatureC();
}

float humiReading()
{
    return ht_sensor.readHumidity();
}