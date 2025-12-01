#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ====== PIN DEFINITIONS ======
// SPI (CAN)
#define SPI_MOSI      11
#define SPI_MISO      13
#define SPI_SCK       12
#define CAN1_CS       10
#define CAN2_CS       9
#define CAN1_TERM     15
#define CAN2_TERM     16

// I2C
#define I2C_SDA       3
#define I2C_SCL       2

// Temperature sensors
#define TEMP1_PIN     6
#define TEMP1_PULLUP  17
#define TEMP2_PIN     7
#define TEMP2_PULLUP  18

// PWM outputs
#define PWM1_PIN      4
#define PWM2_PIN      5

// UART
#define UART_TX       43
#define UART_RX       44

// ====== PCF8575 GPIO MAPPING ======
#define PCF8575_ADDR  0x20
// Outputs
#define PCF_AUX1      0
#define PCF_AUX2      1
#define PCF_AUX3      2
#define PCF_AUX4      3
// Inputs
#define PCF_DIN1      5
#define PCF_DIN2      6
#define PCF_DIN3      7
#define PCF_DIN4      8
#define PCF_DIN5      9
// LEDs
#define PCF_LED1      10
#define PCF_LED2      11
#define PCF_LED3      12
#define PCF_LED4      13
#define PCF_LED5      14
#define PCF_LED6      15

// ====== CONSTANTS ======
#define NTC_NOMINAL_RESISTANCE  10000.0f
#define NTC_NOMINAL_TEMP        25.0f
#define NTC_BETA                3950.0f
#define NTC_SERIES_RESISTOR     10000.0f

#define ADS1115_ADDR  0x48
#define ADC_VOLTAGE_SCALE  (5.0 / 32768.0)

// ====== CONFIGURATION STRUCT ======
struct Config {
  uint32_t can_bitrate = 500000;
  uint32_t pwm1_freq = 1000;
  uint32_t pwm2_freq = 1000;
  uint8_t pwm1_duty = 0;
  uint8_t pwm2_duty = 0;
  bool can1_term = false;
  bool can2_term = false;
  bool can1_enabled = true;   // Enable by default
  bool can2_enabled = true;   // Enable by default
  uint16_t update_interval = 500;
};

// ====== STATE STRUCT ======
struct State {
  // CAN
  uint32_t can1_tx_count = 0;
  uint32_t can1_rx_count = 0;
  uint32_t can2_tx_count = 0;
  uint32_t can2_rx_count = 0;
  
  // PCF8575
  uint16_t pcf_state = 0;
  bool pcf_available = false;
  
  // ADS1115
  float analog_voltage[3] = {0};
  bool ads_available = false;
  
  // Temperature
  float temp1 = 0;
  float temp2 = 0;
  
  // System
  uint32_t uptime = 0;
  uint32_t free_heap = 0;
  
  // Digital inputs
  bool din[5] = {false};
  
  // Aux outputs
  bool aux[4] = {false};
  
  // LEDs
  bool led[6] = {false};
};

// Global instances (extern declarations)
extern Config config;
extern State state;
extern String wifi_ssid;

#endif