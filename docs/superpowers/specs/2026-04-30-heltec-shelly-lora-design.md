# Heltec Shelly LoRa Design

## Goal

Build Arduino firmware for two Heltec WiFi LoRa 32 V2 boards. One board is the server: it polls a Shelly Plug S Gen3 every 30 seconds over Wi-Fi and sends power telemetry over raw LoRa at 868 MHz. The other board is the client: it receives LoRa telemetry and displays it.

## Architecture

The project uses one PlatformIO Arduino codebase with two build environments, `server` and `client`. Shared code handles telemetry serialization, LoRa setup, and OLED display helpers. The server-only path connects to Wi-Fi, calls the Shelly RPC endpoint, parses measured values, and transmits a readable packet. The client-only path listens for packets, parses them, and updates the OLED with power, total energy, packet age, and RSSI.

## Hardware And Protocol

Both boards use the Heltec WiFi LoRa 32 V2 pin mapping: OLED reset on GPIO 16, OLED I2C on GPIO 4/15, LoRa SPI pins SCK 5, MISO 19, MOSI 27, SS 18, reset 14, DIO0 26. The LoRa radio uses 868 MHz for Europe.

The Shelly server polls:

```text
http://192.168.60.96/rpc/Switch.GetStatus?id=0
```

It extracts `apower`, `voltage`, `current`, `pf`, `freq`, and `aenergy.total`.

The LoRa packet is readable ASCII:

```text
PWR1|seq=12|w=43.2|v=232.1|a=0.19|pf=0.91|hz=50.0|wh=1234.5
```

## Error Handling

If Wi-Fi is disconnected, the server reconnects and shows the state on OLED. If the Shelly request fails or returns incomplete JSON, the server shows an error and does not transmit a fresh telemetry packet for that cycle. The client keeps showing the last valid packet and displays packet age so stale data is visible.

## Testing

Telemetry formatting and parsing are pure C++ and tested with a native `g++` test binary. Hardware integration is verified by compiling with PlatformIO and then uploading `server` to one board and `client` to the other.
