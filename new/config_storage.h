#ifndef CONFIG_STORAGE_H
#define CONFIG_STORAGE_H

#include <Arduino.h>
#include "gateway_config.h"

void initConfigStorage();
bool loadConfig();
bool saveConfig();
void resetToDefaults();
uint32_t calculateChecksum(const GatewayConfig& cfg);

#endif
