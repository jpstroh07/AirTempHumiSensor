#include "TempHumi.h"

SHT1x ht_sensor(TH_DATA_PIN, TH_CLOCK_PIN);

float tempReading()
{
    float temperature = ht_sensor.readTemperatureC();

    if (temperature < -40.0 || temperature > 125.0)
    {
        Serial.println("Temperature reading out of range!");
        throw "Sensor is broken or not connected.";
    }

    return temperature;
}

float humiReading()
{
    float humidity = ht_sensor.readHumidity();

    if (humidity < 0.0 || humidity > 100.0)
    {
        Serial.println("Humidity reading out of range!");
        throw "Sensor is broken or not connected.";
    }

    return humidity;
}