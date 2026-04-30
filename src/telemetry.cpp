#include "telemetry.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

std::string formatTelemetry(const TelemetryReading &reading) {
    char buffer[128];
    std::snprintf(
        buffer,
        sizeof(buffer),
        "PWR1|seq=%lu|w=%.1f|v=%.1f|a=%.2f|pf=%.2f|hz=%.1f|wh=%.1f",
        static_cast<unsigned long>(reading.sequence),
        reading.powerW,
        reading.voltageV,
        reading.currentA,
        reading.powerFactor,
        reading.frequencyHz,
        reading.energyWh);
    return std::string(buffer);
}

static bool parseUnsigned(const char *text, uint32_t &value) {
    char *end = nullptr;
    const unsigned long parsed = std::strtoul(text, &end, 10);
    if (end == text || *end != '\0') {
        return false;
    }
    value = static_cast<uint32_t>(parsed);
    return true;
}

static bool parseFloat(const char *text, float &value) {
    char *end = nullptr;
    const float parsed = std::strtof(text, &end);
    if (end == text || *end != '\0') {
        return false;
    }
    value = parsed;
    return true;
}

bool parseTelemetry(const std::string &packet, TelemetryReading &reading) {
    constexpr const char *prefix = "PWR1|";
    if (packet.rfind(prefix, 0) != 0) {
        return false;
    }

    TelemetryReading parsed{};
    bool hasSeq = false;
    bool hasPower = false;
    bool hasVoltage = false;
    bool hasCurrent = false;
    bool hasPowerFactor = false;
    bool hasFrequency = false;
    bool hasEnergy = false;

    char buffer[160];
    if (packet.size() >= sizeof(buffer)) {
        return false;
    }
    std::strncpy(buffer, packet.c_str() + std::strlen(prefix), sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    char *token = std::strtok(buffer, "|");
    while (token != nullptr) {
        char *equals = std::strchr(token, '=');
        if (equals == nullptr) {
            return false;
        }

        *equals = '\0';
        const char *key = token;
        const char *value = equals + 1;

        if (std::strcmp(key, "seq") == 0) {
            hasSeq = parseUnsigned(value, parsed.sequence);
        } else if (std::strcmp(key, "w") == 0) {
            hasPower = parseFloat(value, parsed.powerW);
        } else if (std::strcmp(key, "v") == 0) {
            hasVoltage = parseFloat(value, parsed.voltageV);
        } else if (std::strcmp(key, "a") == 0) {
            hasCurrent = parseFloat(value, parsed.currentA);
        } else if (std::strcmp(key, "pf") == 0) {
            hasPowerFactor = parseFloat(value, parsed.powerFactor);
        } else if (std::strcmp(key, "hz") == 0) {
            hasFrequency = parseFloat(value, parsed.frequencyHz);
        } else if (std::strcmp(key, "wh") == 0) {
            hasEnergy = parseFloat(value, parsed.energyWh);
        } else {
            return false;
        }

        token = std::strtok(nullptr, "|");
    }

    if (!(hasSeq && hasPower && hasVoltage && hasCurrent && hasPowerFactor && hasFrequency && hasEnergy)) {
        return false;
    }

    reading = parsed;
    return true;
}
