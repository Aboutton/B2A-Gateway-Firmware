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

// Config version for flash storage
#define CONFIG_VERSION 0x0100
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
};

// CAN routing rule
struct CanRoute {
  bool enabled = false;
  uint8_t direction = ROUTE_CAN1_TO_CAN2;
  uint32_t src_id = 0;
  uint32_t dst_id = 0;
  bool remap_id = false;
  uint8_t rate_limit = 0;  // ms between forwards (0=no limit)
  unsigned long last_forward = 0;
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
  char wifi_ssid[32] = "B2A-Gateway";
  char wifi_password[32] = "B2A12345";
  char bt_name[32] = "B2A-Gateway-BT";
  bool wifi_enabled = true;
  bool bluetooth_enabled = true;
  
  uint32_t checksum = 0;
};

extern GatewayConfig gw_config;

#endif
