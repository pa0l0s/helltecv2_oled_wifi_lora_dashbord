#pragma once

#include "telemetry.h"

bool fetchShellyReading(const char *url, uint32_t sequence, TelemetryReading &reading, char *error, unsigned int errorSize);
