#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

#include "telemetry.h"

static bool closeTo(float actual, float expected) {
    return std::fabs(actual - expected) < 0.001f;
}

int main() {
    TelemetryReading reading{};
    reading.sequence = 12;
    reading.powerW = 43.2f;
    reading.voltageV = 232.1f;
    reading.currentA = 0.19f;
    reading.powerFactor = 0.91f;
    reading.frequencyHz = 50.0f;
    reading.energyWh = 1234.5f;

    const std::string packet = formatTelemetry(reading);
    assert(packet == "PWR1|seq=12|w=43.2|v=232.1|a=0.19|pf=0.91|hz=50.0|wh=1234.5");

    TelemetryReading parsed{};
    assert(parseTelemetry(packet, parsed));
    assert(parsed.sequence == 12);
    assert(closeTo(parsed.powerW, 43.2f));
    assert(closeTo(parsed.voltageV, 232.1f));
    assert(closeTo(parsed.currentA, 0.19f));
    assert(closeTo(parsed.powerFactor, 0.91f));
    assert(closeTo(parsed.frequencyHz, 50.0f));
    assert(closeTo(parsed.energyWh, 1234.5f));

    assert(!parseTelemetry("BAD|seq=12|w=43.2", parsed));
    assert(!parseTelemetry("PWR1|seq=12|w=43.2|v=232.1", parsed));

    std::cout << "telemetry tests passed\n";
    return 0;
}
