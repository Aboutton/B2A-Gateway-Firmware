#include "sensor_broadcaster.h"
#include "can_bus.h"
#include "config.h"

void broadcastSensors() {
  uint8_t data[8] = {0};

  // Broadcast analog inputs
  for (uint8_t i = 0; i < 3; i++) {
    if (gw_config.analog[i].enabled) {
      memset(data, 0, 8);

      float value = state.analog_voltage[i];
      value = (value * gw_config.analog[i].scale) + gw_config.analog[i].offset;

      // Convert to 16-bit integer
      int16_t scaled = (int16_t)(value * 100);  // 2 decimal places

      uint8_t start = gw_config.analog[i].start_byte;
      if (start <= 6) {
        data[start] = (scaled >> 8) & 0xFF;
        data[start + 1] = scaled & 0xFF;

        canSend(1, gw_config.analog[i].can_id, false, data, 8);
      }
    }
  }

  // Broadcast temperature sensors
  for (uint8_t i = 0; i < 2; i++) {
    if (gw_config.temp[i].enabled) {
      float value = (i == 0) ? state.temp1 : state.temp2;
      value = (value * gw_config.temp[i].scale) + gw_config.temp[i].offset;

      // Convert to 16-bit integer
      int16_t scaled = (int16_t)(value * 10);  // 1 decimal place

      uint8_t start = gw_config.temp[i].start_byte;
      if (start <= 6) {
        memset(data, 0, 8);
        data[start] = (scaled >> 8) & 0xFF;
        data[start + 1] = scaled & 0xFF;

        canSend(1, gw_config.temp[i].can_id, false, data, 8);
      }
    }
  }
}
