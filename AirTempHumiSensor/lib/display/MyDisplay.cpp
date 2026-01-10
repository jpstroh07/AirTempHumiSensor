#include "MyDisplay.h"

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE, SCL_PIN, SDA_PIN);

void initDisplay()
{
    Wire.begin(SDA_PIN, SCL_PIN);

    display.begin();

    display.clearBuffer();
    display.setFont(u8g2_font_6x13_tf);
    display.setCursor(0, 20);

    display.println("Sensor initialized");

    display.sendBuffer();
}

void showIPAddress(String ip)
{
    display.clearBuffer();
    display.setFont(u8g2_font_6x13_tf);
    display.setCursor(0, 10);

    display.println("IP Address:");
    display.setCursor(0, 25);
    display.println(ip.c_str());

    display.sendBuffer();
}

void showReadings(float temperature, float humidity)
{
    display.clearBuffer();
    display.setFont(u8g2_font_8x13_tf);
    display.setCursor(0, 20);

    display.print(temperature);
    display.print("C | ");
    display.print(humidity);
    display.print("%");

    display.sendBuffer();
}

void clearDisplay()
{
    display.clearBuffer();
    display.sendBuffer();
}

void showError(const char *message)
{

    display.clearBuffer();
    display.setFont(u8g2_font_6x13_tf);
    display.setCursor(0, 10);

    display.println("Error:");
    display.setCursor(0, 25);
    display.println(message);
    
    display.sendBuffer();
}