#include <Arduino.h>
#include <LoRa.h>
#include <WiFi.h>

#include "config.h"
#include "display.h"
#include "lora_link.h"
#include "shelly_client.h"
#include "telemetry.h"

namespace {
uint32_t sequenceNumber = 0;
unsigned long lastPollMs = 0;
TelemetryReading lastReading{};
bool hasReading = false;
int lastRssi = 0;
unsigned long lastReceiveMs = 0;

#if defined(DEVICE_ROLE_SERVER)
void connectWiFi() {
    if (WiFi.status() == WL_CONNECTED) {
        return;
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    showServerStatus("connecting", "idle", hasReading ? &lastReading : nullptr);

    const unsigned long startedMs = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startedMs < 15000UL) {
        delay(250);
    }
}

void pollAndTransmit() {
    connectWiFi();
    if (WiFi.status() != WL_CONNECTED) {
        showServerStatus("offline", hasReading ? "last value" : "skipped", hasReading ? &lastReading : nullptr);
        return;
    }

    char error[32] = {};
    TelemetryReading reading{};
    if (!fetchShellyReading(SHELLY_STATUS_URL, ++sequenceNumber, reading, error, sizeof(error))) {
        showServerStatus("online", hasReading ? "last value" : error, hasReading ? &lastReading : nullptr);
        Serial.print("Shelly error: ");
        Serial.println(error);
        return;
    }

    const std::string packet = formatTelemetry(reading);
    const bool sent = sendLoRaPacket(packet);
    lastReading = reading;
    hasReading = true;

    showServerStatus("online", sent ? "sent" : "LoRa failed", &lastReading);
    Serial.println(packet.c_str());
}
#endif

#if defined(DEVICE_ROLE_CLIENT)
void handleReceive() {
    const int packetSize = LoRa.parsePacket();
    if (packetSize <= 0) {
        if (hasReading) {
            showClientReading(lastReading, lastRssi, millis() - lastReceiveMs);
        }
        delay(250);
        return;
    }

    std::string packet;
    while (LoRa.available()) {
        packet.push_back(static_cast<char>(LoRa.read()));
    }

    TelemetryReading reading{};
    if (!parseTelemetry(packet, reading)) {
        showError("Bad packet", packet.c_str());
        Serial.print("Bad packet: ");
        Serial.println(packet.c_str());
        return;
    }

    lastReading = reading;
    hasReading = true;
    lastRssi = LoRa.packetRssi();
    lastReceiveMs = millis();
    showClientReading(lastReading, lastRssi, 0);
    Serial.println(packet.c_str());
}
#endif
}

void setup() {
    Serial.begin(115200);
    delay(200);

    if (!setupDisplay()) {
        Serial.println("OLED setup failed");
    }

#if defined(DEVICE_ROLE_SERVER)
    showBoot("server");
#elif defined(DEVICE_ROLE_CLIENT)
    showBoot("client");
#else
#error "Define DEVICE_ROLE_SERVER or DEVICE_ROLE_CLIENT"
#endif

    if (!setupLoRa()) {
        showError("LoRa setup", "failed");
        Serial.println("LoRa setup failed");
        while (true) {
            delay(1000);
        }
    }

#if defined(DEVICE_ROLE_CLIENT)
    showClientWaiting();
#endif
}

void loop() {
#if defined(DEVICE_ROLE_SERVER)
    const unsigned long now = millis();
    if (lastPollMs == 0 || now - lastPollMs >= POLL_INTERVAL_MS) {
        lastPollMs = now;
        pollAndTransmit();
    }
    delay(100);
#elif defined(DEVICE_ROLE_CLIENT)
    handleReceive();
#endif
}
