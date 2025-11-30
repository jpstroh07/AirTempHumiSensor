#include <Arduino.h>
#include <TempHumi.h>
#include <Distance.h>
#include <THWifi.h>
#include <THServer.h>

void setup()
{
	Serial.begin(9600);
	initWiFi();
	initServer();
}

void loop()
{
	if (isInRange())
	{
		float temperature = tempReading();
		float humidity = humiReading();

		updatePageContent(temperature, humidity);
	}
	else
	{
		Serial.println("Object out of range.");
		sensorOffline();
	}

	delay(2000);
}
