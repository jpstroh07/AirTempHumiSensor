#include <Arduino.h>
#include <TempHumi.h>
#include <Distance.h>

void setup()
{
	Serial.begin(9600);
	Serial.println("Temperature and Humidity Sensor Test");
	Serial.println("-----------------------------------");
}

void loop()
{
	if (isInRange())
	{
		float temperature = tempReading();
		float humidity = humiReading();

		Serial.print("Temperature: ");
		Serial.print(temperature);
		Serial.print(" °C, Humidity: ");
		Serial.print(humidity);
		Serial.println(" %");
	}
	else
	{
		Serial.println("Object out of range.");
	}
	delay(2000);
}
