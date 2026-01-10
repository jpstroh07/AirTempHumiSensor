#include <Arduino.h>
#include <TempHumi.h>
#include <Distance.h>
#include <THWifi.h>
#include <THServer.h>

int offlineTick = 0;

void readAndUpdate()
{
	float temperature = tempReading();
	float humidity = humiReading();

	updatePageContent(temperature, humidity);

	offlineTick = 0;
}

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
		readAndUpdate();
	}
	else
	{
		Serial.println("Object out of range.");
		sensorOffline();

		offlineTick++;

		if (offlineTick >= 5) {
			readAndUpdate();
		}
	}

	delay(2000);
}
