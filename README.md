# Heltec Shelly LoRa Bridge

Arduino/PlatformIO firmware for two Heltec WiFi LoRa 32 V2 boards.

- `server`: connects to Wi-Fi, polls Shelly Plug S Gen3 at `http://192.168.60.96/rpc/Switch.GetStatus?id=0`, and sends telemetry every 30 seconds over raw LoRa 868 MHz.
- `client`: listens on LoRa 868 MHz and displays the latest power and total energy on the OLED.

## Configure

Edit `src/config.h` before flashing the server board:

```cpp
#define WIFI_SSID "your-wifi-name"
#define WIFI_PASSWORD "your-wifi-password"
#define SHELLY_STATUS_URL "http://192.168.60.96/rpc/Switch.GetStatus?id=0"
#define POLL_INTERVAL_MS 30000UL
```

The client does not use Wi-Fi.

## Build And Upload

Install PlatformIO, connect the board by USB, then upload the server firmware to the board near Wi-Fi/Shelly:

```bash
PLATFORMIO_CORE_DIR=.platformio .venv/bin/pio run -e server -t upload
PLATFORMIO_CORE_DIR=.platformio .venv/bin/pio device monitor -b 115200
```

Upload the client firmware to the second board:

```bash
PLATFORMIO_CORE_DIR=.platformio .venv/bin/pio run -e client -t upload
PLATFORMIO_CORE_DIR=.platformio .venv/bin/pio device monitor -b 115200
```

Do not use `/usr/bin/pio` on this machine. The distro-installed PlatformIO is too old for the installed Python Click package. Use `.venv/bin/pio` from this project.

## Browser Flashing

If WSL cannot see the USB serial device, use a browser ESP32 flasher from Windows Chrome or Edge.

For the server firmware, use the merged image:

```text
.pio/build/server/server-merged.bin
```

Flash it at offset:

```text
0x0
```

This merged image already contains the bootloader, partition table, Arduino boot app, and firmware.

## Native Test

The packet formatter/parser can be tested without PlatformIO:

```bash
g++ -std=c++17 -Isrc test/native/test_telemetry.cpp src/telemetry.cpp -o /tmp/telemetry_test
/tmp/telemetry_test
```

Expected output:

```text
telemetry tests passed
```

## LoRa Packet

Packets are readable text:

```text
PWR1|seq=12|w=43.2|v=232.1|a=0.19|pf=0.91|hz=50.0|wh=1234.5
```
