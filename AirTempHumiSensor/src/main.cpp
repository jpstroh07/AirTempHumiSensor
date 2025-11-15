#include <Arduino.h>
#include <SHT1x.h>
#include "display/displaylogic.h"
#include <HCSR04.h>

#define DATA_PIN_G 17
#define CLOCK_PIN_G 16

#define TRIGGER_PIN 5
#define ECHO_PIN 18

SHT1x sht10_gold(DATA_PIN_G, CLOCK_PIN_G);

UltraSonicDistanceSensor sensor(TRIGGER_PIN, ECHO_PIN);

void setup()
{
    Serial.begin(9600);
    Serial.println("Start reading...");
    showSplashScreen();
    delay(2000);
}

void loop()
{
    long distance = sensor.measureDistanceCm();
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance <= 10)
    {
        for (int i = 0; i < 5; i++)
        {
            float temp = sht10_gold.readTemperatureC();
            float humi = sht10_gold.readHumidity();

            if (temp >= 200.0 || humi < 0.0 || humi > 100.0)
            {
                Serial.println("Failed to read from sensor");
                showErrorMessage();
            }
            else
            {
                Serial.print("Temperature: ");
                Serial.print(temp);
                Serial.print("°C, Humidity: ");
                Serial.print(humi);
                Serial.println("%");

                updateDisplay(temp, humi);
            }
            delay(1000);
        }
    }

    blankDisplay();
    delay(1000);
}