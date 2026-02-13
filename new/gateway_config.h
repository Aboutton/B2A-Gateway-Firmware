#ifndef GATEWAY_CONFIG_H
#define GATEWAY_CONFIG_H

#include <Arduino.h>

// Trigger modes
#define TRIGGER_OFF       0
#define TRIGGER_CAN       1
#define TRIGGER_DIN       2
#define TRIGGER_ALWAYS_ON 3

// Route directions
#define ROUTE_CAN1_TO_CAN2  0
#define ROUTE_CAN2_TO_CAN1  1
#define ROUTE_BIDIRECTIONAL 2
#define ROUTE_CAN1_TO_CAN1  3   // Loopback: read from CAN1, modify, send back to CAN1
#define ROUTE_CAN2_TO_CAN2  4   // Loopback: read from CAN2, modify, send back to CAN2

// Route filter modes
#define FILTER_EXACT    0   // Match exact src_id
#define FILTER_MASK     1   // Match (msg.id & id_mask) == src_id
#define FILTER_RANGE    2   // Match src_id <= msg.id <= dst_id_range_end
#define FILTER_PASS_ALL 3   // Forward everything (src_id ignored)

// Config version for flash storage
#define CONFIG_VERSION 0x0301
#define CONFIG_MAGIC   0xB2A0

// AUX output trigger configuration
struct AuxTrigger {
  uint8_t mode = TRIGGER_OFF;
  uint32_t can_id = 0;
  uint8_t data_byte = 0;
  uint8_t data_value = 0;
  uint8_t data_mask = 0xFF;
  uint8_t digital_input = 0;  // 1-5
  bool invert = false;
};

// PWM output trigger configuration
struct PwmTrigger {
  uint8_t mode = TRIGGER_OFF;
  uint32_t can_id = 0;
  uint8_t data_byte = 0;
  float scale = 1.0;
  uint32_t frequency = 1000;
  uint8_t digital_input = 0;  // 1-5
  uint8_t always_duty = 0;
};

// Sensor broadcast configuration
struct SensorBroadcast {
  bool enabled = false;
  uint32_t can_id = 0;
  uint8_t start_byte = 0;
  float scale = 1.0;
  float offset = 0.0;
  uint8_t can_bus = 1;         // Which bus to broadcast on: 1=CAN1, 2=CAN2, 3=Both
  uint16_t interval = 100;     // Per-sensor broadcast interval in ms
};

// Data modification operation types
#define MODIFY_SET     0   // Set bits:    data[byte] |= value  (within mask)
#define MODIFY_CLEAR   1   // Clear bits:  data[byte] &= ~value (within mask)
#define MODIFY_REPLACE 2   // Replace:     data[byte] = (data[byte] & ~mask) | (value & mask)

// Single bit/byte modification rule
struct DataModify {
  bool enabled = false;
  uint8_t byte_index = 0;    // Which data byte (0-7)
  uint8_t bit_mask = 0xFF;   // Which bits to modify (bitmask)
  uint8_t value = 0;         // Value to set/apply
  uint8_t operation = MODIFY_REPLACE;  // SET, CLEAR, or REPLACE
};

// CAN routing rule
struct CanRoute {
  bool enabled = false;
  uint8_t direction = ROUTE_CAN1_TO_CAN2;
  uint8_t filter_mode = FILTER_EXACT;   // How to match incoming CAN IDs
  uint32_t src_id = 0;                  // Source ID (or base for mask/range)
  uint32_t id_mask = 0x7FF;             // Mask for FILTER_MASK mode
  uint32_t range_end = 0;               // End ID for FILTER_RANGE mode
  uint32_t dst_id = 0;                  // Remapped destination ID
  bool remap_id = false;                // If true, use dst_id instead of original
  uint16_t rate_limit = 0;              // ms between forwards (0=no limit, max 65535)
  bool allow_multi_match = false;       // If true, don't stop after this route matches

  // Data modifications — up to 4 per route
  DataModify modifications[4];

  // Runtime fields (not saved to flash)
  unsigned long last_forward = 0;
  uint32_t forward_count = 0;           // Per-route forwarded message counter
  uint32_t drop_count = 0;              // Per-route rate-limited drop counter
};

// Main gateway configuration
struct GatewayConfig {
  uint16_t magic = CONFIG_MAGIC;
  uint16_t version = CONFIG_VERSION;
  
  // CAN settings
  uint32_t can1_bitrate = 500000;
  uint32_t can2_bitrate = 500000;
  bool can1_term = false;
  bool can2_term = false;
  bool can1_enabled = true;
  bool can2_enabled = true;
  
  // Output triggers
  AuxTrigger aux[4];
  PwmTrigger pwm[2];
  
  // Sensor broadcasting
  SensorBroadcast analog[3];
  SensorBroadcast temp[2];
  uint16_t sensor_interval = 100;  // ms
  
  // CAN routing
  CanRoute routes[40];
  uint8_t route_count = 0;
  
  // Communication
  char wifi_password[32] = "B2A12345";
  char bt_name[32] = "B2A-Gateway-BT";
  bool wifi_enabled = true;
  bool bluetooth_enabled = true;
  
  uint32_t checksum = 0;
};

extern GatewayConfig gw_config;

#endif
