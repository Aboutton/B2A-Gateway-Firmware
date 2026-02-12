#include "config_storage.h"
#include <Preferences.h>
#include <esp_crc.h>

Preferences preferences;

uint32_t calculateChecksum(const GatewayConfig& cfg) {
  const uint8_t* data = (const uint8_t*)&cfg;
  size_t size = sizeof(GatewayConfig) - sizeof(uint32_t); // Exclude checksum field
  return esp_crc32_le(0, data, size);
}

void initConfigStorage() {
  preferences.begin("gateway", false);
}

static void resetRuntimeFields() {
  // Reset non-persistent runtime fields after loading from flash
  for (uint8_t i = 0; i < 40; i++) {
    gw_config.routes[i].last_forward = 0;
    gw_config.routes[i].forward_count = 0;
    gw_config.routes[i].drop_count = 0;
  }
}

bool loadConfig() {
  if (preferences.isKey("config")) {
    size_t len = preferences.getBytesLength("config");
    if (len == sizeof(GatewayConfig)) {
      preferences.getBytes("config", &gw_config, sizeof(GatewayConfig));

      // Verify checksum
      uint32_t calc_checksum = calculateChecksum(gw_config);
      if (gw_config.checksum == calc_checksum && gw_config.magic == CONFIG_MAGIC) {
        resetRuntimeFields();
        Serial.println("Configuration loaded from flash");
        return true;
      } else {
        Serial.println("Config checksum mismatch - using defaults");
      }
    } else {
      Serial.printf("Config size mismatch (stored=%d, expected=%d) - using defaults\n",
        len, sizeof(GatewayConfig));
    }
  }

  Serial.println("No valid config found - using defaults");
  resetToDefaults();
  return false;
}

bool saveConfig() {
  gw_config.checksum = calculateChecksum(gw_config);
  size_t written = preferences.putBytes("config", &gw_config, sizeof(GatewayConfig));

  if (written == sizeof(GatewayConfig)) {
    Serial.println("Configuration saved to flash");
    return true;
  } else {
    Serial.println("Failed to save configuration");
    return false;
  }
}

void resetToDefaults() {
  gw_config = GatewayConfig();  // Reset to default values
  Serial.println("Configuration reset to factory defaults");
  saveConfig();
}
