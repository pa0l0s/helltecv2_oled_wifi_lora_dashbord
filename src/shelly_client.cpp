#include "shelly_client.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

static void setError(char *error, unsigned int errorSize, const char *message) {
    if (error == nullptr || errorSize == 0) {
        return;
    }
    strlcpy(error, message, errorSize);
}

bool fetchShellyReading(const char *url, uint32_t sequence, TelemetryReading &reading, char *error, unsigned int errorSize) {
    HTTPClient http;
    http.setTimeout(5000);

    if (!http.begin(url)) {
        setError(error, errorSize, "HTTP begin failed");
        return false;
    }

    const int status = http.GET();
    if (status != HTTP_CODE_OK) {
        http.end();
        setError(error, errorSize, "Shelly HTTP failed");
        return false;
    }

    const String body = http.getString();
    http.end();

    JsonDocument doc;
    const DeserializationError jsonError = deserializeJson(doc, body);
    if (jsonError) {
        setError(error, errorSize, "Shelly JSON failed");
        return false;
    }

    if (!doc["apower"].is<float>() || !doc["voltage"].is<float>() || !doc["current"].is<float>() ||
        !doc["freq"].is<float>() || !doc["aenergy"]["total"].is<float>()) {
        setError(error, errorSize, "Shelly fields missing");
        return false;
    }

    const float powerW = doc["apower"].as<float>();
    const float voltageV = doc["voltage"].as<float>();
    const float currentA = doc["current"].as<float>();

    reading.sequence = sequence;
    reading.powerW = powerW;
    reading.voltageV = voltageV;
    reading.currentA = currentA;
    const float apparentPowerVA = voltageV * currentA;
    reading.powerFactor = doc["pf"].is<float>() ? doc["pf"].as<float>() : (apparentPowerVA > 0.0f ? powerW / apparentPowerVA : 0.0f);
    reading.frequencyHz = doc["freq"].as<float>();
    reading.energyWh = doc["aenergy"]["total"].as<float>();
    setError(error, errorSize, "OK");
    return true;
}
