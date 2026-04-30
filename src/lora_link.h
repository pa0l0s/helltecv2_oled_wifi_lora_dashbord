#pragma once

#include <string>

bool setupLoRa();
bool sendLoRaPacket(const std::string &packet);
