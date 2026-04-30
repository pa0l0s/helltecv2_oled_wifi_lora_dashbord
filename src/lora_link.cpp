#include "lora_link.h"

#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>

namespace {
constexpr int LORA_SCK = 5;
constexpr int LORA_MISO = 19;
constexpr int LORA_MOSI = 27;
constexpr int LORA_SS = 18;
constexpr int LORA_RST = 14;
constexpr int LORA_DIO0 = 26;
}

bool setupLoRa() {
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

    if (!LoRa.begin(LORA_FREQUENCY)) {
        return false;
    }

    LoRa.setSpreadingFactor(7);
    LoRa.setSignalBandwidth(125E3);
    LoRa.setCodingRate4(5);
    LoRa.enableCrc();
    return true;
}

bool sendLoRaPacket(const std::string &packet) {
    if (!LoRa.beginPacket()) {
        return false;
    }
    LoRa.print(packet.c_str());
    return LoRa.endPacket() == 1;
}
