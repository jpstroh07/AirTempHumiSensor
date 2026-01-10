#include <Arduino.h>
#include <TempHumi.h>
#include <Distance.h>
#include <THWifi.h>
#include <THServer.h>
#include <MyDisplay.h>

int offlineTick = 0;

void readAndUpdate()
{
	float temperature = tempReading();
	float humidity = humiReading();

	updatePageContent(temperature, humidity);
	showReadings(temperature, humidity);

	offlineTick = 0;
}

void offlineBehavior()
{
	Serial.println("Object out of range.");

	sensorOffline();
	clearDisplay();

	offlineTick++;

	if (offlineTick >= 5)
	{
		Serial.println("Offline tick limit reached, forcing update.");

		readAndUpdate();
		offlineTick = 0;
	}
}

void setup()
{
	Serial.begin(9600);

	initDisplay();
	initWiFi();
	initServer();

	delay(2000);
}

void loop()
{
	if (isInRange())
	{
		readAndUpdate();
	}
	else
	{
		offlineBehavior();
	}

	delay(2000);
}
