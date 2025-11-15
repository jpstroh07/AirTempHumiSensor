#include "displaylogic.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

void showSplashScreen()
{
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_helvB12_tf);
    u8g2.setCursor(0, 20);
    u8g2.print("SHT10 Sensor");
    u8g2.setCursor(0, 40);
    u8g2.print("Test");
    u8g2.sendBuffer();
}

void updateDisplay(float temp, float humi)
{
    u8g2.clearBuffer();
    u8g2.setCursor(0, 20);
    u8g2.print("Temp: ");
    u8g2.print(temp, 1);
    u8g2.print(" C");
    u8g2.setCursor(0, 40);
    u8g2.print("Humidity: ");
    u8g2.print(humi, 1);
    u8g2.print(" %");
    u8g2.sendBuffer();
}

void blankDisplay()
{
    u8g2.clearBuffer();
    u8g2.sendBuffer();
}

void showErrorMessage()
{
    u8g2.clearBuffer();
    u8g2.setCursor(0, 20);
    u8g2.print("Error reading");
    u8g2.setCursor(0, 40);
    u8g2.print("from sensor");
    u8g2.sendBuffer();
}