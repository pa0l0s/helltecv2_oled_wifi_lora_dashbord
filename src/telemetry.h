#pragma once

#include <cstdint>
#include <string>

struct TelemetryReading {
    uint32_t sequence = 0;
    float powerW = 0.0f;
    float voltageV = 0.0f;
    float currentA = 0.0f;
    float powerFactor = 0.0f;
    float frequencyHz = 0.0f;
    float energyWh = 0.0f;
};

std::string formatTelemetry(const TelemetryReading &reading);
bool parseTelemetry(const std::string &packet, TelemetryReading &reading);
