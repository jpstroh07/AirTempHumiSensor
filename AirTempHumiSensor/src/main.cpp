#include <Arduino.h>
#include <TempHumi.h>
#include <Distance.h>
#include <THWifi.h>
#include <THServer.h>
#include <MyDisplay.h>

#define IP_BUTTON_PIN 0

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
	sensorOffline();
	clearDisplay();

	offlineTick++;
	Serial.print("Sensor offline tick: ");
	Serial.println(offlineTick);

	if (offlineTick >= 150)
	{
		Serial.println("Offline tick limit reached, forcing update.");

		readAndUpdate();
		offlineTick = 0;
	}
}

void setup()
{
	try
	{
		Serial.begin(9600);
		pinMode(IP_BUTTON_PIN, INPUT_PULLUP);

		initDisplay();
		initWiFi();
		initServer();

		delay(2000);
	}
	catch (const char *msg)
	{
		Serial.print("Exception caught during setup: ");
		Serial.println(msg);

		showError(msg);

		for (;;)
			;
	}
}

void loop()
{
	try
	{
		if (digitalRead(IP_BUTTON_PIN) == LOW)
		{
			Serial.println("IP button pressed.");
			showIPAddress(getIPAddress());

			delay(5000);
		}
		else
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
	}
	catch (const char *msg)
	{
		Serial.print("Exception caught: ");
		Serial.println(msg);

		showError(msg);
		sensorError();

		for (;;)
			;
	}
}