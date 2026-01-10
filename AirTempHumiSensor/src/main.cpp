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
	Serial.println("Person not in range.");

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
	pinMode(IP_BUTTON_PIN, INPUT_PULLUP);

	initWiFi();
	initServer();
	initDisplay();

	delay(2000);
}

void loop()
{
	try
	{
		if (digitalRead(IP_BUTTON_PIN) == LOW)
		{
			Serial.println("IP button pressed.");
			showIPAddress(WiFi.localIP().toString().c_str());

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

		for (;;)
			;
	}
}