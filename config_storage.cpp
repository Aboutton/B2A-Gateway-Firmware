#include "config_storage.h"
#include <Preferences.h>

Preferences preferences;

uint32_t calculateChecksum(const GatewayConfig& cfg) {
  uint32_t sum = 0;
  const uint8_t* data = (const uint8_t*)&cfg;
  size_t size = sizeof(GatewayConfig) - sizeof(uint32_t); // Exclude checksum field
  
  for (size_t i = 0; i < size; i++) {
    sum += data[i];
  }
  return sum;
}

void initConfigStorage() {
  preferences.begin("gateway", false);
}

bool loadConfig() {
  if (preferences.isKey("config")) {
    size_t len = preferences.getBytesLength("config");
    if (len == sizeof(GatewayConfig)) {
      preferences.getBytes("config", &gw_config, sizeof(GatewayConfig));
      
      // Verify checksum
      uint32_t calc_checksum = calculateChecksum(gw_config);
      if (gw_config.checksum == calc_checksum && gw_config.magic == CONFIG_MAGIC) {
        Serial.println("✓ Configuration loaded from flash");
        return true;
      } else {
        Serial.println("✗ Config checksum mismatch - using defaults");
      }
    }
  }
  
  Serial.println("⚠ No valid config found - using defaults");
  resetToDefaults();
  return false;
}

bool saveConfig() {
  gw_config.checksum = calculateChecksum(gw_config);
  size_t written = preferences.putBytes("config", &gw_config, sizeof(GatewayConfig));
  
  if (written == sizeof(GatewayConfig)) {
    Serial.println("✓ Configuration saved to flash");
    return true;
  } else {
    Serial.println("✗ Failed to save configuration");
    return false;
  }
}

void resetToDefaults() {
  gw_config = GatewayConfig();  // Reset to default values
  Serial.println("⚙ Configuration reset to factory defaults");
  saveConfig();
}
