#include "Distance.h"

UltraSonicDistanceSensor us_sensor(US_TRIGGER_PIN, US_ECHO_PIN);

boolean isInRange(int rangeCm)
{
    long distance = us_sensor.measureDistanceCm();
    if (distance > 0 && distance < rangeCm)
    {
        return true;
    }
    else
    {
        return false;
    }
}