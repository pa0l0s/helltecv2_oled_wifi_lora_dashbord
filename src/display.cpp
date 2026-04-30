#include "display.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

namespace {
constexpr int OLED_SDA = 4;
constexpr int OLED_SCL = 15;
constexpr int OLED_RST = 16;
constexpr int VEXT_PIN = 21;
constexpr int SCREEN_WIDTH = 128;
constexpr int SCREEN_HEIGHT = 64;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

void clearWithTitle(const char *title) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(title);
    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
}
}

bool setupDisplay() {
    pinMode(VEXT_PIN, OUTPUT);
    digitalWrite(VEXT_PIN, LOW);
    delay(50);

    pinMode(OLED_RST, OUTPUT);
    digitalWrite(OLED_RST, LOW);
    delay(20);
    digitalWrite(OLED_RST, HIGH);

    Wire.begin(OLED_SDA, OLED_SCL);
    return display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void showBoot(const char *role) {
    clearWithTitle("Heltec Shelly LoRa");
    display.setCursor(0, 18);
    display.print("Role: ");
    display.println(role);
    display.println("LoRa: 868 MHz");
    display.display();
}

void showServerStatus(const char *wifiStatus, const char *shellyStatus, const TelemetryReading *reading) {
    clearWithTitle("Server");
    display.setCursor(0, 14);
    display.print("WiFi: ");
    display.println(wifiStatus);
    display.print("Shelly: ");
    display.println(shellyStatus);
    if (reading != nullptr) {
        display.print("P: ");
        display.print(reading->powerW, 1);
        display.println(" W");
        display.print("E: ");
        display.print(reading->energyWh, 1);
        display.println(" Wh");
        display.print("Seq: ");
        display.println(reading->sequence);
    }
    display.display();
}

void showClientWaiting() {
    clearWithTitle("Client");
    display.setCursor(0, 18);
    display.println("Waiting for LoRa");
    display.println("868 MHz");
    display.display();
}

void showClientReading(const TelemetryReading &reading, int rssi, unsigned long ageMs) {
    clearWithTitle("Client");
    display.setCursor(0, 13);
    display.print("P: ");
    display.print(reading.powerW, 1);
    display.println(" W");
    display.print("E: ");
    display.print(reading.energyWh, 1);
    display.println(" Wh");
    display.print("V/A: ");
    display.print(reading.voltageV, 0);
    display.print(" / ");
    display.println(reading.currentA, 2);
    display.print("Age: ");
    display.print(ageMs / 1000UL);
    display.println(" s");
    display.print("RSSI: ");
    display.println(rssi);
    display.display();
}

void showError(const char *line1, const char *line2) {
    clearWithTitle("Error");
    display.setCursor(0, 18);
    display.println(line1);
    display.println(line2);
    display.display();
}
