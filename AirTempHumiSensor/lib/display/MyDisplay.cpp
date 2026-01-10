#include "MyDisplay.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initDisplay() {
    Wire.begin(SDA_PIN, SCL_PIN);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }

    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("Display Initialized");

    display.display();
}

void showIPAddress(const char* ip) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("IP Address:");
    display.setTextSize(2);
    display.setCursor(0,16);
    display.println(ip);
    display.display();
}

void showReadings(float temperature, float humidity) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.print("Temp: ");
    display.print(temperature);
    display.println(" C");
    display.print("Humi: ");
    display.print(humidity);
    display.println(" %");
    display.display();
}

void clearDisplay() {
    display.clearDisplay();
    display.display();
}

void showError() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("Error!");
    display.display();
}