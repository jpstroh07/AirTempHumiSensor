#include "THWifi.h"

void initWiFi()
{
    Serial.print("Connecting to WiFi");

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    for (size_t i = 0; i < 10; i++)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            delay(1000);
            Serial.print(".");
        }
        else
        {
            break;
        }
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("Connected to WiFi!");
        Serial.print("IP Address: ");
        Serial.println(getIPAddress());
    }
    else
    {
        Serial.println("Failed to connect to WiFi");
        throw "WiFi Connection Failed";
    }
}

String getIPAddress()
{
    return WiFi.localIP().toString();
}