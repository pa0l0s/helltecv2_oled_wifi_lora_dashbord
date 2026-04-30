#pragma once

#include "telemetry.h"

#include <Arduino.h>

bool setupDisplay();
void showBoot(const char *role);
void showServerStatus(const char *wifiStatus, const char *shellyStatus, const TelemetryReading *reading);
void showClientWaiting();
void showClientReading(const TelemetryReading &reading, int rssi, unsigned long ageMs);
void showError(const char *line1, const char *line2);
