# Heltec Shelly LoRa Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build Arduino firmware for a Heltec server board that forwards Shelly Plug power data every 30 seconds to a Heltec client board over 868 MHz LoRa.

**Architecture:** One PlatformIO project has two build environments: `server` defines `DEVICE_ROLE_SERVER`, and `client` defines `DEVICE_ROLE_CLIENT`. Shared modules own telemetry packet parsing/formatting, display rendering, and LoRa setup. Server code owns Wi-Fi and Shelly HTTP polling; client code owns receive loop and stale-data display.

**Tech Stack:** PlatformIO, Arduino framework for ESP32, Heltec WiFi LoRa 32 V2, `LoRa`, `ArduinoJson`, `Adafruit_SSD1306`, `Adafruit_GFX`, native `g++` tests for shared C++.

---

## File Structure

- `platformio.ini`: PlatformIO environments and dependencies.
- `include/config.example.h`: User-editable Wi-Fi and Shelly settings template.
- `src/config.h`: Local default settings for the current workspace.
- `src/telemetry.h`, `src/telemetry.cpp`: Pure C++ packet model, formatter, parser.
- `src/lora_link.h`, `src/lora_link.cpp`: SX127x pin/frequency setup and packet send helper.
- `src/display.h`, `src/display.cpp`: OLED setup and rendering helpers.
- `src/shelly_client.h`, `src/shelly_client.cpp`: HTTP GET and Shelly JSON parsing.
- `src/main.cpp`: Role-specific Arduino setup/loop.
- `test/native/test_telemetry.cpp`: Native tests for telemetry packets.

## Tasks

- [x] Write failing native telemetry tests for format and parse behavior.
- [x] Run native test build and confirm it fails before implementation.
- [x] Implement telemetry formatter/parser.
- [x] Run native tests and confirm they pass.
- [x] Add PlatformIO Arduino project files.
- [x] Add LoRa, display, Shelly, and role-specific main firmware.
- [x] Verify native tests again.
- [x] Attempt PlatformIO build if available.
